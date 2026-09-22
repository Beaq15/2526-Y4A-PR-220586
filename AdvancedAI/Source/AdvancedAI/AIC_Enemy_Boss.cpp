// Fill out your copyright notice in the Description page of Project Settings.


#include "AIC_Enemy_Boss.h"
#include "Kismet/GameplayStatics.h"
#include "AdvancedAICharacter.h"
#include "EnemyBoss.h"

void AAIC_Enemy_Boss::SetStateAsAttacking(AActor* AttackTarget, bool UseLastKnownAttackTarget)
{
	Super::SetStateAsAttacking(AttackTarget, UseLastKnownAttackTarget);
	if (!bHasDoneOnce)
	{
		bHasDoneOnce = true;
		AAdvancedAICharacter* Player = Cast<AAdvancedAICharacter>(AttackTarget);

		if (Player)
		{
			APawn* ControlledPawn = GetPawn();

			AEnemyBoss* Enemy = Cast < AEnemyBoss >(ControlledPawn);

			if (Enemy)
			{
				Player->PlayerHUDWidget->VerticalBox->AddChildToVerticalBox(Enemy->HealthBarWidget);
			}

			else
			{
				bHasDoneOnce = false;
			}
		}

		else
		{
			bHasDoneOnce = false;
		}
	}
}
