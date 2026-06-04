// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_IsTargetAttacking.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTD_IsTargetAttacking : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTD_IsTargetAttacking();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, Category = "Keys")
	FBlackboardKeySelector AttackTargetKey;
	
};
