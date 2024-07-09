// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"

void UMenuWidget::Setup()
{
	this->AddToViewport();
	UWorld* world = GetWorld();
	if (world == nullptr)
	{
		return;
	}

	APlayerController* playerController = world->GetFirstPlayerController();
	if (playerController == nullptr)
	{
		return;
	}

	FInputModeUIOnly inputModeData;
	inputModeData.SetWidgetToFocus(this->TakeWidget());
	inputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	playerController->SetInputMode(inputModeData);
	playerController->bShowMouseCursor = true;
}

void UMenuWidget::Teardown()
{
	this->RemoveFromViewport();

	UE_LOG(LogTemp, Warning, TEXT("[UMainMenu][Teardown]: Init"));

	UWorld* world = GetWorld();
	if (world == nullptr)
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("[UMainMenu][Teardown]: World correct"));

	APlayerController* playerController = world->GetFirstPlayerController();
	if (playerController == nullptr)
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("[UMainMenu][Teardown]: PlayerController correct"));

	FInputModeGameOnly inputModeGame;
	playerController->SetInputMode(inputModeGame);
	playerController->bShowMouseCursor = false;
	bIsFocusable = true;
}

void UMenuWidget::SetMenuInterface(IMenuInterface* menuInterface)
{
	MenuInterface = menuInterface;
}