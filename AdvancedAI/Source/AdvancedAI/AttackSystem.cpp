// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackSystem.h"
#include "DamageableInterface.h"
#include "Perception/AISense_Damage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EnemyInterface.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnemyBase.h"
#include "AIC_Enemy_Base.h"
#include "DamageableInterface.h" 
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

void UAttackSystem::MagicSpell(FTransform SpawnTransform, AActor* TargetActor, FDamageInfo DamageInfo, float Speed)
{
	if (!ProjectileClass) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());

	AProjectileBase* Projectile = GetWorld()->SpawnActorDeferred<AProjectileBase>(
		ProjectileClass,
		SpawnTransform,
		GetOwner(),
		Cast<APawn>(GetOwner()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!Projectile) return;

	Projectile->Speed = Speed;
	Projectile->Target = TargetActor;

	DamageInfoRef = DamageInfo;

	Projectile->OnProjectileImpact.AddDynamic(this, &UAttackSystem::OnProjectileHit);

	Projectile->FinishSpawning(SpawnTransform);
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

void UAttackSystem::AroundAttack(FAttackInfo AttackInfo, float Radius)
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

void UAttackSystem::RangeAttack(FAttackInfo AttackInfo, float Radius, float Length)
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

void UAttackSystem::BasicMageSpell(FAttackInfo AttackInfo)
{
	CachedAttackInfo = AttackInfo;

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
		AOEDamage(CachedRadius, CachedAttackInfo.DamageInfo);
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
		AActor* Target = CachedAttackInfo.AttackTarget;
		ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
		if (!Target || !OwnerCharacter) return;

		const FVector PredictedLocation = CalculateFutureActorLocation(CachedAttackInfo.AttackTarget, 1.0f);
		const FVector EndPos(PredictedLocation.X, PredictedLocation.Y, PredictedLocation.Z);

		const float Distance = Target->GetDistanceTo(OwnerCharacter);
		const float Alpha = FMath::Clamp(FMath::GetRangePct(400.f, 800.f, Distance), 0.f, 1.f);
		const float ArcParam = FMath::Lerp(0.5f, 0.94f, Alpha);

		FVector LaunchVelocity;
		UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, LaunchVelocity, GetOwner()->GetActorLocation(), EndPos, 0.f, ArcParam);

		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), EndPos, 100.f, 12, FLinearColor::White, 2.0f);

		OwnerCharacter->LaunchCharacter(LaunchVelocity, true, true);

		OwnerCharacter->LandedDelegate.AddDynamic(this, &UAttackSystem::OnLand);
	}

	if (NotifyName == FName("AOESlash"))
	{
		AOEDamage(CachedRadius, CachedAttackInfo.DamageInfo);
	}

	if (NotifyName == "Fire")
	{
		ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
		FVector SocketLocation = OwnerCharacter->GetMesh()->GetSocketLocation(FName("RightHand"));
		FVector SpawnLocation = SocketLocation + OwnerCharacter->GetActorForwardVector() * 50.f;

		FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, CachedAttackInfo.AttackTarget->GetActorLocation());
		FTransform SpawnTransform(SpawnRotation, SpawnLocation, FVector::OneVector);

		MagicSpell(SpawnTransform, CachedAttackInfo.AttackTarget, CachedAttackInfo.DamageInfo, 1000.0f);
	}

	if (NotifyName == "Throw")
	{
		
		if (!ProjectileClass) return;

		AEnemyBase* Enemy = Cast<AEnemyBase>(GetOwner());
		if (!Enemy) return;
		
		if (Enemy->WeaponActor)
		{
			Enemy->WeaponActor->SetActorHiddenInGame(true);
		}

		USkeletalMeshComponent* EnemyMesh = Enemy->GetMesh();
		if (!EnemyMesh) return;

		const FVector SpawnLocation = EnemyMesh->GetSocketLocation(FName("hand_r"));

		AActor* TargetActor = nullptr;
		if (AAIC_Enemy_Base* AIC = Cast<AAIC_Enemy_Base>(Enemy->GetController()))
		{
			TargetActor = AIC->AttackTargetActor;
		}

		FRotator SpawnRotation = Enemy->GetActorRotation();
		if (TargetActor)
		{
			SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, TargetActor->GetActorLocation());
		}


		const FTransform SpawnTransform(SpawnRotation, SpawnLocation);

		AProjectileBase* Axe = GetWorld()->SpawnActorDeferred<AProjectileBase>(ProjectileClass, SpawnTransform, Enemy, Enemy, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (Axe)
		{
			Axe->Speed = 3500.f;
			Axe->Gravity = 0.f;
			Axe->Target = TargetActor;
			Axe->bIsHoming = true;

			Axe->OnProjectileImpact.AddUniqueDynamic(this, &UAttackSystem::OnProjectileImpact_Event);

			Axe->FinishSpawning(SpawnTransform);
		}
	}
}

void UAttackSystem::OnProjectileImpact_Event(AActor* OtherActor, FHitResult Hit)
{
	if (!OtherActor || OtherActor == GetOwner()) return;

	if (!OtherActor->Implements<UDamageableInterface>()) return;

	FDamageInfo DamageInfo;
	DamageInfo.Amount = 20.f;
	DamageInfo.DamageType = EDamageType::Projectile;
	DamageInfo.DamageResponse = EDamageResponse::HitReaction;
	DamageInfo.bCanBeBlocked = true;

	IDamageableInterface::Execute_TakeDamage(OtherActor, DamageInfo, GetOwner());
}

void UAttackSystem::OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	IEnemyInterface::Execute_AttackEnd(GetOwner(), CachedAttackInfo.AttackTarget);

	IDamageableInterface::Execute_SetIsInterruptable(GetOwner(), true);

	AEnemyBase* Enemy = Cast<AEnemyBase>(GetOwner());
	if (!Enemy) return;

	if (Enemy->WeaponActor)
	{
		Enemy->WeaponActor->SetActorHiddenInGame(false);
	}
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

void UAttackSystem::AOEDamage(float Radius, FDamageInfo DamageInfo)
{
	CachedAttackInfo.DamageInfo = DamageInfo;

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
	if (IDamageableInterface::Execute_GetTeamNumber(Actor) != IDamageableInterface::Execute_GetTeamNumber(GetOwner()))
		IDamageableInterface::Execute_TakeDamage(Actor, CachedAttackInfo.DamageInfo, GetOwner());
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