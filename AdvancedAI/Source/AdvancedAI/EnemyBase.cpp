// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::Attack()
{
	if (AttackMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(AttackMontage, 1.0f);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemyBase::OnAttackMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
		}
	}
}

void AEnemyBase::OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
		OnAttackEnd.Broadcast();
}

void AEnemyBase::OnEquipSwordMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	OnEquipSwordEnd.Broadcast();
}

void AEnemyBase::OnDropSwordMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	OnDropSwordEnd.Broadcast();
}

void AEnemyBase::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{
	if (NotifyName == FName("HoldSword"))
	{
		if (!SwordClass) return;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = this;

		SpawnedSword = GetWorld()->SpawnActor<AActor>(SwordClass, GetActorTransform(), SpawnParams);
		if (!SpawnedSword) return;

		SpawnedSword->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true), FName("ik_hand_r_sword_socket"));
		
		bIsWieldingSword = true;
		
		GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.RemoveDynamic(this, &AEnemyBase::OnMontageNotifyBegin);
	}

	if (NotifyName == FName("DropSword"))
	{
		SpawnedSword->Destroy();
		bIsWieldingSword = false;

		GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.RemoveDynamic(this, &AEnemyBase::OnMontageNotifyBegin);
	}
}

APatrolRoute* AEnemyBase::GetPatrolRoute_Implementation()
{
	return PatrolRoute;
}

float AEnemyBase::SetMovementSpeed_Implementation(EMovementSpeed Speed)
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement) return 0.f;

	switch (Speed)
	{
	case EMovementSpeed::Idle:
		Movement->MaxWalkSpeed = 0.f;
		break;
	case EMovementSpeed::Walking:
		Movement->MaxWalkSpeed = 100.f;
		break;
	case EMovementSpeed::Jogging:
		Movement->MaxWalkSpeed = 300.f;
		break;
	case EMovementSpeed::Sprinting:
		Movement->MaxWalkSpeed = 500.f;
		break;
	}

	return Movement->MaxWalkSpeed;
}

void AEnemyBase::GetIdealRange_Implementation(float AttackRadius, float DefendRadius)
{
	AttackRadius = 100.f;
	DefendRadius = 350.f;
}

void AEnemyBase::WieldSword()
{
	if (EquipSwordMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(EquipSwordMontage, 1.0f);
			AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AEnemyBase::OnMontageNotifyBegin);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemyBase::OnEquipSwordMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, EquipSwordMontage);
		}
	}
}

void AEnemyBase::DropSword()
{
	if (DropSwordMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(DropSwordMontage, 1.0f);

			AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AEnemyBase::OnMontageNotifyBegin);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemyBase::OnDropSwordMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, DropSwordMontage);
		}
	}
}

