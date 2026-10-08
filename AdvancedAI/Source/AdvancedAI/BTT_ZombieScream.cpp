// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ZombieScream.h"
#include "AIC_Enemy_Base.h"
UBTT_ZombieScream::UBTT_ZombieScream()
{
	NodeName = "ZombieScream";
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_ZombieScream::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyZombie* Zombie = Cast<AEnemyZombie>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Zombie) return EBTNodeResult::Failed;

	Zombie->Scream();

	CachedOwnerComp = &OwnerComp;
	Zombie->OnScreamEndCallback = [this]()
		{
			if (CachedOwnerComp)
			{
				FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
				CachedOwnerComp = nullptr;
			}
		};
	return EBTNodeResult::InProgress;
}