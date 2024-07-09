// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformsGameInstance.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "PlatformTriggerAlt.h"
#include "Blueprint/UserWidget.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/MenuWidget.h"

UPuzzlePlatformsGameInstance::UPuzzlePlatformsGameInstance(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
    if (MenuBPClass.Class != nullptr) 
    {
        UE_LOG(LogTemp, Warning, TEXT("[UPuzzlePlatformsGameInstance]: MenuBPClass Class found!"));
        MenuClass = MenuBPClass.Class;
    }

    ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/MenuSystem/WBP_InGameMenu"));
    if (InGameMenuBPClass.Class != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UPuzzlePlatformsGameInstance]: InGameMenuBPClass Class found!"));
        InGameMenuClass = InGameMenuBPClass.Class;
    }
}

void UPuzzlePlatformsGameInstance::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("[UPuzzlePlatformsGameInstance][Init]: Found class %s"), *MenuClass->GetName());
}

void UPuzzlePlatformsGameInstance::Host()
{
    if (Menu != nullptr) 
    {
        Menu->Teardown();
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hosting ..."));

        UWorld* world = GetWorld();
        if (world != nullptr) {
            world->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
        }
    }
}

void UPuzzlePlatformsGameInstance::Join(const FString& Adress)
{
    if (Menu != nullptr)
    {
        Menu->Teardown();
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Joining %s"), *Adress));
        APlayerController* playerController = GetFirstLocalPlayerController();
        if (playerController != nullptr) 
        {
            playerController->ClientTravel(Adress,ETravelType::TRAVEL_Absolute);
        }
    }
}

void UPuzzlePlatformsGameInstance::LoadMenu()
{
    if (MenuClass == nullptr) 
    {
        return;
    }

    Menu = CreateWidget<UMainMenu>(this, MenuClass);
    if (Menu == nullptr) 
    {
        return;
    }

    /*menu->AddToViewport();

    APlayerController* playerController = GetFirstLocalPlayerController();
    if (playerController == nullptr)
    {
        return;
    }

    FInputModeUIOnly inputModeData;
    inputModeData.SetWidgetToFocus(menu->TakeWidget());
    inputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

    playerController->SetInputMode(inputModeData);
    playerController->bShowMouseCursor = true;*/

    Menu->bIsFocusable = true;
    Menu->Setup();
    Menu->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::InGameLoadMenu()
{
    if (InGameMenuClass == nullptr)
    {
        return;
    }

    UMenuWidget* inGameMenu = CreateWidget<UMenuWidget>(this, InGameMenuClass);
    if (inGameMenu == nullptr)
    {
        return;
    }

    inGameMenu->bIsFocusable = true;
    inGameMenu->Setup();
    inGameMenu->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::LoadMainMenu()
{
    APlayerController* playerController = GetFirstLocalPlayerController();
    if (playerController != nullptr)
    {
        playerController->ClientTravel("/Game/MenuSystem/MainMenu", ETravelType::TRAVEL_Absolute);
    }
}
