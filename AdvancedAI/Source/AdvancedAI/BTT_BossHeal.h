// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_BossHeal.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_BossHeal : public UBTTaskNode
{
	GENERATED_BODY()
	

	UBehaviorTreeComponent* CachedOwnerComp;

public:

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HasBossHealedKey;

	UBTT_BossHeal();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
