// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageableInterface.h"
#include "Components/ProgressBar.h"
#include "WidgetHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UWidgetHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	float GetPercent();

	UPROPERTY(BlueprintReadWrite)
	TScriptInterface<IDamageableInterface> DamageableActor;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar_40;
	
};
