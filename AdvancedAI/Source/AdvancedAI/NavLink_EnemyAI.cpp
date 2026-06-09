// Fill out your copyright notice in the Description page of Project Settings.


#include "NavLink_EnemyAI.h"
#include "NavLinkCustomComponent.h"
#include "EnemyInterface.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"


void ANavLink_EnemyAI::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (UNavLinkCustomComponent* LinkComp = GetSmartLinkComp())
	{
		LinkComp->SetMoveReachedLink(this, &ANavLink_EnemyAI::OnSmartLinkReachedCustom);
	}
}

void ANavLink_EnemyAI::OnSmartLinkReachedCustom(UNavLinkCustomComponent* LinkComp, UObject* PathingAgent, const FVector& DestPoint)
{
	UE_LOG(LogTemp, Warning, TEXT("Reached Link"));


    UPathFollowingComponent* PathComp = Cast<UPathFollowingComponent>(PathingAgent);
    if (PathComp)
    {
        //AActor* Actor = Cast<AActor>(PathComp->GetOwner()->GetOwner());
        // PathFollowingComp -> AIController -> Pawn
        AAIController* AIC = Cast<AAIController>(PathComp->GetOwner());
        if (AIC)
        {
            AActor* Agent = AIC->GetPawn();
            if (Agent && Agent->Implements<UEnemyInterface>())
            {
                IEnemyInterface::Execute_JumpToDestination(Agent, DestPoint);
            }
        }
    }
}
