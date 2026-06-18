// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_RangedAttack.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_RangedAttack : public UBTTaskNode
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category = "Keys")
	FBlackboardKeySelector AttackTargetKey;

	UPROPERTY(EditAnywhere)
	int32 TokensNeeded;
public:

	UBTT_RangedAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
