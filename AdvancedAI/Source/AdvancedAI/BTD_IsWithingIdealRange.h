// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_IsWithingIdealRange.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTD_IsWithingIdealRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTD_IsWithingIdealRange();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Keys")
	FBlackboardKeySelector IdealRangeKey;

	UPROPERTY(EditAnywhere, Category = "Keys")
	FBlackboardKeySelector AttackTargetKey;
	
};
