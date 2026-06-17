// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Passive, //0
	Attacking, //1
	Frozen, //2
	Investigating, //3 
	Dead, //4
	Seeking //5
};

UENUM()
enum class EMovementSpeed : uint8
{
	Idle,
	Walking,
	Jogging,
	Sprinting
};

UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

class ADVANCEDAI_API IEnemyInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	APatrolRoute* GetPatrolRoute();

	UFUNCTION(BlueprintNativeEvent)
	float SetMovementSpeed(EMovementSpeed Speed);

	UFUNCTION(BlueprintNativeEvent)
	void GetIdealRange(float& AttackRadius, float& DefendRadius);

	UFUNCTION(BlueprintNativeEvent)
	void EquipWeapon();

	UFUNCTION(BlueprintNativeEvent)
	void UnequipWeapon();

	UFUNCTION(BlueprintNativeEvent)
	void JumpToDestination(FVector Destination);

	UFUNCTION(BlueprintNativeEvent)
	void Attack(AActor* AttackTarget);

	UFUNCTION(BlueprintNativeEvent)
	bool DidAttackStart(AActor* AttackTarget, int32 Amount);

	UFUNCTION(BlueprintNativeEvent)
	void AttackEnd(AActor* AttackTarget);

	UFUNCTION(BlueprintNativeEvent)
	void StoreAttackTokens(AActor* AttackTarget, int32 Amount);
};
