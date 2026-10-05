// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageableInterface.generated.h"

USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Amount = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeBlocked = false;
};

UENUM(BlueprintType)
enum class EDamageResult : uint8
{
	BlockDamage,
	DoDamage,
	NoDamage
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ADVANCEDAI_API IDamageableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	float GetCurrentHealth();

	UFUNCTION(BlueprintNativeEvent)
	float GetMaxHealth();

	UFUNCTION(BlueprintNativeEvent)
	float Heal(float Amount);

	UFUNCTION(BlueprintNativeEvent)
	bool TakeDamage(const FDamageInfo& DamageInfo, AActor* DamageCauser);

	UFUNCTION(BlueprintNativeEvent)
	bool IsDead();

	UFUNCTION(BlueprintNativeEvent)
	bool IsAttacking();

	UFUNCTION(BlueprintNativeEvent)
	bool ReserveAttackToken(int32 Amount);

	UFUNCTION(BlueprintNativeEvent)
	void ReturnAttackToken(int32 Amount);

	UFUNCTION(BlueprintNativeEvent)
	void SetIsInterruptable(bool Value);

	UFUNCTION(BlueprintNativeEvent)
	int GetTeamNumber();
};
