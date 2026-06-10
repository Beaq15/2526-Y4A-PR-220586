// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MeleeBlock.h"
#include "EnemyMelee.h"
#include "AIController.h"

UBTT_MeleeBlock::UBTT_MeleeBlock()
{
	NodeName = "Melee Block";
}

EBTNodeResult::Type UBTT_MeleeBlock::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyMelee* Enemy = Cast<AEnemyMelee>(OwnerComp.GetAIOwner()->GetPawn());

	if (!Enemy) return EBTNodeResult::Failed;

	CachedOwnerComp = &OwnerComp;

	Enemy->StartBlock();
	Enemy->OnBlockEnd.AddDynamic(this, &UBTT_MeleeBlock::OnBlockEnd);

	return EBTNodeResult::InProgress;
}

void UBTT_MeleeBlock::OnBlockEnd()
{
	if (!CachedOwnerComp)
		return;

	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	CachedOwnerComp = nullptr;
}
