// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnemyZombie.h"
#include "BTT_ZombieScream.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_ZombieScream : public UBTTaskNode
{
	GENERATED_BODY()
	
public:

	UBTT_ZombieScream();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY() 
	TObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;

	FDelegateHandle ScreamFinishedHandle;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HasScreamedKey;
};
