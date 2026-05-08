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
	
public:	
	// Sets default values for this actor's properties
	APatrolRoute();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void IncrementPatrolRoute();

	UFUNCTION()
	FVector GetSplinePointAsWorldPosition();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	

	UPROPERTY(VisibleAnywhere)
	USplineComponent* PatrolSpline;

	UPROPERTY(VisibleAnywhere)
	int32 PatrolIndex = 0;

	UPROPERTY(VisibleAnywhere)
	int32 Direction = 1;

};
