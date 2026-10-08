// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ZombieScream.h"
#include "AIC_Enemy_Base.h"
#include "BehaviorTree/BlackboardComponent.h"

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
				if (UBlackboardComponent* BB = CachedOwnerComp->GetBlackboardComponent())
				{
					BB->SetValueAsBool(HasScreamedKey.SelectedKeyName, true);
				}

				FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
				CachedOwnerComp = nullptr;
			}
		};
	return EBTNodeResult::InProgress;
}