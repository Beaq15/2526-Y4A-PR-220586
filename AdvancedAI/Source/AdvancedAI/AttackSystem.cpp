// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackSystem.h"
#include "DamageableInterface.h"
#include "Perception/AISense_Damage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EnemyInterface.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

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

void UAttackSystem::ShortRange(FAttackInfo AttackInfo, float Radius, float Length)
{
	CachedAttackInfo = AttackInfo;
	CachedRadius = Radius;
	CachedLength = Length;

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

void UAttackSystem::LongRange(FAttackInfo AttackInfo, float Radius, float Length)
{
	CachedAttackInfo = AttackInfo;
	CachedRadius = Radius;
	CachedLength = Length;

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

void UAttackSystem::Spinning(FAttackInfo AttackInfo, float Radius)
{
	CachedAttackInfo = AttackInfo;
	CachedRadius = Radius;

	if (AttackInfo.Montage)
	{
		IDamageableInterface::Execute_SetIsInterruptable(GetOwner(), false);

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
}

void UAttackSystem::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{

	if (NotifyName == FName("Smash"))
	{
		AOEDamage(CachedAttackInfo.AttackTarget, CachedRadius, CachedDamageInfo);
	}

	if (NotifyName == FName("Slash"))
	{
		FVector Start = GetOwner()->GetActorLocation();
		FVector End = GetOwner()->GetActorForwardVector() * CachedLength + GetOwner()->GetActorLocation();

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(GetOwner());

		TArray <FHitResult> OutHits;

		bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Start, End, CachedRadius, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHits, true);

		if (bHit)
			DamageAllNonTeamMembers(CachedAttackInfo.DamageInfo, OutHits);
	}

	if (NotifyName == FName("Jump"))
	{
		const FVector PredictedLocation = CalculateFutureActorLocation(CachedAttackInfo.AttackTarget, 1.0f);
		const FVector EndPos(PredictedLocation.X, PredictedLocation.Y, PredictedLocation.Z);

		FVector LaunchVelocity;
		UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, LaunchVelocity, GetOwner()->GetActorLocation(), EndPos);

		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), EndPos, 100.f, 12, FLinearColor::White, 2.0f);

		ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
		OwnerCharacter->LaunchCharacter(LaunchVelocity, true, true);

		OwnerCharacter->LandedDelegate.AddDynamic(this, &UAttackSystem::OnLand);
	}

	if (NotifyName == FName("AOESlash"))
	{
		AOEDamage(CachedAttackInfo.AttackTarget, CachedRadius, CachedDamageInfo);
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

FVector UAttackSystem::CalculateFutureActorLocation(AActor* Actor, float Time)
{
	// l = v + t + currentLocation

	if (!Actor)
	{
		return FVector::ZeroVector;
	}

	const FVector Velocity = Actor->GetVelocity();
	const FVector HorizontalVelocity(Velocity.X, Velocity.Y, 0.0f);

	return Actor->GetActorLocation() + (HorizontalVelocity * Time);
}

void UAttackSystem::OnLand(const FHitResult& Hit)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	OwnerCharacter->LandedDelegate.RemoveDynamic(this, &UAttackSystem::OnLand);

	OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
}