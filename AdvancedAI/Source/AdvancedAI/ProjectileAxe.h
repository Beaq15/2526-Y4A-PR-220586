// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "Components/TimelineComponent.h"
#include "ProjectileAxe.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API AProjectileAxe : public AProjectileBase
{
	GENERATED_BODY()

	FTimeline SpinTimeline;

	UPROPERTY()
	TObjectPtr<UCurveFloat> LinearCurve;

	UFUNCTION()
	void OnSpinTimeLineUpdate(float Value);

public:
	AProjectileAxe();

	void SpinAxe();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RotationPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UParticleSystemComponent> TrailEffect;
};
