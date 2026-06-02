// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetPlayerHUD.h"

void UWidgetPlayerHUD::ShowCrosshair()
{
	Crosshair->SetVisibility(ESlateVisibility::Visible);
}

void UWidgetPlayerHUD::HideCrosshair()
{
	Crosshair->SetVisibility(ESlateVisibility::Hidden);
}
