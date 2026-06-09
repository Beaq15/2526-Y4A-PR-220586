// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "NavLink_EnemyAI.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API ANavLink_EnemyAI : public ANavLinkProxy
{
	GENERATED_BODY()
	
public:
	virtual void PostInitializeComponents() override;

	void OnSmartLinkReachedCustom(UNavLinkCustomComponent* LinkComp, UObject* PathingAgent, const FVector& DestPoint);
};
