// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetIsInterruptable.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_SetIsInterruptable : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_SetIsInterruptable();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
	bool Value = false;
};
