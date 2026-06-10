// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "PatrolRoute.generated.h"

UENUM()
enum class EMovementSpeed : uint8
{
	Idle,
	Walking,
	Jogging,
	Sprinting
};

UCLASS()
class ADVANCEDAI_API APatrolRoute : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = "Patrol")
	TObjectPtr<USplineComponent> PatrolSpline;

	int32 PatrolIndex = 0;
	int32 Direction = 1;

public:	
	APatrolRoute();

	void IncrementPatrolRoute();
	FVector GetSplinePointAsWorldPosition() const;

protected:
	virtual void BeginPlay() override;
};
