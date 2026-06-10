// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_StopAttackingIfTargetIsDead.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTS_StopAttackingIfTargetIsDead : public UBTService
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Keys")
	FBlackboardKeySelector AttackTargetKey;
	
public:
	UBTS_StopAttackingIfTargetIsDead();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
