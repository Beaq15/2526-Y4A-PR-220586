// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetHealthBar.h"
#include "AIController.h"
#include "AdvancedAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

float UWidgetHealthBar::GetPercent() const
{
	if (!DamageableActor) return 0;

	float MaxHealth = IDamageableInterface::Execute_GetMaxHealth(DamageableActor.GetObject());
	float CurrentHealth = IDamageableInterface::Execute_GetCurrentHealth(DamageableActor.GetObject());

	return CurrentHealth / MaxHealth;
}

ESlateVisibility UWidgetHealthBar::GetHealthBarVisibility() const
{

	if (!DamageableActor)
		return ESlateVisibility::Collapsed;

	AActor* OwnerActor = Cast<AActor>(DamageableActor.GetObject());
	if (!OwnerActor) return ESlateVisibility::Collapsed;

	APawn* OwnerPawn = Cast<APawn>(OwnerActor);
	if (!OwnerPawn) return ESlateVisibility::Collapsed;

	AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
	if (!AIController) return ESlateVisibility::Collapsed;

	UBlackboardComponent* BB = AIController->GetBlackboardComponent();
	if (!BB) return ESlateVisibility::Collapsed;

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TEXT("AttackTarget")));
	bool bTargetingPlayer = TargetActor && TargetActor->IsA<AAdvancedAICharacter>();

	return bTargetingPlayer ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
}
