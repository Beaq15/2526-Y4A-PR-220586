// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBoss.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Blueprint/UserWidget.h"

AEnemyBoss::AEnemyBoss()
{
	DamageSystem->MaxHealth = 700.f;
	DamageSystem->Health = 700.f;

	HealthBarComponent->SetWidget(nullptr);

	if (HealthBarWidgetClass)
	{
		HealthBarWidget = CreateWidget<UWidgetBossHealthBar>(GetWorld(), HealthBarWidgetClass);
		if (HealthBarWidget)
		{
			HealthBarWidget->UpdateHealthPercentage(DamageSystem->MaxHealth, DamageSystem->Health);
		}
	}
}
float AEnemyBoss::SetMovementSpeed_Implementation(EMovementSpeed Speed)
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement) return 0.f;

	switch (Speed)
	{
	case EMovementSpeed::Idle:      Movement->MaxWalkSpeed = 0.f;   break;
	case EMovementSpeed::Walking:   Movement->MaxWalkSpeed = 100.f; break;
	case EMovementSpeed::Jogging:   Movement->MaxWalkSpeed = 300.f; break;
	case EMovementSpeed::Sprinting: Movement->MaxWalkSpeed = 350.f; break;
	}

	return Movement->MaxWalkSpeed;
}

bool AEnemyBoss::TakeDamage_Implementation(const FDamageInfo& DamageInfo, AActor* DamageCauser)
{
	bool ActualDamage = Super::TakeDamage_Implementation(DamageInfo, DamageCauser);

	HealthBarWidget->UpdateHealthPercentage(DamageSystem->MaxHealth, DamageSystem->Health);

	return ActualDamage;
}

void AEnemyBoss::EquipWeapon_Implementation()
{
	if (!WeaponClass) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = this;

	WeaponActor = GetWorld()->SpawnActor<AActor>(WeaponClass, GetActorTransform(), SpawnParams);
	if (!WeaponActor) return;

	WeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true), FName("hand_r_sword_socket"));

	bIsWieldingWeapon = true;

	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			OnEquipWeaponEnd.Broadcast();
		});
}

void AEnemyBoss::UnequipWeapon_Implementation()
{
	if (WeaponActor)
	{
		WeaponActor->Destroy();
		WeaponActor = nullptr;
	}

	bIsWieldingWeapon = false;


	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			OnDropWeaponEnd.Broadcast();
		});
}

void AEnemyBoss::ThrowAxe(AActor* AttackTarget)
{
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 20.0f;
	DamageInfo.bCanBeBlocked = true;

	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = ThrowAxeMontage;
	AttackInfo.DamageInfo = DamageInfo;
	AttackSystem->RangeAttack(AttackInfo, 40.0f, 220.f);
}

void AEnemyBoss::QuickAttack(AActor* AttackTarget)
{
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 20.0f;
	DamageInfo.bCanBeBlocked = true;

	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = QuickAttackMontage;
	AttackInfo.DamageInfo = DamageInfo;
	AttackSystem->RangeAttack(AttackInfo, 40.0f, 220.f);
}

void AEnemyBoss::ShowVictoryScreen()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->RemoveFromParent();
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC || !VictoryWidgetClass) return;

	UUserWidget* Widget = CreateWidget<UUserWidget>(PC, VictoryWidgetClass);
	if (!Widget) return;

	Widget->AddToViewport();

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(Widget->TakeWidget());
	PC->SetInputMode(InputMode);
	PC->SetShowMouseCursor(true);
}

void AEnemyBoss::AttackCombo1(AActor* AttackTarget)
{
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 20.0f;
	DamageInfo.bCanBeBlocked = true;

	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = AxeComboMontage1;
	AttackInfo.DamageInfo = DamageInfo;

	AttackSystem->RangeAttack(AttackInfo, 40.0f, 220.f);
}

