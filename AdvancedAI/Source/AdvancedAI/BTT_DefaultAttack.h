// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnemyBase.h"
#include "BTT_DefaultAttack.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_DefaultAttack : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Keys")
	FBlackboardKeySelector AttackTargetKey;

	UBehaviorTreeComponent* CachedOwnerComp = nullptr;

	UFUNCTION()
	void OnAttackEnd();
public:
	UBTT_DefaultAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
