// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetPlayerDeath.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UWidgetPlayerDeath::NativeConstruct()
{
	Super::NativeConstruct();

	if (ButtonRestart)
	{
		ButtonRestart->OnClicked.AddDynamic(this, &UWidgetPlayerDeath::OnRestartClicked);
	}

	if (ButtonExit)
	{
		ButtonExit->OnClicked.AddDynamic(this, &UWidgetPlayerDeath::OnExitClicked);
	}
}

void UWidgetPlayerDeath::OnRestartClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
	}

	const FName LevelName(*UGameplayStatics::GetCurrentLevelName(this, true));
	UGameplayStatics::OpenLevel(this, LevelName);
}

void UWidgetPlayerDeath::OnExitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
