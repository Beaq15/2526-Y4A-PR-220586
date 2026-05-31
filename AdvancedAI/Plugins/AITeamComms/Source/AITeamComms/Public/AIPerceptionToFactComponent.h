#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIPerceptionToFactComponent.generated.h"

class UAIPerceptionComponent;
class UAIKnowledgeComponent;

UCLASS(ClassGroup = (AI), meta = (BlueprintSpawnableComponent))
class AITEAMCOMMS_API UAIPerceptionToFactComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAIPerceptionToFactComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
    float SightFactTTL = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
    float HearingFactTTL = 5.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
    float DamageFactTTL = 15.f;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    UAIPerceptionComponent* PerceptionComp;

    UPROPERTY()
    UAIKnowledgeComponent* KnowledgeComp;

    UFUNCTION()
    void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    void HandleSightStimulus(AActor* Actor, const FAIStimulus& Stimulus);
    void HandleHearingStimulus(AActor* Actor, const FAIStimulus& Stimulus);
    void HandleDamageStimulus(AActor* Actor, const FAIStimulus& Stimulus);
};