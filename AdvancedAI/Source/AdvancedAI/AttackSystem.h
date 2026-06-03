// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectileBase.h"
#include "DamageSystem.h"
#include "AttackSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnded);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVANCEDAI_API UAttackSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttackSystem();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void MagicSpell(FTransform SpawnTransform, AActor* TargetActor, FDamageInfo DamageInfo);

	UFUNCTION()
	void FireBullet(FVector TraceStart, FVector TraceEnd, FDamageInfo DamageInfo);

	UFUNCTION()
	void OnProjectileHit(AActor* OtherActor, FHitResult Hit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDamageInfo DamageInfoRef;

	UPROPERTY(BlueprintAssignable)
	FOnAttackEnded OnAttackEnd;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AProjectileBase> ProjectileClass;

};