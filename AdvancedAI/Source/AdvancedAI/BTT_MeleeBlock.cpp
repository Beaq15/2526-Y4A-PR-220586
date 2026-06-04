// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MeleeBlock.h"
#include "EnemyMelee.h"
#include "AIController.h"

UBTT_MeleeBlock::UBTT_MeleeBlock()
{
	NodeName = "MeleeBlock";
}

EBTNodeResult::Type UBTT_MeleeBlock::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyMelee* Enemy = Cast<AEnemyMelee>(OwnerComp.GetAIOwner()->GetPawn());

	if (!Enemy)
		return EBTNodeResult::Failed;

	OwnerCompRef = &OwnerComp;

	Enemy->StartBlock();
	Enemy->OnBlockEnd.AddDynamic(this, &UBTT_MeleeBlock::EndExecution);

	return EBTNodeResult::InProgress;
}

void UBTT_MeleeBlock::EndExecution()
{
	if (OwnerCompRef)
		FinishLatentTask(*OwnerCompRef, EBTNodeResult::Succeeded);
}
