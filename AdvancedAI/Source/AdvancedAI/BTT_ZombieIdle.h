// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ZombieIdle.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_ZombieIdle : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTT_ZombieIdle();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
