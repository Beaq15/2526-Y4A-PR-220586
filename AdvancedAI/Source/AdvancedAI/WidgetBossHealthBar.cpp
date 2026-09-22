// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetBossHealthBar.h"

void UWidgetBossHealthBar::UpdateHealthPercentage(float MaxHealth, float CurrentHealth)
{
	ProgressBar->SetPercent(CurrentHealth / MaxHealth);
}
