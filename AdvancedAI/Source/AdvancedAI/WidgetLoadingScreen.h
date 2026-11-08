// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	void StartFakeLoad();

	UPROPERTY(BlueprintAssignable, Category = "Loading")
	FOnFakeLoadComplete OnLoadComplete;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PercentText;

	UPROPERTY(EditAnywhere, Category = "Loading", meta = (ClampMin = "0.1"))
	float LoadDuration = 3.f;

private:
	float Elapsed = 0.f;
	bool bRunning = false;
};
