#include "AssetCleanupLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "EditorAssetLibrary.h"
#include "Misc/FileHelper.h"
#include "Modules/ModuleManager.h"
 
FAssetCleanupResult UAssetCleanupLibrary::_lastCleanupResult;
 
bool UAssetCleanupLibrary::CleanupAssets(const FString& Folder, const FString& OwnerName, TArray<FString> TagsToApply, const FString& ReportOutputPath)
{
    for (const FAssetData& Asset : GetAssetsInFolder(Folder))
    {
        UObject* LoadedAsset = Asset.GetAsset();

        // Change owner name
        UEditorAssetLibrary::SetMetadataTag(LoadedAsset, FName(TEXT("Owner")), OwnerName);

        // Apply new tags
        for (const FString& Tag : TagsToApply)
        {
            UEditorAssetLibrary::SetMetadataTag(LoadedAsset, FName(*Tag), OwnerName);
        }

        // Save asset
        UEditorAssetLibrary::SaveLoadedAsset(LoadedAsset);

        // Notify 
        GetMutableDefault<UAssetCleanupLibrary>()->OnAssetSaved.Broadcast(Asset.GetObjectPathString());

        FAssetModification Modification;
        Modification.AssetPath = Asset.GetObjectPathString();
        Modification.OwnerName = OwnerName;
        Modification.Tags = TagsToApply;

        // Record modification
        _lastCleanupResult.AssetsChanged.Emplace(Modification);
    }

    return ExportReport(ReportOutputPath);
}

TArray<FAssetData> UAssetCleanupLibrary::GetAssetsInFolder(const FString& Folder)
{
    FAssetRegistryModule& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    TArray<FAssetData> Assets; AssetRegistry.Get().GetAssetsByPath(FName(*Folder), Assets, true); return Assets;
}
 
bool UAssetCleanupLibrary::ExportReport(const FString& OutputPath)
{
    // Headers
    FString Csv = TEXT("AssetPath,Owner,Tags\n");
    
    for (const FAssetModification& Modification : _lastCleanupResult.AssetsChanged)
    {
        // Build Tags list
        FString Tags;
        for (int32 i = 0; i < Modification.Tags.Num(); i++)
        {
            Tags += Modification.Tags[i];
            if (i < Modification.Tags.Num() - 1)
                Tags += TEXT(",");
        }

        // Add entry
        Csv += FString::Printf(TEXT("%s,%s,%s\n"),
            *Modification.AssetPath,
            *Modification.OwnerName,
            *Tags);

        // Save CSV
        FFileHelper::SaveStringToFile(Csv, *OutputPath);
    }

    return true;
}