// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_IsHealthBelowTreshold.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTD_IsHealthBelowTreshold : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTD_IsHealthBelowTreshold();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HealthTreshold = 0.f;
	
};
