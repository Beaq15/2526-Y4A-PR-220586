// Fill out your copyright notice in the Description page of Project Settings.


#include "EQS_Context_AttackTarget.h"
#include "AIC_Enemy_Base.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void UEQS_Context_AttackTarget::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	UObject* QueryOwner = QueryInstance.Owner.Get();
	AActor* QueryActor = Cast<AActor>(QueryOwner);
	if (!QueryActor) return;

	AAIC_Enemy_Base* AIC = Cast<AAIC_Enemy_Base>(QueryActor->GetInstigatorController());

	if (!AIC) return;

	UEnvQueryItemType_Actor::SetContextHelper(ContextData, AIC->AttackTargetActor);
	
	/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("TESTING PURPOSES ONLY"));
	AActor* PlayerStart = UGameplayStatics::GetActorOfClass(QueryInstance.World, APlayerStart::StaticClass());

	UEnvQueryItemType_Actor::SetContextHelper(ContextData, PlayerStart);*/
}
