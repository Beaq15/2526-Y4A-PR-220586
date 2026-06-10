// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_StopAttackingIfTargetIsDead.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DamageableInterface.h"
#include "AIC_Enemy_Base.h"

UBTS_StopAttackingIfTargetIsDead::UBTS_StopAttackingIfTargetIsDead()
{
	NodeName = "Stop Attacking If Target Is Dead";
	Interval = 1.0f;
	RandomDeviation = 0.f;
}

void UBTS_StopAttackingIfTargetIsDead::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(AttackTargetKey.SelectedKeyName));

	if (Target && Target->Implements<UDamageableInterface>())
	{
		if (IDamageableInterface::Execute_IsDead(Target))
		{
			AAIC_Enemy_Base* AIC = Cast<AAIC_Enemy_Base>(OwnerComp.GetAIOwner());

			if (AIC)
				AIC->SetStateAsPassive();
		}

	}
}
