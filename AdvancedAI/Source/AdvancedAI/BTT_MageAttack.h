// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnemyMage.h"
#include "BTT_MageAttack.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UBTT_MageAttack : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Attack")
	EMage_Attacks AttackName;

	UPROPERTY(EditAnywhere, Category = "Keys")
	FBlackboardKeySelector AttackTargetKey;

	UPROPERTY(EditAnywhere, Category = "Keys")
	FBlackboardKeySelector TeleportLocationKey;

	UPROPERTY(EditAnywhere)
	int32 TokensNeeded;

	UPROPERTY(EditAnywhere)
	bool bShouldTeleport = true;

	UBehaviorTreeComponent* CachedOwnerComp = nullptr;

public:
	UBTT_MageAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
