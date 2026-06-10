// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolRoute.h"

// Sets default values
APatrolRoute::APatrolRoute()
{
	PrimaryActorTick.bCanEverTick = true;

	PatrolSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PatrolSpline"));
	PatrolSpline->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APatrolRoute::BeginPlay()
{
	Super::BeginPlay();
	
}

void APatrolRoute::IncrementPatrolRoute()
{
	PatrolIndex += Direction;

	if (PatrolIndex == (PatrolSpline->GetNumberOfSplinePoints() - 1))
		Direction = -1;
	else if (PatrolIndex == 0)
		Direction = 1;
}

FVector APatrolRoute::GetSplinePointAsWorldPosition() const
{
	return PatrolSpline->GetLocationAtSplinePoint(PatrolIndex, ESplineCoordinateSpace::World);
}
