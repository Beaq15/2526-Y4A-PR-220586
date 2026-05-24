// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetHealthBar.h"

float UWidgetHealthBar::GetPercent()
{
	if (!DamageableActor) return 0;

	float MaxHealth = IDamageableInterface::Execute_GetMaxHealth(DamageableActor.GetObject());
	float CurrentHealth = IDamageableInterface::Execute_GetCurrentHealth(DamageableActor.GetObject());

	return CurrentHealth / MaxHealth;
}
