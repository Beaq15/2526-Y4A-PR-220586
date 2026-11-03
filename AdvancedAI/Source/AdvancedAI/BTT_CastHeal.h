// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_CastHeal.generated.h"

class AAOE_Heal;

UCLASS()
class ADVANCEDAI_API UBTT_CastHeal : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
    UBTT_CastHeal();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
};
