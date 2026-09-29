// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMelee.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"

//----------------------------------------------------------------------
// Lifecycle
//----------------------------------------------------------------------

void AEnemyMelee::BeginPlay()
{
	Super::BeginPlay();

	DamageSystem->OnBlocked.AddDynamic(this, &AEnemyMelee::OnBlocked);
}

AEnemyMelee::AEnemyMelee()
{

}

//----------------------------------------------------------------------
// Combat API
//----------------------------------------------------------------------

void AEnemyMelee::GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius)
{
	AttackRadius = 200.f;
	DefendRadius = 400.f;
}

void AEnemyMelee::Attack_Implementation(AActor* AttackTarget)
{
	Super::Attack_Implementation(AttackTarget);
}

void AEnemyMelee::ShortRangeAttack(AActor* AttackTarget)
{
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 10.f;
	DamageInfo.DamageType = EDamageType::Melee;

	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.DamageInfo = DamageInfo;
	AttackInfo.Montage = AttackMontage;
	AttackSystem->ShortRange(AttackInfo, 20.f, 200.f);
}

void AEnemyMelee::LongRangeAttack(AActor* AttackTarget)
{
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 10.f;
	DamageInfo.DamageType = EDamageType::Melee;

	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.DamageInfo = DamageInfo;
	AttackInfo.Montage = SwordJumpAttackMontage;
	AttackSystem->LongRange(AttackInfo, 20.f, 200.f);
}

void AEnemyMelee::SpinningAttack(AActor* AttackTarget)
{
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 10.f;
	DamageInfo.DamageType = EDamageType::Melee;

	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.DamageInfo = DamageInfo;
	AttackInfo.Montage = SpinningAttackMontage;
	AttackSystem->Spinning(AttackInfo, 300.f);
}

void AEnemyMelee::StartBlock()
{
	GetWorldTimerManager().ClearTimer(HoldBlockTimer);
	HoldBlockTimer.Invalidate();

	GetCharacterMovement()->StopMovementImmediately();
	DamageSystem->isBlocking = true;
	BlockingState = EBlockingState::Blocking;

	GetWorldTimerManager().SetTimer(HoldBlockTimer, this, &AEnemyMelee::EndBlock, 2.0f, false);
}

void AEnemyMelee::TryToBlock()
{
	if (FMath::FRand() <= BlockChance)
		StartBlock();
}

void AEnemyMelee::EndBlock()
{
	DamageSystem->isBlocking = false;
	BlockingState = EBlockingState::None;

	OnBlockEnd.Broadcast();
}

//----------------------------------------------------------------------
// IEnemyInterface
//----------------------------------------------------------------------


void AEnemyMelee::EquipWeapon_Implementation()
{
	if (EquipSwordMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(EquipSwordMontage, 1.0f);
			AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &AEnemyMelee::OnMontageNotifyBegin);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemyMelee::OnEquipSwordMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, EquipSwordMontage);
		}
	}
}

void AEnemyMelee::UnequipWeapon_Implementation()
{
	if (DropSwordMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(DropSwordMontage, 1.0f);
			AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &AEnemyMelee::OnMontageNotifyBegin);


			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemyMelee::OnDropSwordMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, DropSwordMontage);
		}
	}
}

bool AEnemyMelee::TakeDamage_Implementation(const FDamageInfo& DamageInfo, AActor* DamageCauser)
{
	if (DamageInfo.bCanBeBlocked)
		TryToBlock();

	return DamageSystem->TakeDamage(DamageInfo, DamageCauser);
}

//----------------------------------------------------------------------
// Animation Callbacks
//----------------------------------------------------------------------

void AEnemyMelee::OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	IEnemyInterface::Execute_AttackEnd(this, CachedAttackTarget);

	DamageSystem->isInterruptible = true;
}

void AEnemyMelee::OnEquipSwordMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	OnEquipWeaponEnd.Broadcast();
}

void AEnemyMelee::OnDropSwordMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	bIsWieldingWeapon = false;
	OnDropWeaponEnd.Broadcast();
}

void AEnemyMelee::OnBlocked(bool bCanBeParried, AActor* DamageCauser)
{
	BlockingState = EBlockingState::BlockedSuccessfully;

	GetWorldTimerManager().ClearTimer(HoldBlockTimer);
	HoldBlockTimer.Invalidate();

	if (SwordBlockHitMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(SwordBlockHitMontage, 1.0f);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemyMelee::OnBlockHitMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, SwordBlockHitMontage);
		}
	}
}

void AEnemyMelee::OnBlockHitMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	EndBlock();
}

void AEnemyMelee::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{

	if (NotifyName == FName("HoldSword"))
	{
		if (!WeaponClass) return;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = this;

		WeaponActor = GetWorld()->SpawnActor<AActor>(WeaponClass, GetActorTransform(), SpawnParams);
		if (!WeaponActor) return;

		WeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true), FName("hand_r_sword_socket"));

		bIsWieldingWeapon = true;

		GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.RemoveDynamic(this, &AEnemyMelee::OnMontageNotifyBegin);
	}

	if (NotifyName == FName("DropSword"))
	{
		if (WeaponActor)
		{
			WeaponActor->Destroy();
			WeaponActor = nullptr;
		}

		bIsWieldingWeapon = false;

		GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.RemoveDynamic(this, &AEnemyMelee::OnMontageNotifyBegin);
	}

	if (NotifyName == FName("AOESlash"))
	{
		const FTransform SpawnTransform(GetActorRotation(), GetActorLocation());

		AOE = GetWorld()->SpawnActorDeferred<AAOE_Base>(ActorToSpawn, SpawnTransform, this, this, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (AOE)
		{
			AOE->Radius = 300.0f;
			AOE->DrawDebugSphere = false;
			AOE->IgnoreInstigator = true;
			AOE->OnAOEOverlapActor.AddDynamic(this, &AEnemyMelee::AOEDamageActor);
			AOE->FinishSpawning(SpawnTransform);
			AOE->Trigger();
		}
	}
}

void AEnemyMelee::AOEDamageActor(AActor* Actor)
{
	if (Actor == CachedAttackTarget)
	{
		FDamageInfo DamageInfo;
		DamageInfo.Amount = 3.f;
		DamageInfo.DamageType = EDamageType::Melee;
		DamageInfo.DamageResponse = EDamageResponse::HitReaction;

		IDamageableInterface::Execute_TakeDamage(Actor, DamageInfo, this);
	}
}

void AEnemyMelee::GroundSmashAttack(AActor* AttackTarget)
{
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 25.f;
	DamageInfo.DamageType = EDamageType::Explosion;

	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.DamageInfo = DamageInfo;
	AttackInfo.Montage = GroundSmashMontage;

	AttackSystem->GroundSmash(AttackInfo, 300.f);
}