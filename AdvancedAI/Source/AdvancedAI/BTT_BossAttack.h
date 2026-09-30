// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnemyBoss.h"
#include "BTT_BossAttack.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_BossAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	EBoss_Attacks AttackName;

	UPROPERTY(EditAnywhere, Category = "Keys")
	FBlackboardKeySelector AttackTargetKey;

	UPROPERTY(EditAnywhere)
	int32 TokensNeeded;

	UBehaviorTreeComponent* CachedOwnerComp = nullptr;

	void AttackBasedOnName(AEnemyBoss* BossRef);

public:
	UBTT_BossAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
