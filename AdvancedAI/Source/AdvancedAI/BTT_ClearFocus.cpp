// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ClearFocus.h"
#include "AIController.h"

UBTT_ClearFocus::UBTT_ClearFocus()
{
	NodeName = "Clear Focus";
}

EBTNodeResult::Type UBTT_ClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	AIController->ClearFocus(EAIFocusPriority::Gameplay);

	return EBTNodeResult::Succeeded;
}
