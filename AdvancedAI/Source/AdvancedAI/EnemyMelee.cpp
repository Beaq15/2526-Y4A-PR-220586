// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMelee.h"
#include "Kismet/KismetSystemLibrary.h"


void AEnemyMelee::OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	OnAttackEnd.Broadcast();
}

void AEnemyMelee::OnEquipSwordMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	OnEquipWeaponEnd.Broadcast();
}

void AEnemyMelee::OnDropSwordMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	OnDropWeaponEnd.Broadcast();
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

		WeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true), FName("ik_hand_r_sword_socket"));

		bIsWieldingWeapon = true;

		GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.RemoveDynamic(this, &AEnemyMelee::OnMontageNotifyBegin);
	}

	if (NotifyName == FName("DropSword"))
	{
		WeaponActor->Destroy();
		bIsWieldingWeapon = false;

		GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.RemoveDynamic(this, &AEnemyMelee::OnMontageNotifyBegin);
	}

	if (NotifyName == FName("Slash"))
	{
		FVector Start = GetActorLocation();
		FVector End = GetActorForwardVector() * 200.f + GetActorLocation();

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);

		FHitResult HitResult;

		bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), Start, End, 20.f, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true);

		if (bHit)
		{
			AActor* HitActor = HitResult.GetActor();

			FDamageInfo DamageInfo;
			DamageInfo.Amount = 25.f;
			DamageInfo.DamageType = EDamageType::Melee;
			Execute_TakeDamage(HitActor, DamageInfo, this);
		}
	}
}

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

void AEnemyMelee::Attack()
{
	if (AttackMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(AttackMontage, 1.0f);

			AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &AEnemyMelee::OnMontageNotifyBegin);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemyMelee::OnAttackMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
		}
	}
}
