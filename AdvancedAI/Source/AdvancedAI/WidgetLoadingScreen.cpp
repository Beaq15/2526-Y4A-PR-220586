// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetLoadingScreen.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/AssetManager.h"

void UWidgetLoadingScreen::StartLoad()
{
	Elapsed = 0.f;
	TargetProgress = 0.f;
	DisplayedProgress = 0.f;
	bRunning = true;

	if (AssetsToPreload.Num() > 0)
	{
		TArray<FSoftObjectPath> Paths;
		for (const TSoftObjectPtr<UObject>& Asset : AssetsToPreload)
		{
			Paths.Add(Asset.ToSoftObjectPath());
		}
		AssetHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(Paths);
	}

	ProgressBar->SetPercent(0.f);
}

float UWidgetLoadingScreen::GetAssetProgress() const
{
	if (!AssetHandle.IsValid() || AssetHandle->HasLoadCompleted())
	{
		return 1.f;
	}
	return AssetHandle->GetProgress();
}

void UWidgetLoadingScreen::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bRunning)
	{
		return;
	}

	Elapsed += InDeltaTime;

	TargetProgress = FMath::Max(TargetProgress, GetAssetProgress());

	DisplayedProgress = FMath::FInterpTo(DisplayedProgress, TargetProgress, InDeltaTime, BarInterpSpeed);
	ProgressBar->SetPercent(DisplayedProgress);

	if (PercentText)
	{
		PercentText->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), FMath::RoundToInt(DisplayedProgress * 100.f))));
	}

	const bool bAllDone = TargetProgress >= 1.f && DisplayedProgress >= 0.995f;
	if (bAllDone && Elapsed >= MinDisplayTime)
	{
		bRunning = false;
		ProgressBar->SetPercent(1.f);
		OnLoadComplete.Broadcast();
	}
}