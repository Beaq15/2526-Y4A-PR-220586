#include "EnvQueryContext_FactFromBlackboard.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"

void UEnvQueryContext_FactFromBlackboard::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
    UObject* QuerierObject = QueryInstance.Owner.Get();
    if (!QuerierObject)
    {
        return;
    }

    AAIController* AIC = Cast<AAIController>(QuerierObject);
    if (!AIC)
    {
        APawn* Pawn = Cast<APawn>(QuerierObject);
        if (Pawn)
        {
            AIC = Cast<AAIController>(Pawn->GetController());
        }
    }

    if (!AIC)
    {
        return;
    }

    UBlackboardComponent* BB = AIC->GetBlackboardComponent();
    if (!BB)
    {
        return;
    }

    // Hardcoded to read EnemySighted_Location
    FVector Location = BB->GetValueAsVector(FName("EnemySighted_Location"));

    if (!Location.IsZero())
    {
        UEnvQueryItemType_Point::SetContextHelper(ContextData, Location);

        /*if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan,
                FString::Printf(TEXT("EQS Context: Using EnemySighted_Location = %s"),
                    *Location.ToString()));
        }*/
    }
    else
    {
        // Fallback to querier location
        if (AIC->GetPawn())
        {
            UEnvQueryItemType_Point::SetContextHelper(ContextData, AIC->GetPawn()->GetActorLocation());
        }
    }
}