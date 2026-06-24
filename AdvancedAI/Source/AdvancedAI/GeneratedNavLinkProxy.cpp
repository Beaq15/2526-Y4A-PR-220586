// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneratedNavLinkProxy.h"
#include "EnemyInterface.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"


UGeneratedNavLinkProxy::UGeneratedNavLinkProxy(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    OnSmartLinkReached.AddDynamic(this, &UGeneratedNavLinkProxy::OnSmartLinkReachedCustom);
}

bool UGeneratedNavLinkProxy::OnLinkMoveStarted(UObject* PathComp, const FVector& DestPoint)
{
    return Super::OnLinkMoveStarted(PathComp, DestPoint);
}

UWorld* UGeneratedNavLinkProxy::GetWorld() const
{
    if (UObject* Outer = GetOuter())
    {
        return Outer->GetWorld();
    }
    return nullptr;
}

void UGeneratedNavLinkProxy::OnSmartLinkReachedCustom(AActor* MovingActor, const FVector DestinationPoint)
{
    if (MovingActor && MovingActor->Implements<UEnemyInterface>())
    {
        IEnemyInterface::Execute_JumpToDestination(MovingActor, DestinationPoint);
    }
}