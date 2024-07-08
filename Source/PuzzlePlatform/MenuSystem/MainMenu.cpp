// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

bool UMainMenu::Initialize()
{
	bool success = Super::Initialize();
	if (!success) {
		return false;
	}

	if (HostButton == nullptr || JoinButton == nullptr || BackButton == nullptr) {
		return false;
	}

	HostButton->OnClicked.AddDynamic(this, &UMainMenu::HostService);
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);
	BackButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	return true;
}


void UMainMenu::SetMenuInterface(IMenuInterface* menuInterface)
{
	MenuInterface = menuInterface;
}

void UMainMenu::Setup()
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

void UMainMenu::Teardown()
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


void UMainMenu::HostService()
{
	UE_LOG(LogTemp, Warning, TEXT("[UMainMenu][HostService]: I am going to host the server!"));
	if (MenuInterface != nullptr) 
	{
		MenuInterface->Host();
	}
}

void UMainMenu::OpenJoinMenu() 
{
	if (MenuSwitcher == nullptr || JoinMenu == nullptr)
	{
		return;
	}

	MenuSwitcher->SetActiveWidget(JoinMenu);

}

void UMainMenu::OpenMainMenu()
{
	if (MenuSwitcher == nullptr || MainMenu == nullptr)
	{
		return;
	}

	MenuSwitcher->SetActiveWidget(MainMenu);
}
