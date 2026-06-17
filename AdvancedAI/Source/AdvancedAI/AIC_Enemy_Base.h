// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "EnemyBase.h"
#include "GenericTeamAgentInterface.h"
#include "AIC_Enemy_Base.generated.h"

UENUM(BlueprintType)
enum class EAISense : uint8
{
	None,
	Sight,
	Hearing,
	Damage
};

UCLASS()
class ADVANCEDAI_API AAIC_Enemy_Base : public AAIController
{
	GENERATED_BODY()

	//----------------------------------------------------------------------
	// Private — Blackboard Key Names
	//----------------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName AttackTargetKeyName = "AttackTarget";

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName StateKeyName = "State";

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName PointOfInterestKeyName = "PointOfInterest";

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName AttackRadiusKeyName = "AttackRadius";

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName DefendRadiusKeyName = "DefendRadius";

	//----------------------------------------------------------------------
	// Private — Timers
	//----------------------------------------------------------------------

	FTimerHandle ForgottenActorTimerHandle;
	FTimerHandle SeekAttackTargetTimer;

	//----------------------------------------------------------------------
	// Private — Perception Handlers
	//----------------------------------------------------------------------

	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	void HandleSensedSight(AActor* Actor);
	void HandleSensedSound(FVector Location);
	void HandleSensedDamage(AActor* Actor);
	void HandleForgotActor(AActor* Actor);
	void HandleLostSight(AActor* Actor);

	//----------------------------------------------------------------------
	// Private — Internal Helpers
	//----------------------------------------------------------------------

	void StartBehaviorTree(AEnemyBase* Enemy);
	void CheckIfForgottonSeenActor();

	bool CanSenseActor(AActor* Actor, EAISense Sense, FAIStimulus& OutStimulus);
	bool CanSenseActor(AActor* Actor, EAISense Sense)
	{
		FAIStimulus Dummy;
		return CanSenseActor(Actor, Sense, Dummy);
	}

	UFUNCTION()
	void SeekAttackTarget();

	UFUNCTION()
	EAIState GetCurrentState() const;

public:
	//----------------------------------------------------------------------
	// Public — Lifecycle
	//----------------------------------------------------------------------
	AAIC_Enemy_Base();

	//----------------------------------------------------------------------
	// Public — State API
	//----------------------------------------------------------------------

	UFUNCTION()
	void SetStateAsPassive();

	UFUNCTION()
	void SetStateAsDead();

	UFUNCTION()
	void SetStateAsFrozen();

	UFUNCTION()
	void SetStateAsAttacking(AActor* AttackTarget, bool UseLastKnownAttackTarget);

	UFUNCTION()
	void SetStateAsInvestigating(FVector Location);

	UFUNCTION()
	void SetStateAsSeeking(FVector Location);

	//----------------------------------------------------------------------
	// Public — State (read access for external actors)
	//----------------------------------------------------------------------

	UPROPERTY()
	TObjectPtr<AActor> AttackTargetActor;

	TArray<AActor*> KnownSeenActors;

	UFUNCTION()
	void OnEnemyAttackEnd();

	FSimpleDelegate OnAttackEndDelegate;

protected:
	//----------------------------------------------------------------------
	// Protected — Lifecycle Overrides
	//----------------------------------------------------------------------

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	//----------------------------------------------------------------------
	// Protected — Team
	//----------------------------------------------------------------------
	virtual FGenericTeamId GetGenericTeamId() const override;

	bool OnSameTeam(AActor* OtherActor);

	//----------------------------------------------------------------------
	// Protected — Perception Components
	//----------------------------------------------------------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception")
	TObjectPtr<UAIPerceptionComponent> AIPerception;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception")
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception")
	TObjectPtr<UAISenseConfig_Damage> DamageConfig;
};
