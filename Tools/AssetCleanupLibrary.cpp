#include "AssetCleanupLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "EditorAssetLibrary.h"
#include "Misc/FileHelper.h"
#include "Modules/ModuleManager.h"
 
namespace
{
    TArray<FAssetData> GetAssetsInFolder(const FString& Folder)
    {
        FAssetRegistryModule& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
        TArray<FAssetData> Assets; AssetRegistry.Get().GetAssetsByPath(FName(*Folder), Assets, true); return Assets;
    }
}
 
FAssetCleanupResultV4 UAssetCleanupLibraryV4::LastResult;
 
FAssetCleanupResultV4 UAssetCleanupLibraryV4::CleanupAssets(const FString& Folder, const FString& RequiredPrefix, const FString& OwnerName, bool bSaveAssets)
{
    LastResult = {};
    for (const FAssetData& Asset : GetAssetsInFolder(Folder))
    {
        UObject* LoadedAsset = Asset.GetAsset();
        if (LoadedAsset == nullptr) { continue; }
        if (!RequiredPrefix.IsEmpty() && !LoadedAsset->GetName().StartsWith(RequiredPrefix))
        {
            const FString NewName = FString::Printf(TEXT("%s_%s"), *RequiredPrefix, *LoadedAsset->GetName());
            const FString NewPath = FString::Printf(TEXT("%s/%s"), *Folder, *NewName);
            if (UEditorAssetLibrary::RenameAsset(Asset.GetObjectPathString(), NewPath))
            {
                ++LastResult.RenamedAssetCount;
            }
        }
        if (!OwnerName.IsEmpty())
        {
            UEditorAssetLibrary::SetMetadataTag(LoadedAsset, FName(TEXT("Owner")), OwnerName); ++LastResult.TaggedAssetCount;
        }
        LastResult.TouchedAssets.Add(Asset.GetObjectPathString());
        if (bSaveAssets) { UEditorAssetLibrary::SaveAsset(Asset.GetObjectPathString(), false); }
    }
    return LastResult;
}

void UAssetCleanupLibraryV4::initialize()
{
    OnAssetRenamed.BindUObject(this, &UAssetCleanupLibraryV4::HandleRenameEvent);
}

static void UAssetCleanupLibraryV4::HandleRenameEvent(FString AssetPath)
{
    UE_LOG(LogTemp, Log, TEXT("Renamed: %s"), *AssetPath);
}

void UAssetCleanupLibraryV4::BatchTagAssets(
    TArray<FAssetData> AssetsToProcess,
    TArray<FString>    TagsToApply,
    const FString&     OwnerName)
{
    for (const FAssetData& Asset : AssetsToProcess)
    {
        FString assetPath = Asset.GetObjectPathString();
        int32   tagCount  = TagsToApply.Num();
 
        UObject* LoadedAsset = Asset.GetAsset();
        if (LoadedAsset == nullptr) { continue; }
 
        for (const FString& Tag : TagsToApply)
        {
            UEditorAssetLibrary::SetMetadataTag(LoadedAsset, FName(*Tag), OwnerName);
        }
 
        UE_LOG(LogTemp, Log, TEXT("Tagged %s with %d tags"), *assetPath, tagCount);
    }
}
 
void UAssetCleanupLibraryV4::CollectAssetPaths(const TArray<FAssetData>& Assets, TArray<FString>& OutPaths)
{
    for (const FAssetData& Asset : Assets)
    {
        OutPaths.Add(Asset.GetObjectPathString());
    }
}
 
bool UAssetCleanupLibraryV4::ExportDependencyReport(const FString& Folder, const FString& OutputPath)
{
    FString Csv = TEXT("AssetPath,Owner,ReferencerCount\n");
    for (const FAssetData& Asset : GetAssetsInFolder(Folder))
    {
        UObject* LoadedAsset = Asset.GetAsset();
        const FString OwnerName = LoadedAsset != nullptr ? UEditorAssetLibrary::GetMetadataTag(LoadedAsset, FName(TEXT("Owner"))) : TEXT("");
        Csv += FString::Printf(TEXT("%s,%s,%d\n"), *Asset.GetObjectPathString(), *OwnerName, UEditorAssetLibrary::FindPackageReferencersForAsset(Asset.GetObjectPathString(), false).Num());
        FFileHelper::SaveStringToFile(Csv, *OutputPath);
    }
    LastResult.LastReportPath = OutputPath;
    return true;
}
 
FAssetCleanupResult UAssetCleanupLibraryV4::GetLastResult() { return LastResult; }