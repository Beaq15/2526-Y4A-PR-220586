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
	
public:	
	// Sets default values for this actor's properties
	AProjectileBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FOnProjectileImpact OnProjectileImpact;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Speed = 500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Gravity = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool IsHoming = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	AActor* Target = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UBoxComponent* BoxCollision = nullptr;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UParticleSystem* ImpactEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USoundBase* SpawnSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USoundBase* ImpactSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UProjectileMovementComponent* ProjectileMovement = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UArrowComponent* Arrow = nullptr;

	UFUNCTION()
	void RotateToTarget();

	UFUNCTION()
	void PlayImpactSound(FVector Location);

	UFUNCTION()
	void PlaySpawnSound();

	UFUNCTION()
	void SpawnImpactEffect(FVector Location);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
