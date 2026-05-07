// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FocusTarget.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_FocusTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FocusTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "Keys")
	FBlackboardKeySelector AttackTargetKey;
	
};
