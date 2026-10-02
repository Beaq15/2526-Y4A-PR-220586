// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileAxe.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"

AProjectileAxe::AProjectileAxe()
{
	PrimaryActorTick.bCanEverTick = true;

	RotationPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RotationPoint"));
	RotationPoint->SetupAttachment(BoxCollision);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(RotationPoint);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TrailEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailEffect"));
	TrailEffect->SetupAttachment(StaticMeshComp);
	TrailEffect->bAutoActivate = true;
}

void AProjectileAxe::BeginPlay()
{
	Super::BeginPlay();

	// TIMELINE
	LinearCurve = NewObject<UCurveFloat>(this);
	LinearCurve->FloatCurve.AddKey(0.0f, 0.0f);
	LinearCurve->FloatCurve.AddKey(0.5f, -360.0f);

	FOnTimelineFloat UpdateDelegate;
	UpdateDelegate.BindUFunction(this, FName("OnSpinTimeLineUpdate"));
	SpinTimeline.AddInterpFloat(LinearCurve, UpdateDelegate);

	SpinTimeline.SetLooping(true);

	SpinAxe();
}

void AProjectileAxe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SpinTimeline.TickTimeline(DeltaTime);
}

void AProjectileAxe::SpinAxe()
{
	SpinTimeline.Play();
}

void AProjectileAxe::OnSpinTimeLineUpdate(float Value)
{
	const FRotator Current = RotationPoint->GetComponentRotation();
	RotationPoint->SetWorldRotation(FRotator(Current.Pitch, Value, Current.Roll));
}