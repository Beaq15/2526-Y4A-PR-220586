// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "WidgetPlayerDeath.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UWidgetPlayerDeath : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ButtonRestart;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ButtonExit;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()
	void OnExitClicked();
};
