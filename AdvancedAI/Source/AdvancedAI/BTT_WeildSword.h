// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_WeildSword.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_WieldSword : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_WieldSword();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