void AEnemyBoss::AttackCombo2(AActor* AttackTarget)
{
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 15.0f;
	DamageInfo.bCanBeBlocked = true;

	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = AxeComboMontage2;
	AttackInfo.DamageInfo = DamageInfo;

	AttackSystem->RangeAttack(AttackInfo, 40.0f, 220.f);
}

void AEnemyBoss::JumpAttack(AActor* AttackTarget)
{
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 15.0f;
	DamageInfo.bCanBeBlocked = false;

	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = JumpAttackMontage;
	AttackInfo.DamageInfo = DamageInfo;

	AttackSystem->RangeAttack(AttackInfo,300.0f, 220.f);
}

void AEnemyBoss::GroundSmashAttack(AActor* AttackTarget)
{
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 25.0f;
	DamageInfo.bCanBeBlocked = false;

	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = GroundSmashAttackMontage;
	AttackInfo.DamageInfo = DamageInfo;

	AttackSystem->AroundAttack(AttackInfo, 1100.0f);
}

void AEnemyBoss::SpawnMinions(const FVector& SpawnLocation)
{
	CachedSpawnLocation = SpawnLocation;

	if (SpawnMinionsMontage)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(SpawnMinionsMontage, 1.0f);

			if (SummonSpawnPointEffect)
			{
				ActiveSummonSpawnPointEffect = UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(), SummonSpawnPointEffect, CachedSpawnLocation, FRotator::ZeroRotator, FVector(1.f), true);
			}

			if (SummonBossEffect)
			{
				ActiveSummonBossEffect = UGameplayStatics::SpawnEmitterAttached(
					SummonBossEffect, GetMesh(), FName("root"), FVector::ZeroVector, FRotator::ZeroRotator,
					FVector(1.f), EAttachLocation::KeepRelativeOffset, false);
			}

			AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &AEnemyBoss::OnMontageNotifyBegin);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemyBoss::OnAttackMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, SpawnMinionsMontage);
		}
	}
	DamageSystem->isInterruptible = false;
}

void AEnemyBoss::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{

	if (NotifyName == FName("Spawn"))
	{
		if (bIsDead || !CanSummonMinion()) return;

		if (!MinionClasses.IsEmpty())
		{
			const TSubclassOf<AEnemyBase> MinionClass = MinionClasses[FMath::RandRange(0, MinionClasses.Num() - 1)];

			if (MinionClass)
			{
				const float HalfHeight = MinionClass.GetDefaultObject()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
				const FVector FinalLocation = CachedSpawnLocation + FVector(0.f, 0.f, HalfHeight);
				const FTransform SpawnTransform(GetActorRotation(), FinalLocation);

				AEnemyBase* Minion = GetWorld()->SpawnActorDeferred<AEnemyBase>(
					MinionClass, SpawnTransform, this, GetInstigator(),
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

				if (Minion)
				{
					Minion->AutoPossessAI = EAutoPossessAI::Disabled;
					Minion->FinishSpawning(SpawnTransform);
					Minion->RiseFromGround(FinalLocation, 2.0f);

					ActiveMinions.Add(Minion);
				}

			}
		}
	}
}

bool AEnemyBoss::CanSummonMinion()
{
	for (int32 i = ActiveMinions.Num() - 1; i >= 0; --i)
	{
		if (!ActiveMinions[i].IsValid())
		{
			ActiveMinions.RemoveAt(i);
		}
	}

	if (ActiveMinions.Num() < MaxMinions)
	{
		return true;
	}

	return false;
}

void AEnemyBoss::OnDeath_Event()
{
	bIsDead = true;

	for (const TWeakObjectPtr<AEnemyBase>& Minion : ActiveMinions)
	{
		if (Minion.IsValid())
		{
			Minion->OnDeath_Event();
		}
	}
	ActiveMinions.Empty();

	Super::OnDeath_Event();

	GetWorldTimerManager().SetTimer(VictoryTimerHandle, this, &AEnemyBoss::ShowVictoryScreen, VictoryDelay, false);
}

void AEnemyBoss::OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	DamageSystem->isInterruptible = true;

	if (IsValid(ActiveSummonSpawnPointEffect))
	{
		ActiveSummonSpawnPointEffect->DestroyComponent();
		ActiveSummonSpawnPointEffect = nullptr;
	}

	if (IsValid(ActiveSummonBossEffect))
	{
		ActiveSummonBossEffect->DestroyComponent();
		ActiveSummonBossEffect = nullptr;
	}
}

