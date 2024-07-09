// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenu.h"
#include "Components/Button.h"

bool UInGameMenu::Initialize()
{
	bool success = Super::Initialize();
	if (!success) {
		return false;
	}

	if (CancelButton == nullptr || QuitButton == nullptr) {
		return false;
	}

	CancelButton->OnClicked.AddDynamic(this, &UInGameMenu::CloseCancelMenu);
	QuitButton->OnClicked.AddDynamic(this, &UInGameMenu::QuitGame);

	return true;
}

void UInGameMenu::CloseCancelMenu()
{
	Teardown();
}

void UInGameMenu::QuitGame()
{
	if (MenuInterface != nullptr) 
	{
		Teardown();
		MenuInterface->LoadMainMenu();
	}
}
