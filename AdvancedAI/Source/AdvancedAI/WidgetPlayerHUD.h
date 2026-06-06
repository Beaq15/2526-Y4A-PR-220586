// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "DamageableInterface.h"
#include "WidgetPlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UWidgetPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UImage* Crosshair;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar_148;

	UPROPERTY(VisibleAnywhere)
	ACharacter* Player;

	UFUNCTION(BlueprintPure)
	float GetPercent();

	UFUNCTION()
	void ShowCrosshair();

	UFUNCTION()
	void HideCrosshair();
};
