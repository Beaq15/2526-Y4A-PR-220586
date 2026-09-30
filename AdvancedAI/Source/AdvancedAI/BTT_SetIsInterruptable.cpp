// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SetIsInterruptable.h"
#include "DamageableInterface.h"
#include "AIC_Enemy_Base.h"

UBTT_SetIsInterruptable::UBTT_SetIsInterruptable()
{
	NodeName = "Set IsInterruptable";
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_SetIsInterruptable::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIC_Enemy_Base* AIController = Cast<AAIC_Enemy_Base>(OwnerComp.GetOwner());
	AEnemyBase* ControllerPawn = Cast<AEnemyBase>(AIController->GetPawn());

	IDamageableInterface::Execute_SetIsInterruptable(ControllerPawn, Value);

	return EBTNodeResult::Succeeded;
}
