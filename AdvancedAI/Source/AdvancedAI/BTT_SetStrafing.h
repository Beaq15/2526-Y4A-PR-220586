// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetStrafing.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_SetStrafing : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_SetStrafing();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Strafing")
	bool bStrafingValue = false;
};
