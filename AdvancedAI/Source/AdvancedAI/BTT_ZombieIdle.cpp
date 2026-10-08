// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ZombieIdle.h"
#include "EnemyZombie.h"
#include "AIC_Enemy_Base.h"

UBTT_ZombieIdle::UBTT_ZombieIdle()
{
	NodeName = "ZombieIdle";
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_ZombieIdle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyZombie* Zombie = Cast<AEnemyZombie>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Zombie) return EBTNodeResult::Failed;

	Zombie->Idle();

	return EBTNodeResult::InProgress;
}