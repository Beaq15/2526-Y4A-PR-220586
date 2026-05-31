// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "GameplayTagContainer.h"
#include "BTS_CheckFact.generated.h"

/**
 * 
 */
UCLASS()
class AITEAMCOMMS_API UBTS_CheckFact : public UBTService
{
	GENERATED_BODY()

public:
    UBTS_CheckFact();

protected:
    // Runs every tick (or interval) while the branch is active
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    /** Which fact type we are monitoring */
    UPROPERTY(EditAnywhere, Category = "Fact")
    FGameplayTag RequiredFact;

    /** Blackboard key to reset when the fact is gone */
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector AnnouncedFactKey;
	
};
