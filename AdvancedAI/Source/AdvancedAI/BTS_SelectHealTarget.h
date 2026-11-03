// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_SelectHealTarget.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTS_SelectHealTarget : public UBTService
{
	GENERATED_BODY()
	
public:
    UBTS_SelectHealTarget();

    UPROPERTY(EditAnywhere, Category = "Heal")
    FName HealTargetKeyName = "HealTarget";

    UPROPERTY(EditAnywhere, Category = "Heal")
    float SupportRadius = 1500.f;

    UPROPERTY(EditAnywhere, Category = "Heal")
    float MinRequestConfidence = 0.35f;

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
