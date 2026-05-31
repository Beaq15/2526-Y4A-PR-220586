// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_CheckFact.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIKnowledgeComponent.h"
#include "SharedFact.h"

UBTS_CheckFact::UBTS_CheckFact()
{
    bNotifyTick = true;      // enable TickNode
    Interval = 0.2f;         // check every 0.2 seconds
    NodeName = TEXT("Service: Check Fact");
}

void UBTS_CheckFact::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    const APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
    if (!Pawn) return;

    if (const UAIKnowledgeComponent* KC = Pawn->FindComponentByClass<UAIKnowledgeComponent>())
    {
        FSharedFact Dummy;
        const bool bHas = KC->GetLatestFact(RequiredFact, Dummy);

        if (!bHas)
        {
            if (auto* BB = OwnerComp.GetBlackboardComponent())
            {
                BB->SetValueAsBool(TEXT("AnnouncedFact"), false);
            }
        }
    }
}