void AEnemyBoss::Teleport(FVector Location, AActor* AttackTarget)
{
	GetMesh()->SetVisibility(false, true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	TeleportBodyEffect = UGameplayStatics::SpawnEmitterAttached(
		P_GideonBurde, GetMesh(), FName("Spine_01"), FVector::ZeroVector, FRotator::ZeroRotator, FVector(1.25f), EAttachLocation::KeepRelativeOffset, false);

	TeleportTrailEffect = UGameplayStatics::SpawnEmitterAttached(
		P_GideonMeteor, GetMesh(), FName("Spine_01"), FVector::ZeroVector, FRotator::ZeroRotator, FVector(1.25f), EAttachLocation::KeepRelativeOffset, false);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
		AnimInstance->StopAllMontages(0.25f);

	if (AttackTarget)
	{
		const FVector ToBoss = (GetActorLocation() - AttackTarget->GetActorLocation()).GetSafeNormal();
		Location = AttackTarget->GetActorLocation() + ToBoss * 150.f;
	}

	CachedTeleportLocation = Location;

	TeleportStartTime = GetWorld()->GetTimeSeconds();

	GetWorldTimerManager().SetTimer(TeleportMoveTimerHandle, [this]()
		{
			const FVector Current = GetActorLocation();
			FVector ToTarget = CachedTeleportLocation - Current;
			ToTarget.Z = 0.f;
			const float Dist2D = ToTarget.Size();

			const bool bTimedOut = GetWorld()->GetTimeSeconds() - TeleportStartTime > 2.f;
			if (Dist2D <= 50.f || bTimedOut)
			{
				GetWorldTimerManager().ClearTimer(TeleportMoveTimerHandle);
				TeleportEnd();
				return;
			}

			const float Step = FMath::Min(Dist2D, 80.f);
			SetActorLocation(Current + ToTarget.GetSafeNormal() * Step, false, nullptr, ETeleportType::TeleportPhysics);
		}, 0.016f, true);
}

void AEnemyBoss::TeleportEnd()
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

void AEnemyBoss::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(TeleportMoveTimerHandle);
	if (HealAOE)
	{
		HealAOE->OnAOEOverlapActor.RemoveDynamic(this, &AEnemyBoss::OnHealTick);
	}
	Super::EndPlay(EndPlayReason);
}

void AEnemyBoss::HealEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Warning, TEXT("HealEnded - interrupted: %d"), bInterrupted);
	if (HealAOE)
	{
		HealAOE->Destroy();
		HealAOE = nullptr;
	}

	if (OnHealOverTimeEndCallback)
	{
		OnHealOverTimeEndCallback();
		OnHealOverTimeEndCallback = nullptr;
	}

	OnHealOverTimeEnd.Broadcast();
}

void AEnemyBoss::OnHealTick(AActor* Actor)
{
	if (Actor == this)
	{
		Heal_Implementation(DamageSystem->MaxHealth * 0.075);
		HealthBarWidget->UpdateHealthPercentage(DamageSystem->MaxHealth, DamageSystem->Health);
	}

}

void AEnemyBoss::HealOverTime()
{
	if (HealMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			const float MontageLength = AnimInstance->Montage_Play(HealMontage, 1.0f);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemyBoss::HealEnded);
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

				HealAOE->OnAOEOverlapActor.AddDynamic(this, &AEnemyBoss::OnHealTick);
				HealAOE->FinishSpawning(SpawnTransform);

			}
		}
	}
}

