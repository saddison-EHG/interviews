#pragma once
 
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AssetCleanupLibrary.generated.h"

DECLARE_DELEGATE_OneParam(FOnAssetRenamed, FString);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAssetTagged, FString, AssetPath);
 
USTRUCT(BlueprintType)
struct FAssetCleanupResult
{
    GENERATED_BODY()
 
public:
    UPROPERTY(BlueprintReadOnly, Category = "Asset Cleanup")
    int32 RenamedAssetCount = 0;
 
    UPROPERTY(BlueprintReadOnly, Category = "Asset Cleanup")
    int32 TaggedAssetCount = 0;
 
    UPROPERTY(BlueprintReadOnly, Category = "Asset Cleanup")
    FString LastReportPath;
 
    UPROPERTY(BlueprintReadOnly, Category = "Asset Cleanup")
    TArray<FString> TouchedAssets;
};
 
UCLASS()
class CODEREVIEW_API UAssetCleanupLibraryV4 : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
 
    static FAssetCleanupResult LastResult;
 
public:

    FOnAssetTagged OnAssetTagged;
    
    UPROPERTY(BlueprintAssignable, Category = "Asset Cleanup")
    FOnAssetTagged OnAssetSaved;
 
    UFUNCTION(BlueprintCallable, Category = "Asset Cleanup")
    static FAssetCleanupResult CleanupAssets(
        const FString& Folder,
        const FString& RequiredPrefix,
        const FString& OwnerName,
        bool bSaveAssets);
    
    UFUNCTION(BlueprintCallable, Category = "Asset Cleanup")
    static void BatchTagAssets(
        TArray<FAssetData> AssetsToProcess,
        TArray<FString>    TagsToApply,
        const FString&     OwnerName);
    
    UFUNCTION(BlueprintCallable, Category = "Asset Cleanup")
    static void CollectAssetPaths(
        const TArray<FAssetData>& Assets,
        TArray<FString>&          OutPaths);
 
    UFUNCTION(BlueprintCallable, Category = "Asset Cleanup")
    static bool ExportDependencyReport(
        const FString& Folder,
        const FString& OutputPath);
 
    UFUNCTION(BlueprintPure, Category = "Asset Cleanup")
    static FAssetCleanupResult GetLastResult();
    
    UPROPERTY(EditAnywhere, Category = "Asset Cleanup")
    bool StrictNamingEnabled = false;
    
    UPROPERTY(EditAnywhere, Category = "Asset Cleanup")
    FString defaultOwnerName;
    
    UPROPERTY(EditAnywhere, Category = "Asset Cleanup")
    TArray<FString> m_cachedAssetPaths;
    
    UPROPERTY(EditAnywhere, Category = "Asset Cleanup")
    int32 MaxRenameWarnings = 50;
};