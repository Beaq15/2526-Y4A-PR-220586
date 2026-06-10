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

	UPROPERTY(EditAnywhere, Category = "Keys")
	FBlackboardKeySelector IdealRangeKey;

	UPROPERTY(EditAnywhere, Category = "Keys")
	FBlackboardKeySelector AttackTargetKey;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float ErrorMargin = 50.f;

public:
	UBTD_IsWithingIdealRange();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
