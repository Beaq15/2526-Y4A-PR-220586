// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetPlayerHUD.h"
#include "AdvancedAICharacter.h"

void UWidgetPlayerHUD::ShowCrosshair()
{
	Crosshair->SetVisibility(ESlateVisibility::Visible);
}

void UWidgetPlayerHUD::HideCrosshair()
{
	Crosshair->SetVisibility(ESlateVisibility::Hidden);
}

float UWidgetPlayerHUD::GetPercent() const
{
	AAdvancedAICharacter* Character = Cast<AAdvancedAICharacter>(Player);

	if (!Character) return 0.f;

	float MaxHealth = IDamageableInterface::Execute_GetMaxHealth(Character);
	float CurrentHealth = IDamageableInterface::Execute_GetCurrentHealth(Character);

	return CurrentHealth / MaxHealth;
}
