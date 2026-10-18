// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetVictoryScreen.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UWidgetVictoryScreen::NativeConstruct()
{
	Super::NativeConstruct();

	if (ButtonRestart)
	{
		ButtonRestart->OnClicked.AddDynamic(this, &UWidgetVictoryScreen::OnRestartClicked);
	}

	if (ButtonExit)
	{
		ButtonExit->OnClicked.AddDynamic(this, &UWidgetVictoryScreen::OnExitClicked);
	}
}

void UWidgetVictoryScreen::OnRestartClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
	}

	const FName LevelName(*UGameplayStatics::GetCurrentLevelName(this, true));
	UGameplayStatics::OpenLevel(this, LevelName);
}

void UWidgetVictoryScreen::OnExitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}