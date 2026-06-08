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

/**
 * 
 */
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
public:
	AAIC_Enemy_Base();

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

	AActor* AttackTargetActor;

	TArray<AActor*> KnownSeenActors;

	void CheckIfForgottonSeenActor();

	FTimerHandle ForgottenActorTimerHandle;

protected:

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(1); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerception;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Damage* DamageConfig;

private:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

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


	UFUNCTION()
	void StartBehaviorTree(AEnemyBase* Enemy);

	UFUNCTION()
	bool CanSenseActor(AActor* Actor, EAISense Sense, FAIStimulus& OutStimulus);

	bool CanSenseActor(AActor* Actor, EAISense Sense)
	{
		FAIStimulus Dummy;
		return CanSenseActor(Actor, Sense, Dummy);
	}

	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UFUNCTION()
	void HandleSensedSight(AActor* Actor);

	UFUNCTION()
	void HandleSensedSound(FVector Location);

	UFUNCTION()
	void HandleSensedDamage(AActor* Actor);

	UFUNCTION()
	void HandleForgotActor(AActor* Actor);

	UFUNCTION()
	uint8 GetCurrentState();
	
};
