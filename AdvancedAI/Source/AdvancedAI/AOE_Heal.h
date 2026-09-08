// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AOE_Base.h"
#include "AOE_Heal.generated.h"

UCLASS()
class ADVANCEDAI_API AAOE_Heal : public AAOE_Base
{
	GENERATED_BODY()
	
public:	
	AAOE_Heal();
	void Trigger();

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<UParticleSystem> P_HealAura;

protected:
	virtual void BeginPlay() override;
};
