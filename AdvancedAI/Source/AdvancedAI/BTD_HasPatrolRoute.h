// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_HasPatrolRoute.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTD_HasPatrolRoute : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTD_HasPatrolRoute();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
