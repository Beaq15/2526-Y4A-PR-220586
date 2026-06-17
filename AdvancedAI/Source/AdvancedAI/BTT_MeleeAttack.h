// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "BTT_MeleeAttack.generated.h"

class AEnemyBase;
/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_MeleeAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Keys")
	FBlackboardKeySelector AttackTargetKey;

	UPROPERTY(EditAnywhere, Category = "Keys")
	FBlackboardKeySelector AttackRadiusKey;

	UPROPERTY(EditAnywhere)
	int32 TokensNeeded;

	UPROPERTY()
	TObjectPtr<AEnemyBase> CachedEnemy;

	UPROPERTY()
	TObjectPtr<AActor> CachedTarget;

	UBehaviorTreeComponent* CachedOwnerComp = nullptr;

	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
public:

	UBTT_MeleeAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
