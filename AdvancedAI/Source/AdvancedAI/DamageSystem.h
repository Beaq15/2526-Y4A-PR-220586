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
	// Sets default values for this actor's properties
	UDamageSystem();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FOnBlocked OnBlocked;

	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable)
	FOnDamageResponse OnDamageResponse;

	float Health = 100.f;
	float MaxHealth = 100.f;
	bool isDead = false;
	bool isInvincible = false;
	bool isBlocking = false;
	bool isInterruptible = false;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


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
