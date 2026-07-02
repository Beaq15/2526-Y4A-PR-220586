// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_MageAttack.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_MageAttack : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Keys")
	FBlackboardKeySelector AttackTargetKey;

	UPROPERTY(EditAnywhere, Category = "Keys")
	FBlackboardKeySelector TeleportLocationKey;

	UPROPERTY(EditAnywhere)
	int32 TokensNeeded;

public:
	UBTT_MageAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
