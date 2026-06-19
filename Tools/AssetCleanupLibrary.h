#pragma once
 
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AssetCleanupLibrary.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAssetSaved,  const FString&, AssetPath);

USTRUCT(BlueprintType)
struct FAssetModification
{
    GENERATED_BODY()
 
public:
    UPROPERTY(BlueprintReadOnly, Category = "Asset Cleanup")
    FString AssetPath;
 
    UPROPERTY(BlueprintReadOnly, Category = "Asset Cleanup")
    FString OwnerName;
 
    UPROPERTY(BlueprintReadOnly, Category = "Asset Cleanup")
    TArray<FString> Tags;
};

USTRUCT(BlueprintType)
struct FAssetCleanupResult
{
    GENERATED_BODY()
 
public:
    UPROPERTY(BlueprintReadOnly, Category = "Asset Cleanup")
    TArray<FAssetModification> AssetsChanged;
};
 
UCLASS()
class UAssetCleanupLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
 
    static FAssetCleanupResult _lastCleanupResult;
    
public:
    
    UPROPERTY(BlueprintAssignable, Category = "Asset Cleanup")
    FOnAssetSaved OnAssetSaved;
 
    UFUNCTION(BlueprintCallable, Category = "Asset Cleanup")
    static bool CleanupAssets(const FString& Folder,
        const FString& OwnerName,
        TArray<FString> TagsToApply,
        const FString& ReportOutputPath);

private:

    UFUNCTION(BlueprintCallable, Category = "Asset Cleanup")
    static TArray<FAssetData> GetAssetsInFolder(const FString& Folder);
 
    UFUNCTION(BlueprintCallable, Category = "Asset Cleanup")
    static bool ExportReport(const FString& OutputPath);
};