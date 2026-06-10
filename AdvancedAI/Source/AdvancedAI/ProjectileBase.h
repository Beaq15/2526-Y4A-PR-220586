// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ArrowComponent.h"
#include "ProjectileBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProjectileImpact, AActor*, OtherActor, FHitResult, Hit);

UCLASS()
class ADVANCEDAI_API AProjectileBase : public AActor
{
	GENERATED_BODY()

	//----------------------------------------------------------------------
	// Private — Callbacks
	//----------------------------------------------------------------------

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void RotateToTarget();
	void PlayImpactSound(FVector Location) const;
	void PlaySpawnSound() const;
	void SpawnImpactEffect(FVector Location) const;
	
public:	
	//----------------------------------------------------------------------
	// Public — Lifecycle
	//----------------------------------------------------------------------
	AProjectileBase();

	//----------------------------------------------------------------------
	// Public — Delegates
	//----------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable)
	FOnProjectileImpact OnProjectileImpact;

	//----------------------------------------------------------------------
	// Public — Config (set externally before BeginPlay)
	//----------------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile")
	float Speed = 500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile")
	float Gravity = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile")
	bool bIsHoming = false;

	UPROPERTY(BlueprintReadWrite, Category = "Projectile")
	TObjectPtr<AActor> Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BoxCollision;

protected:
	//----------------------------------------------------------------------
	// Protected — Lifecycle
	//----------------------------------------------------------------------

	virtual void BeginPlay() override;

	//----------------------------------------------------------------------
	// Protected — Components
	//----------------------------------------------------------------------

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UArrowComponent> Arrow;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	//----------------------------------------------------------------------
	// Protected — Assets
	//----------------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<UParticleSystem> ImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> SpawnSound;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> ImpactSound;
};
