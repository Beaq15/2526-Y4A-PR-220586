// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_MeleeBlock.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_MeleeBlock : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_MeleeBlock();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UFUNCTION()
	void EndExecution();

	UBehaviorTreeComponent* OwnerCompRef = nullptr;
};
