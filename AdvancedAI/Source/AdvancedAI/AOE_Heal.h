// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AOE_Heal.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAOEOverlapActor, AActor*, Actor);

UCLASS()
class ADVANCEDAI_API AAOE_Heal : public AActor
{
	GENERATED_BODY()
	
public:	
	AAOE_Heal();
	virtual void Tick(float DeltaTime) override;

	void Trigger();
	void SpawnAOESphere();
	void DestroyActorByTimer();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(BlueprintAssignable)
	FOnAOEOverlapActor OnAOEOverlapActor;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<UParticleSystem> P_HealAura;

	UPROPERTY(EditDefaultsOnly)
	float Radius = 300.f;
	UPROPERTY(EditDefaultsOnly)
	bool DrawDebugSphere = false;
	UPROPERTY(EditDefaultsOnly)
	bool IgnoreInstigator = false;
	UPROPERTY(EditDefaultsOnly)
	bool TriggerOnBeginPlay = false;
	UPROPERTY(EditDefaultsOnly)
	float Duration = 5.f;
	UPROPERTY(EditDefaultsOnly)
	float SpawnInterval = 0.0f;

	FTimerHandle DestroyTimerHandle;
	FTimerHandle SpawnIntervalTimerHandle;



protected:
	virtual void BeginPlay() override;
};
