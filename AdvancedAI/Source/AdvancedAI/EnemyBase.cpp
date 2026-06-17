// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"
#include "AIC_Enemy_Base.h"

//----------------------------------------------------------------------
// Lifecycle
//----------------------------------------------------------------------

AEnemyBase::AEnemyBase()
{

	PrimaryActorTick.bCanEverTick = true;

	DamageSystem = CreateDefaultSubobject<UDamageSystem>(TEXT("DamageSystem"));
	AttackSystem = CreateDefaultSubobject<UAttackSystem>(TEXT("AttackSystem"));

	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBarComponent->SetupAttachment(GetMesh());
	HealthBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarComponent->SetDrawSize(FVector2D(200.f, 20.f));

	KnowledgeComponent = CreateDefaultSubobject<UAIKnowledgeComponent>(TEXT("KnowledgeComponent"));
	PerceptionToFactComponent = CreateDefaultSubobject<UAIPerceptionToFactComponent>(TEXT("PerceptionToFactComponent"));
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	AICEnemyBase = Cast<AAIC_Enemy_Base>(GetController());

	if (HealthBarWidgetClass)
	{
		UWidgetHealthBar* HealthBarWidget = CreateWidget<UWidgetHealthBar>(GetWorld(), HealthBarWidgetClass);
		if (HealthBarWidget)
		{
			HealthBarWidget->DamageableActor = TScriptInterface<IDamageableInterface>(this);
			HealthBarComponent->SetWidget(HealthBarWidget);
		}
	}
	
	DamageSystem->OnDeath.AddUniqueDynamic(this, &AEnemyBase::OnDeath_Event);
	DamageSystem->OnDamageResponse.AddUniqueDynamic(this, &AEnemyBase::OnHitResponse_Event);
	KnowledgeComponent->OnFactAdded.AddUniqueDynamic(this, &AEnemyBase::OnFactReceived);
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//----------------------------------------------------------------------
// Callbacks
//----------------------------------------------------------------------

void AEnemyBase::OnHitReactionMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	if (!DamageSystem->isDead && AICEnemyBase)
	{
		AICEnemyBase->SetStateAsAttacking(CachedDamageCauser, true);
	}
}

void AEnemyBase::OnFactReceived(FSharedFact Fact)
{
 	if (Fact.FactType == FGameplayTag::RequestGameplayTag("Fact.UnderAttack") ||
		Fact.FactType == FGameplayTag::RequestGameplayTag("Fact.EnemySighted"))
	{
		if (Fact.Subject.Actor.IsValid() && AICEnemyBase)
			AICEnemyBase->SetStateAsAttacking(Fact.Subject.Actor.Get(), true);
	}
}

void AEnemyBase::OnDeath_Event()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	if (!AICEnemyBase) return;

	AICEnemyBase->SetStateAsDead();

	AICEnemyBase->BrainComponent->StopLogic("Dead");
}

void AEnemyBase::OnHitResponse_Event(EDamageResponse DamageResponse, AActor* DamageCauser)
{
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	if (AICEnemyBase)
		AICEnemyBase->SetStateAsFrozen();

	CachedDamageCauser = DamageCauser;

	if (HitReactionMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(HitReactionMontage, 1.0f);
				
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemyBase::OnHitReactionMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, HitReactionMontage);

		}
	}
}

//----------------------------------------------------------------------
// IEnemyInterface
//----------------------------------------------------------------------

APatrolRoute* AEnemyBase::GetPatrolRoute_Implementation()
{
	return PatrolRouteActor;
}

void AEnemyBase::Attack_Implementation(AActor* AttackTarget)
{
	bAttacking = true;
}

float AEnemyBase::SetMovementSpeed_Implementation(EMovementSpeed Speed)
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement) return 0.f;

	switch (Speed)
	{
	case EMovementSpeed::Idle:      Movement->MaxWalkSpeed = 0.f;   break;
	case EMovementSpeed::Walking:   Movement->MaxWalkSpeed = 100.f; break;
	case EMovementSpeed::Jogging:   Movement->MaxWalkSpeed = 300.f; break;
	case EMovementSpeed::Sprinting: Movement->MaxWalkSpeed = 500.f; break;
	}

	return Movement->MaxWalkSpeed;
}

void AEnemyBase::GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius)
{
	AttackRadius = 100.f;
	DefendRadius = 350.f;
}

void AEnemyBase::JumpToDestination_Implementation(FVector Destination)
{
	FVector LaunchVelocity;
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, LaunchVelocity, GetActorLocation(), { Destination.X, Destination.Y, Destination.Z + 250.f });
	LaunchCharacter(LaunchVelocity, true, true);
		
}

void AEnemyBase::StoreAttackTokens_Implementation(AActor* AttackTarget, int32 Amount)
{
	if (ReservedAttackTokens.Find(AttackTarget))
	{
		int32* Current = ReservedAttackTokens.Find(AttackTarget);
		ReservedAttackTokens.Add(AttackTarget, Amount + *Current);
	}
	else
		ReservedAttackTokens.Add(AttackTarget, Amount);
}

void AEnemyBase::AttackEnd_Implementation(AActor* AttackTarget)
{
	if (AttackTarget->Implements<UDamageableInterface>() && AttackTarget)
	{
		IDamageableInterface::Execute_ReturnAttackToken(AttackTarget, TokensUsedInCurrentAttack);
		IEnemyInterface::Execute_StoreAttackTokens(this, AttackTarget, TokensUsedInCurrentAttack * -1);

		bAttacking = false;
		OnAttackEnd.Broadcast();
	}
}

bool AEnemyBase::DidAttackStart_Implementation(AActor* AttackTarget, int32 Amount)
{
	if (AttackTarget->Implements<UDamageableInterface>() && AttackTarget)
	{
		if (IDamageableInterface::Execute_ReserveAttackToken(AttackTarget, Amount))
		{
			IEnemyInterface::Execute_StoreAttackTokens(this, AttackTarget, Amount);
			TokensUsedInCurrentAttack = Amount;
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------
// IDamageableInterface
//----------------------------------------------------------------------

float AEnemyBase::Heal_Implementation(float Amount)
{
	return DamageSystem->Heal(Amount);
}

bool AEnemyBase::TakeDamage_Implementation(const FDamageInfo& DamageInfo, AActor* DamageCauser)
{
	return DamageSystem->TakeDamage(DamageInfo, DamageCauser);
}

bool AEnemyBase::ReserveAttackToken_Implementation(int32 Amount)
{
	return DamageSystem->ReserveAttackToken(Amount);
}

void AEnemyBase::ReturnAttackToken_Implementation(int32 Amount)
{
	DamageSystem->ReturnAttackToken(Amount);
}
