// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AOE_Base.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAOEOverlapActor, AActor*, Actor);

UCLASS()
class ADVANCEDAI_API AAOE_Base : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAOE_Base();

	void Trigger();
	void SpawnAOESphere();
	void DestroyActorByTimer();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

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

	UPROPERTY(BlueprintAssignable)
	FOnAOEOverlapActor OnAOEOverlapActor;

	FTimerHandle DestroyTimerHandle;
	FTimerHandle SpawnIntervalTimerHandle;
};
