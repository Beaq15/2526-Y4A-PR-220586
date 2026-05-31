// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Test.h"
#include "Behaviortree/BlackboardComponent.h"

UBTT_Test::UBTT_Test()
{
	NodeName = TEXT("Print: Fact Detected");
}

EBTNodeResult::Type UBTT_Test::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    /*if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, TEXT("Fact.Test detected!"));
    }*/
    //if (auto* BB = OwnerComp.GetBlackboardComponent())
    //{
    //    BB->SetValueAsBool(TEXT("AnnouncedFact"), true);

    //    /*FString ProcessedKey = FString::Printf(TEXT("Processed_%s"), *FactToProcess.ToString());
    //    BB->SetValueAsBool(*ProcessedKey, true);*/
    //    BB->SetValueAsBool(TEXT("Processed_Fact"), true);
    //}
    return EBTNodeResult::Succeeded;
}