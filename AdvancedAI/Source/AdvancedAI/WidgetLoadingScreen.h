// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/StreamableManager.h"
#include "WidgetLoadingScreen.generated.h"

class UProgressBar;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFakeLoadComplete);

UCLASS()
class ADVANCEDAI_API UWidgetLoadingScreen : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void StartLoad();

	UPROPERTY(BlueprintAssignable, Category = "Loading")
	FOnFakeLoadComplete OnLoadComplete;

	TSharedPtr<FStreamableHandle> GetAssetHandle() const { return AssetHandle; }

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PercentText;

	UPROPERTY(EditAnywhere, Category = "Loading")
	TArray<TSoftObjectPtr<UObject>> AssetsToPreload;

	UPROPERTY(EditAnywhere, Category = "Loading")
	float MinDisplayTime = 3.f;

	UPROPERTY(EditAnywhere, Category = "Loading")
	float BarInterpSpeed = 6.f;

private:
	float GetAssetProgress() const;

	TSharedPtr<FStreamableHandle> AssetHandle;
	float Elapsed = 0.f;
	float TargetProgress = 0.f;
	float DisplayedProgress = 0.f;
	bool bRunning = false;
};