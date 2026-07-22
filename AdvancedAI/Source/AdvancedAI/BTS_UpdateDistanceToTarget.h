// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_UpdateDistanceToTarget.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTS_UpdateDistanceToTarget : public UBTService
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Keys")
	FBlackboardKeySelector AttackTargetKey;

	UPROPERTY(EditAnywhere, Category = "Keys")
	FBlackboardKeySelector DistanceToTargetKey;

public:
	UBTS_UpdateDistanceToTarget();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
