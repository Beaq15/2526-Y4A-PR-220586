// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackSystem.h"
#include "DamageableInterface.h"
#include "Perception/AISense_Damage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EnemyInterface.h"
#include "GameFramework/Character.h"

//----------------------------------------------------------------------
// Lifecycle
//----------------------------------------------------------------------
// 

UAttackSystem::UAttackSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAttackSystem::BeginPlay()
{
	Super::BeginPlay();
}

void UAttackSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

//----------------------------------------------------------------------
// Attack API
//----------------------------------------------------------------------

void UAttackSystem::MagicSpell(FTransform SpawnTransform, AActor* TargetActor, FDamageInfo DamageInfo)
{
	if (!ProjectileClass) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());

	AProjectileBase* Projectile = GetWorld()->SpawnActor<AProjectileBase>(ProjectileClass, SpawnTransform, SpawnParams);
	if (!Projectile) return;

	Projectile->BoxCollision->IgnoreActorWhenMoving(GetOwner(), true);
	Projectile->Speed = 1000.f;
	Projectile->Target = TargetActor;

	DamageInfoRef = DamageInfo;

	Projectile->OnProjectileImpact.AddDynamic(this, &UAttackSystem::OnProjectileHit);
}

void UAttackSystem::FireBullet(FVector TraceStart, FVector TraceEnd, FDamageInfo DamageInfo)
{
	TArray<FHitResult> OutHits;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	bool bHit = UKismetSystemLibrary::LineTraceMultiForObjects(this, TraceStart, TraceEnd, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHits, true);

	if (bHit)
	{
		for (FHitResult Hit : OutHits)
		{
			AActor* HitActor = Hit.GetActor();

			IDamageableInterface::Execute_TakeDamage(HitActor, DamageInfo, GetOwner());
			UAISense_Damage::ReportDamageEvent(GetWorld(), HitActor, GetOwner(), DamageInfo.Amount, GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation());
		}
	}
}

TArray<AActor*> UAttackSystem::DamageAllNonTeamMembers(FDamageInfo DamageInfo, TArray<FHitResult> Hits)
{
	TArray<AActor*> ActorsDamagedSoFar;
	AActor* Owner = GetOwner();
	const IGenericTeamAgentInterface* OwnerTeam = Cast<IGenericTeamAgentInterface>(Owner);
	if (!OwnerTeam) return TArray<AActor*>();
	for (FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) continue;
		const IGenericTeamAgentInterface* HitTeam = Cast<IGenericTeamAgentInterface>(HitActor);
		if (!HitTeam) continue;
		if ((HitTeam->GetGenericTeamId() != OwnerTeam->GetGenericTeamId()) && !ActorsDamagedSoFar.Contains(HitActor))
		{
			if (HitActor->Implements<UDamageableInterface>())
			{
				IDamageableInterface::Execute_TakeDamage(HitActor, DamageInfo, GetOwner());
				ActorsDamagedSoFar.AddUnique(HitActor);
			}
		}
	}
	return ActorsDamagedSoFar;
}

AActor* UAttackSystem::DamageFirstNonTeamMember(FDamageInfo DamageInfo, TArray<FHitResult> Hits)
{
	AActor* Owner = GetOwner();
	const IGenericTeamAgentInterface* OwnerTeam = Cast<IGenericTeamAgentInterface>(Owner);
	if (!OwnerTeam) return nullptr;

	for (FHitResult Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) continue;

		const IGenericTeamAgentInterface* HitTeam = Cast<IGenericTeamAgentInterface>(HitActor);
		if (!HitTeam) continue;

		if (HitTeam->GetGenericTeamId() != OwnerTeam->GetGenericTeamId())
		{
			if (HitActor->Implements<UDamageableInterface>())
			{
				IDamageableInterface::Execute_TakeDamage(HitActor, DamageInfo, GetOwner());
				return HitActor;
			}
		}
	}
	return nullptr;
}

void UAttackSystem::GroundSmash(FAttackInfo AttackInfo, float Radius)
{
	//IEnemyInterface::Execute_Attack(GetOwner(), AttackInfo.AttackTarget);

	CachedAttackInfo = AttackInfo;
	CachedRadius = Radius;

	if (AttackInfo.Montage)
	{
		if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
		{
			if (UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance())
			{
				AnimInstance->Montage_Play(AttackInfo.Montage, 1.0f);

				AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &UAttackSystem::OnMontageNotifyBegin);


				FOnMontageEnded EndDelegate;
				EndDelegate.BindUObject(this, &UAttackSystem::OnAttackMontageEnd);
				AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackInfo.Montage);
			}
		}
	}

	IDamageableInterface::Execute_SetIsInterruptable(GetOwner(), false);
}

void UAttackSystem::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{

	if (NotifyName == FName("Smash"))
	{
		FDamageInfo DamageInfo;
		DamageInfo.Amount = 25.f;
		DamageInfo.DamageType = EDamageType::Explosion;
		AOEDamage(CachedAttackInfo.AttackTarget, CachedRadius, DamageInfo);
	}
}

void UAttackSystem::OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	IEnemyInterface::Execute_AttackEnd(GetOwner(), CachedAttackInfo.AttackTarget);

	IDamageableInterface::Execute_SetIsInterruptable(GetOwner(), true);
}

//----------------------------------------------------------------------
// Callbacks
//----------------------------------------------------------------------

void UAttackSystem::OnProjectileHit(AActor* OtherActor, FHitResult Hit)
{
	if (!IsValid(OtherActor))
		return;
	if (!OtherActor->Implements<UDamageableInterface>())
		return;

	IDamageableInterface::Execute_TakeDamage(OtherActor, DamageInfoRef, GetOwner());

	UAISense_Damage::ReportDamageEvent(GetWorld(), OtherActor, GetOwner(), DamageInfoRef.Amount, GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation());

	OnAttackEnd.Broadcast();
}

void UAttackSystem::AOEDamage(AActor* AttackTarget, float Radius, FDamageInfo DamageInfo)
{
	CachedAttackTarget = AttackTarget;
	CachedDamageInfo = DamageInfo;

	const FTransform SpawnTransform(GetOwner()->GetActorRotation(), GetOwner()->GetActorLocation());

	AOE = GetWorld()->SpawnActorDeferred<AAOE_Base>(ActorToSpawn, SpawnTransform, Cast<APawn>(GetOwner()), Cast<APawn>(GetOwner()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (AOE)
	{
		AOE->Radius = Radius;
		AOE->DrawDebugSphere = false;
		AOE->IgnoreInstigator = true;
		AOE->OnAOEOverlapActor.AddDynamic(this, &UAttackSystem::AOEDamageActor);
		AOE->FinishSpawning(SpawnTransform);
		AOE->Trigger();
	}
}

void UAttackSystem::AOEDamageActor(AActor* Actor)
{
	if (Actor == CachedAttackTarget)
	{
		IDamageableInterface::Execute_TakeDamage(Actor, CachedDamageInfo, GetOwner());
	}
}
