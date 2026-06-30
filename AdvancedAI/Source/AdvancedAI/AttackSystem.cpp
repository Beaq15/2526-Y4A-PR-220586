// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackSystem.h"
#include "DamageableInterface.h"
#include "Perception/AISense_Damage.h"
#include "Kismet/KismetSystemLibrary.h"


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

