// Fill out your copyright notice in the Description page of Project Settings.


#include "AOE_Heal.h"
#include "Kismet/GameplayStatics.h"

AAOE_Heal::AAOE_Heal()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AAOE_Heal::BeginPlay()
{
	Super::BeginPlay();
}

void AAOE_Heal::Trigger()
{	
	Super::Trigger();

	UGameplayStatics::SpawnEmitterAttached(
		P_HealAura, RootComponent, FName("None"), FVector::ZeroVector, FRotator::ZeroRotator, FVector::OneVector);
}
