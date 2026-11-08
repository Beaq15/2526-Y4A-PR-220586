// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetLoadingScreen.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UWidgetLoadingScreen::StartFakeLoad()
{
	Elapsed = 0.f;
	bRunning = true;

	ProgressBar->SetPercent(0.f);
	if (PercentText)
	{
		PercentText->SetText(FText::FromString(TEXT("0%")));
	}
}

void UWidgetLoadingScreen::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bRunning)
	{
		return;
	}

	Elapsed += InDeltaTime;
	const float Progress = FMath::Clamp(Elapsed / LoadDuration, 0.f, 1.f);

	ProgressBar->SetPercent(Progress);
	if (PercentText)
	{
		PercentText->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), FMath::RoundToInt(Progress * 100.f))));
	}

	if (Progress >= 1.f)
	{
		bRunning = false;
		OnLoadComplete.Broadcast();
	}
}