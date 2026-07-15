// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMage.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIC_Enemy_Base.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystemComponent.h"

//----------------------------------------------------------------------
// Public — IEnemyInterface
//----------------------------------------------------------------------

float AEnemyMage::SetMovementSpeed_Implementation(EMovementSpeed Speed)
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement) return 0.f;

	switch (Speed)
	{
	case EMovementSpeed::Idle:      Movement->MaxWalkSpeed = 0.f;   break;
	case EMovementSpeed::Walking:   Movement->MaxWalkSpeed = 150.f; break;
	case EMovementSpeed::Jogging:   Movement->MaxWalkSpeed = 250.f; break;
	case EMovementSpeed::Sprinting: Movement->MaxWalkSpeed = 400.f; break;
	}

	return Movement->MaxWalkSpeed;
}

void AEnemyMage::GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius)
{
	AttackRadius = 500.0f;
	DefendRadius = 700.0f;
}

void AEnemyMage::Attack_Implementation(AActor* AttackTarget)
{
	Super::Attack_Implementation(AttackTarget);
	CachedAttackTarget = AttackTarget;

	if (FireMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(FireMontage, 1.0f);
			AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &AEnemyMage::OnMontageNotifyBegin);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemyMage::OnAttackMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, FireMontage);
		}
	}
}

//----------------------------------------------------------------------
// Animation Callbacks
//----------------------------------------------------------------------

void AEnemyMage::OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	IEnemyInterface::Execute_AttackEnd(this, CachedAttackTarget);
}

void AEnemyMage::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{
	if (NotifyName == "Fire")
	{
		FDamageInfo DamageInfo;
		DamageInfo.Amount = 20.f;
		DamageInfo.DamageType = EDamageType::Explosion;
		DamageInfo.DamageResponse = EDamageResponse::HitReaction;
		DamageInfo.bCanBeBlocked = true;

		FVector SocketLocation = GetMesh()->GetSocketLocation(FName("RightHand"));
		FVector SpawnLocation = SocketLocation + GetActorForwardVector() * 50.f;

		FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, CachedAttackTarget->GetActorLocation());
		FTransform SpawnTransform(SpawnRotation, SpawnLocation, FVector::OneVector);

		AttackSystem->MagicSpell(SpawnTransform, CachedAttackTarget, DamageInfo);
	}
}

//----------------------------------------------------------------------
// Private — Teleport
//----------------------------------------------------------------------

void AEnemyMage::Teleport(FVector Location)
{
	GetMesh()->SetVisibility(false, true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	
	TeleportBodyEffect = UGameplayStatics::SpawnEmitterAttached(
		P_GideonBurde, GetMesh(), FName("Spine1"), FVector::ZeroVector, FRotator::ZeroRotator, FVector::OneVector, EAttachLocation::KeepRelativeOffset, false);

	TeleportTrailEffect = UGameplayStatics::SpawnEmitterAttached(
		P_GideonMeteor, GetMesh(), FName("Spine1"), FVector::ZeroVector, FRotator::ZeroRotator, FVector::OneVector, EAttachLocation::KeepRelativeOffset, false);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
		AnimInstance->StopAllMontages(0.25f);

	CachedTeleportLocation = Location;
	GetWorldTimerManager().SetTimer(TeleportMoveTimerHandle, [this]()
		{
			float Distance = FVector::Dist(GetActorLocation(), CachedTeleportLocation);

			if (Distance <= 50.f)
			{
				GetWorldTimerManager().ClearTimer(TeleportMoveTimerHandle);
				TeleportEnd();
				return;
			}

			FVector Direction = (CachedTeleportLocation - GetActorLocation()).GetSafeNormal();
			FVector NewLocation = GetActorLocation() + Direction * 80.f;
			SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);


		}, 0.016f, true);
}

void AEnemyMage::TeleportEnd()
{
	GetMesh()->SetVisibility(true, true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	if (OnTeleportEndCallback)
	{
		OnTeleportEndCallback();
		OnTeleportEndCallback = nullptr;
	}

	if (TeleportBodyEffect.Get()) { TeleportBodyEffect->DestroyComponent(); TeleportBodyEffect = nullptr; }
	if (TeleportTrailEffect.Get()) { TeleportTrailEffect->DestroyComponent(); TeleportTrailEffect = nullptr; }
}

void AEnemyMage::HealOverTime()
{
	if (HealMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			const float MontageLength = AnimInstance->Montage_Play(HealMontage, 1.0f);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemyMage::HealEnded);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, HealMontage);


			FHitResult HitResult;
			TArray<AActor*>ActorsToIgnore;
			ActorsToIgnore.Add(this);

			const FVector TraceStart = GetActorLocation();
			const FVector TraceEnd = TraceStart + FVector(0.0f, 0.0f, -1000.0f);

			const bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStart, TraceEnd, UEngineTypes::ConvertToTraceType(ECC_Visibility),
				false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

			FVector SpawnTransformLocation = TraceStart;

			if (bHit)
			{
				SpawnTransformLocation = HitResult.ImpactPoint;
			}
			else
				StopAnimMontage();

			const FTransform SpawnTransform(GetActorRotation(), SpawnTransformLocation);
			
			HealAOE = GetWorld()->SpawnActorDeferred<AAOE_Heal>(ActorToSpawn, SpawnTransform, this, this, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			if (HealAOE)
			{
				HealAOE->SpawnInterval = 1.0f;
				HealAOE->Duration = MontageLength;
				HealAOE->DrawDebugSphere = false;
				HealAOE->IgnoreInstigator = false;
				HealAOE->TriggerOnBeginPlay = true;

				HealAOE->OnAOEOverlapActor.AddDynamic(this, &AEnemyMage::OnHealTick);
				HealAOE->FinishSpawning(SpawnTransform);

			}
		}
	}
}

void AEnemyMage::HealEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (HealAOE)
	{
		HealAOE->Destroy();
	}

	if (OnHealOverTimeEndCallback)
	{
		OnHealOverTimeEndCallback();
		OnHealOverTimeEndCallback = nullptr;
	}

	OnHealOverTimeEnd.Broadcast();
}

void AEnemyMage::OnHealTick(AActor* Actor)
{
	if (Actor == this)
	{
		Heal_Implementation(DamageSystem->MaxHealth * 0.075);
	}
}

void AEnemyMage::BeginPlay()
{
	Super::BeginPlay();
}

AEnemyMage::AEnemyMage()
{

}

void AEnemyMage::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HealAOE)
	{
		HealAOE->OnAOEOverlapActor.RemoveDynamic(this, &AEnemyMage::OnHealTick);
	}
	GetWorldTimerManager().ClearTimer(TeleportMoveTimerHandle);
	Super::EndPlay(EndPlayReason);
}