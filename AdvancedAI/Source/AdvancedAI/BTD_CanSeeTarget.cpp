// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_CanSeeTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_CanSeeTarget::UBTD_CanSeeTarget()
{
	NodeName = "Can See Target";
	bNotifyTick = true;
}

bool UBTD_CanSeeTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();

	AActor* Actor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));

	if (!Pawn || !Actor) return false;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Pawn);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Pawn->GetPawnViewLocation(), Actor->GetActorLocation() + FVector(0, 0, 60.f), ECollisionChannel::ECC_Visibility, Params);

	return bHit && HitResult.GetActor() == Actor;
}

void UBTD_CanSeeTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if(!CalculateRawConditionValue(OwnerComp, NodeMemory))
		OwnerComp.RequestExecution(this);
}
