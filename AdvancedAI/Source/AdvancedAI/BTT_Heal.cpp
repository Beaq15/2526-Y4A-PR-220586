// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Heal.h"
#include "AIController.h"
#include "EnemyInterface.h"

UBTT_Heal::UBTT_Heal()
{
	NodeName = "Heal";
}

EBTNodeResult::Type UBTT_Heal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();

	if (Pawn->Implements<UEnemyInterface>())
	{
		IEnemyInterface::Execute_Heal(Pawn, HealPercentage);
	}
	return EBTNodeResult::Succeeded;
}
