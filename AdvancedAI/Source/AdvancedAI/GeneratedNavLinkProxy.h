// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/GeneratedNavLinksProxy.h"
#include "GeneratedNavLinkProxy.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API UGeneratedNavLinkProxy : public UGeneratedNavLinksProxy
{
	GENERATED_BODY()

public:
    UGeneratedNavLinkProxy(const FObjectInitializer& ObjectInitializer);

    virtual bool OnLinkMoveStarted(UObject* PathComp, const FVector& DestPoint) override;

    virtual UWorld* GetWorld() const override;

private:
    UFUNCTION()
    void OnSmartLinkReachedCustom(AActor* MovingActor, const FVector DestinationPoint);
};
