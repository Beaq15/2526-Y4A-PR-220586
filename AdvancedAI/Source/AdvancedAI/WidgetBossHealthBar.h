// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "WidgetBossHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UWidgetBossHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateHealthPercentage(float MaxHealth, float CurrentHealth);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;
};
