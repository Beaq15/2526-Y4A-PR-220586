// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTransition.h"
#include "Kismet/GameplayStatics.h"
#include "AdvancedAICharacter.h"

// Sets default values
ALevelTransition::ALevelTransition()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	SetRootComponent(Trigger);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ALevelTransition::OnOverlap);
}

// Called when the game starts or when spawned
void ALevelTransition::BeginPlay()
{
	Super::BeginPlay();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ALevelTransition::OnOverlap);
}

void ALevelTransition::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AAdvancedAICharacter>(OtherActor))
	{
		UGameplayStatics::OpenLevel(this, LevelName);
	}
}

