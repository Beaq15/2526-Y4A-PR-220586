// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetHealthBar.h"
#include "AIController.h"
#include "AdvancedAICharacter.h"

float UWidgetHealthBar::GetPercent() const
{
	if (!DamageableActor) return 0;

	float MaxHealth = IDamageableInterface::Execute_GetMaxHealth(DamageableActor.GetObject());
	float CurrentHealth = IDamageableInterface::Execute_GetCurrentHealth(DamageableActor.GetObject());

	return CurrentHealth / MaxHealth;
}

ESlateVisibility UWidgetHealthBar::GetHealthBarVisibility() const
{
	UE_LOG(LogTemp, Warning, TEXT("GetHealthBarVisibility CALLED"));

	if (!DamageableActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("DamageableActor is NULL"));
		return ESlateVisibility::Collapsed;
	}

	AActor* OwnerActor = Cast<AActor>(DamageableActor.GetObject());
	if (!OwnerActor) return ESlateVisibility::Collapsed;

	AAIController* AIController = Cast<AAIController>(Cast<APawn>(OwnerActor)->GetController());
	if (!AIController) return ESlateVisibility::Collapsed;

	AActor* FocusActor = AIController->GetFocusActor();
	bool bTargetingPlayer = FocusActor && FocusActor->IsA<AAdvancedAICharacter>();

	UE_LOG(LogTemp, Warning, TEXT("%s: bTargetingPlayer = %s"),
		*OwnerActor->GetName(),
		bTargetingPlayer ? TEXT("TRUE") : TEXT("FALSE"));

	return bTargetingPlayer ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
}
