// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PatrolRoute.h"
#include "EnemyInterface.h"
#include "BehaviorTree/BehaviorTree.h"
#include "DamageableInterface.h"
#include "DamageSystem.h"
#include "Components/WidgetComponent.h"
#include "WidgetHealthBar.h"
#include "AITeamComms/Public/AIKnowledgeComponent.h"
#include "AITeamComms/Public/AIPerceptionToFactComponent.h"
#include "AttackSystem.h"
#include "EnemyBase.generated.h"

class AAIC_Enemy_Base;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponEquippedEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponUnequipedEnd);

UCLASS()
class ADVANCEDAI_API AEnemyBase : public ACharacter, public IEnemyInterface, public IDamageableInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	//----------------------------------------------------------------------
	// Public — Lifecycle
	//----------------------------------------------------------------------

	AEnemyBase();

	virtual void Tick(float DeltaTime) override;

	//----------------------------------------------------------------------
	// Public — Delegates
	//----------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Animation")
	FOnAttackEnd OnAttackEnd;

	UPROPERTY(BlueprintAssignable, Category = "Animation")
	FOnWeaponEquippedEnd OnEquipWeaponEnd;

	UPROPERTY(BlueprintAssignable, Category = "Animation")
	FOnWeaponUnequipedEnd OnDropWeaponEnd;

	//----------------------------------------------------------------------
	// Public — State
	//----------------------------------------------------------------------

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	bool bIsWieldingWeapon = false;

	UPROPERTY(VisibleANywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsStrafing = false;

	UPROPERTY()
	EAIState State = EAIState::Passive;

	//----------------------------------------------------------------------
	// Public — AI Asset
	//----------------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	//----------------------------------------------------------------------
	// Public Combat
	//----------------------------------------------------------------------

	virtual void Attack() {}

	//----------------------------------------------------------------------
	// Protected — Team
	//----------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	uint8 TeamId = 1;
	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(TeamId); }

protected:
	//----------------------------------------------------------------------
	// Protected — Lifecycle
	//----------------------------------------------------------------------

	virtual void BeginPlay() override;

	//----------------------------------------------------------------------
	// Protected — Components
	//----------------------------------------------------------------------

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UDamageSystem> DamageSystem;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UAttackSystem> AttackSystem;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UAIKnowledgeComponent> KnowledgeComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UAIPerceptionToFactComponent> PerceptionToFactComponent;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UWidgetComponent> HealthBarComponent;

	//----------------------------------------------------------------------
	// Protected — Config
	//----------------------------------------------------------------------

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<APatrolRoute> PatrolRouteActor;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UWidgetHealthBar> HealthBarWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AActor> WeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> HitReactionMontage;

	//----------------------------------------------------------------------
	// Protected — Runtime State
	//----------------------------------------------------------------------

	UPROPERTY()
	TObjectPtr<AAIC_Enemy_Base> AICEnemyBase;

	UPROPERTY()
	TObjectPtr<AActor> WeaponActor;

	UPROPERTY()
	TObjectPtr<AActor> CachedDamageCauser;

	bool bAttacking = false;

	//----------------------------------------------------------------------
	// Protected — Tokens
	//----------------------------------------------------------------------

	UPROPERTY()
	TMap<TObjectPtr<AActor>, int32> ReservedAttackTokens;

	int32 TokensUsedInCurrentAttack;

	//----------------------------------------------------------------------
	// Protected — Callbacks
	//----------------------------------------------------------------------

	UFUNCTION()
	void OnHitReactionMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnFactReceived(FSharedFact Fact);

	UFUNCTION()
	void OnDeath_Event();

	UFUNCTION()
	void OnHitResponse_Event(EDamageResponse DamageResponse, AActor* DamageCauser);

	//----------------------------------------------------------------------
	// Protected — IEnemyInterface
	//----------------------------------------------------------------------

	virtual void Attack_Implementation(AActor* AttackTarget) override;
	virtual float      SetMovementSpeed_Implementation(EMovementSpeed Speed) override;
	virtual APatrolRoute* GetPatrolRoute_Implementation() override;
	virtual void       GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius) override;
	virtual void       JumpToDestination_Implementation(FVector Destination) override;
	virtual bool DidAttackStart_Implementation(AActor* AttackTarget, int32 Amount) override;
	virtual void StoreAttackTokens_Implementation(AActor* AttackTarget, int32 Amount) override;
	virtual void AttackEnd_Implementation(AActor* AttackTarget) override;

	//----------------------------------------------------------------------
	// Protected — IDamageableInterface
	//----------------------------------------------------------------------
	virtual float GetCurrentHealth_Implementation() override { return DamageSystem->Health; }
	virtual float GetMaxHealth_Implementation()     override { return DamageSystem->MaxHealth; }
	virtual bool  IsDead_Implementation()           override { return DamageSystem->isDead; }
	virtual bool  IsAttacking_Implementation()      override { return bAttacking; }
	virtual float Heal_Implementation(float Amount) override;
	virtual bool  TakeDamage_Implementation(const FDamageInfo& DamageInfo, AActor* DamageCauser) override;
	virtual bool ReserveAttackToken_Implementation(int32 Amount) override;
	virtual void ReturnAttackToken_Implementation(int32 Amount) override;
};