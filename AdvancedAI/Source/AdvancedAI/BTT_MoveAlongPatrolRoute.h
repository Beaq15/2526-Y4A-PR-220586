// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_MoveAlongPatrolRoute.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_MoveAlongPatrolRoute : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_MoveAlongPatrolRoute();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	FVector CurrentTargetLocation = FVector::ZeroVector;
	float AcceptanceRadius = 10.0f;
};
