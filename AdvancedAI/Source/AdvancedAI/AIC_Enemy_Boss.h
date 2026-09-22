// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIC_Enemy_Base.h"
#include "AIC_Enemy_Boss.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API AAIC_Enemy_Boss : public AAIC_Enemy_Base
{
	GENERATED_BODY()
	
public:
	virtual void SetStateAsAttacking(AActor* AttackTarget, bool UseLastKnownAttackTarget) override;

	bool bHasDoneOnce = false;
};
