// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Heal.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_Heal : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HealPercentage = 0.f;
	
public:
	UBTT_Heal();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
