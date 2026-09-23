// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectileBase.h"
#include "DamageSystem.h"
#include "Animation/AnimMontage.h"
#include "AOE_Base.h"
#include "AttackSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnded);

USTRUCT(BlueprintType)
struct FAttackInfo
{
	GENERATED_BODY()

	AActor* AttackTarget;
	UAnimMontage* Montage;
	FDamageInfo DamageInfo;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVANCEDAI_API UAttackSystem : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AProjectileBase> ProjectileClass;

	FDamageInfo DamageInfoRef;

	UFUNCTION()
	void OnProjectileHit(AActor* OtherActor, FHitResult Hit);

	UFUNCTION()
	void AOEDamage(AActor* AttackTarget, float Radius, FDamageInfo DamageInfo);

	UPROPERTY()
	TObjectPtr<AAOE_Base> AOE;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ActorToSpawn;

	UFUNCTION()
	void AOEDamageActor(AActor* Actor);

	UPROPERTY()
	TObjectPtr<AActor> CachedAttackTarget;

	UPROPERTY()
	FDamageInfo CachedDamageInfo;

	UPROPERTY()
	FAttackInfo CachedAttackInfo;

	UPROPERTY()
	float CachedRadius;

	UFUNCTION()
	void OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted);
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

	void GroundSmash(FAttackInfo AttackInfo, float Radius);

	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

	//----------------------------------------------------------------------
	// Public — Delegates
	//----------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable)
	FOnAttackEnded OnAttackEnd;
};