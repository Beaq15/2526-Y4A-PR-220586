// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyRanged.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDAI_API AEnemyRanged : public AEnemyBase
{
	GENERATED_BODY()

private:

	virtual void EquipWeapon_Implementation() override;

	virtual void BeginPlay() override;
	
};
