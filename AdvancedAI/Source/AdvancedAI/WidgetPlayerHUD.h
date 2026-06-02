// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
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

	UFUNCTION()
	void ShowCrosshair();

	UFUNCTION()
	void HideCrosshair();
};
