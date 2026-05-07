// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_WeildSword.h"
#include "EnemyBase.h"
#include "AIController.h"

UBTT_WieldSword::UBTT_WieldSword()
{
	NodeName = "Wield Sword";
}

EBTNodeResult::Type UBTT_WieldSword::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Enemy)  return EBTNodeResult::Failed;

	Enemy->WieldSword();

	return EBTNodeResult::Succeeded;
}
