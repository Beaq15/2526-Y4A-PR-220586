// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_Chance.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTD_Chance : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTD_Chance();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float Percent = 0.5f;

};
