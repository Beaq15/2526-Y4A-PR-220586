// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#include "AIC_Enemy_Base.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DamageSystem = CreateDefaultSubobject<UDamageSystem>(TEXT("DamageSystem"));

	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBarComponent->SetupAttachment(GetMesh());
	HealthBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarComponent->SetDrawSize(FVector2D(200.f, 20.f));

	KnowledgeComponent = CreateDefaultSubobject<UAIKnowledgeComponent>(TEXT("KnowledgeComponent"));
	PerceptionToFactComponent = CreateDefaultSubobject<UAIPerceptionToFactComponent>(TEXT("PerceptionToFactComponent"));
}

void AEnemyBase::OnHitReactionMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	if (!DamageSystem->isDead)
		AICEnemyBase->SetStateAsAttacking(CachedDamageCauser, true);
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	AICEnemyBase = Cast<AAIC_Enemy_Base>(GetController());

	UWidgetHealthBar* HealthBarWidget = CreateWidget<UWidgetHealthBar>(GetWorld(), HealthBarWidgetClass);
	if (HealthBarWidget)
	{
		HealthBarWidget->DamageableActor = TScriptInterface<IDamageableInterface>(this);
		HealthBarComponent->SetWidget(HealthBarWidget);
	}

	DamageSystem->OnDeath.AddUniqueDynamic(this, &AEnemyBase::OnDeath_Event);
	DamageSystem->OnDamageResponse.AddUniqueDynamic(this, &AEnemyBase::OnHitResponse_Event);
	KnowledgeComponent->OnFactAdded.AddUniqueDynamic(this, &AEnemyBase::OnFactReceived);
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

APatrolRoute* AEnemyBase::GetPatrolRoute_Implementation()
{
	return PatrolRoute;
}

void AEnemyBase::OnFactReceived(FSharedFact Fact)
{
	if (Fact.FactType == FGameplayTag::RequestGameplayTag("Fact.UnderAttack") ||
		Fact.FactType == FGameplayTag::RequestGameplayTag("Fact.EmemySighted"))
	{
		if (Fact.Subject.Actor.IsValid() && AICEnemyBase)
			AICEnemyBase->SetStateAsAttacking(Fact.Subject.Actor.Get(), true);
	}
}

void AEnemyBase::OnDeath_Event()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	AICEnemyBase->SetStateAsDead();

	AICEnemyBase->BrainComponent->StopLogic("Dead");
}

void AEnemyBase::OnHitResponse_Event(EDamageResponse DamageResponse, AActor* DamageCauser)
{
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

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

void AEnemyBase::GetIdealRange_Implementation(float& AttackRadius, float& DefendRadius)
{
	AttackRadius = 100.f;
	DefendRadius = 350.f;
}

float AEnemyBase::Heal_Implementation(float Amount)
{
	return DamageSystem->Heal(Amount);
}

bool AEnemyBase::TakeDamage_Implementation(const FDamageInfo& DamageInfo, AActor* DamageCauser)
{
	return DamageSystem->TakeDamage(DamageInfo, DamageCauser);
}

void AEnemyBase::Attack()
{

}

