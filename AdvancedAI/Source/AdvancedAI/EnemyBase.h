// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PatrolRoute.h"
#include "EnemyInterface.h"
#include "GenericTeamAgentInterface.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyBase.generated.h"

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Passive, //0
	Attacking, //1
	Frozen, //2
	Investigating, //3 
	Dead //4
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponEquippedEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponUnequipedEnd);

UCLASS()
class ADVANCEDAI_API AEnemyBase : public ACharacter, public IEnemyInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Animation")
	FOnAttackEnd OnAttackEnd;

	UPROPERTY(BlueprintAssignable, Category = "Animation")
	FOnWeaponEquippedEnd OnEquipWeaponEnd;

	UPROPERTY(BlueprintAssignable, Category = "Animation")
	FOnWeaponUnequipedEnd OnDropWeaponEnd;

	// Sets default values for this character's properties
	AEnemyBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	bool bIsWieldingWeapon = false;

	UPROPERTY(VisibleANywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsStrafing = false;

	UPROPERTY()
	EAIState State = EAIState::Passive;

	virtual void Attack();

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(1); }

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AActor>WeaponClass;
	UPROPERTY()
	TObjectPtr<AActor> WeaponActor;

	UPROPERTY(EditAnywhere, Category = "AI")
	APatrolRoute* PatrolRoute;

	virtual float SetMovementSpeed_Implementation(EMovementSpeed Speed) override;
	virtual APatrolRoute* GetPatrolRoute_Implementation() override;

	virtual void GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius) override;

	float GetCurrentHealth_Implementation() { return Health; }

	float GetMaxHealth_Implementation() { return MaxHealth; }

	void Heal_Implementation(float HealPercentage);

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	float Health = 100.f;
	float MaxHealth = 100.f;
	bool isDead = false;
};
