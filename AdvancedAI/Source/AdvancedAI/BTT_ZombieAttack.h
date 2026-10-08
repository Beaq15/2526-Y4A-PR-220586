// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "EnemyZombie.h"
#include "BTT_ZombieAttack.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_ZombieAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
    UBTT_ZombieAttack();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector AttackTargetKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector AttackRadiusKey;

    UPROPERTY(EditAnywhere, Category = "Attack")
    int32 TokensNeeded = 1;

private:
    UFUNCTION()
    void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

    UPROPERTY()
    TObjectPtr<AEnemyZombie> CachedEnemy;

    UPROPERTY()
    TObjectPtr<AActor> CachedTarget;

    UBehaviorTreeComponent* CachedOwnerComp = nullptr;
};
