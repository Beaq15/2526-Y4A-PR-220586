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

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AProjectileBase> ProjectileClass;

	FDamageInfo DamageInfoRef;

	UFUNCTION()
	void OnProjectileHit(AActor* OtherActor, FHitResult Hit);
public:	
	//----------------------------------------------------------------------
	// Public — Lifecycle
	//----------------------------------------------------------------------
	// 

	UAttackSystem();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//----------------------------------------------------------------------
	// Public — Attack API
	//----------------------------------------------------------------------

	void MagicSpell(FTransform SpawnTransform, AActor* TargetActor, FDamageInfo DamageInfo);
	void FireBullet(FVector TraceStart, FVector TraceEnd, FDamageInfo DamageInfo);
	TArray<AActor*> DamageAllNonTeamMembers(FDamageInfo DamageInfo, TArray<FHitResult> Hits);
	AActor* DamageFirstNonTeamMember(FDamageInfo DamageInfo, TArray<FHitResult> Hits);

	//----------------------------------------------------------------------
	// Public — Delegates
	//----------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable)
	FOnAttackEnded OnAttackEnd;
};