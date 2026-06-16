// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageableInterface.h"
#include "DamageSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBlocked, bool, bCanBeParried, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageResponse, EDamageResponse, DamageResponse, AActor*, DamageCauser);

UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class ADVANCEDAI_API UDamageSystem : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	//----------------------------------------------------------------------
	// Public — Lifecycle
	//----------------------------------------------------------------------
	UDamageSystem();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//----------------------------------------------------------------------
	// Public — Delegates
	//----------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable)
	FOnBlocked OnBlocked;

	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable)
	FOnDamageResponse OnDamageResponse;

	//----------------------------------------------------------------------
	// Public — State
	//----------------------------------------------------------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	float Health = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State")
	bool isDead = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State")
	bool isInvincible = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State")
	bool isBlocking = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State")
	bool isInterruptible = true;

	//----------------------------------------------------------------------
	// Public — Tokens
	//----------------------------------------------------------------------

	bool ReserveAttackToken(int32 Amount);
	void ReturnAttackToken(int32 Amount);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tokens")
	int32 AttackTokensCount = 1;

	//----------------------------------------------------------------------
	// Public — API
	//----------------------------------------------------------------------

	FORCEINLINE EDamageResult CanBeDamaged(bool bShouldDamageInvincible, bool bCanBeBlocked) const
	{
		if (!isDead && (!isInvincible || bShouldDamageInvincible))
		{
			if (isBlocking && bCanBeBlocked)
				return EDamageResult::BlockDamage;
			else
				return EDamageResult::DoDamage;
		}
		else
			return EDamageResult::NoDamage;
	}

	float Heal(float Amount);
	bool TakeDamage(const FDamageInfo& DamageInfo, AActor* DamageCauser);

};
