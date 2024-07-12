// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformsGameInstance.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "PlatformTriggerAlt.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "Blueprint/UserWidget.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/MenuWidget.h"

const static FName SESSION_NAME = TEXT("My Session Game");

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
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (Subsystem != nullptr) 
    {
        UE_LOG(LogTemp, Warning, TEXT("[UPuzzlePlatformsGameInstance][Init]: Found subsystem %s"), *Subsystem->GetSubsystemName().ToString());
        SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid()) {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnCreateSessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnDestroySessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnFindSessionsComplete);

            SessionSearch = MakeShareable(new FOnlineSessionSearch());
            if (SessionSearch.IsValid())
            {
                UE_LOG(LogTemp, Warning, TEXT("Starting Find Session"));
                SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
            }
        }
    }
    else 
    {
        UE_LOG(LogTemp, Warning, TEXT("[UPuzzlePlatformsGameInstance][Init]: Found NO subsystem"));
    }

	UE_LOG(LogTemp, Warning, TEXT("[UPuzzlePlatformsGameInstance][Init]: Found class %s"), *MenuClass->GetName());
}

void UPuzzlePlatformsGameInstance::Host()
{
    if (SessionInterface.IsValid())
    {
        auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
        if (ExistingSession != nullptr)
        {
            SessionInterface->DestroySession(SESSION_NAME);
        }
        else
        {
            CreateSession();
        }
    }
}

void UPuzzlePlatformsGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
    if (Success) {
        CreateSession();
    }
}

void UPuzzlePlatformsGameInstance::CreateSession()
{
    if (SessionInterface.IsValid()) {
        FOnlineSessionSettings SessionSettings;
        SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
    }
}

void UPuzzlePlatformsGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
    if (!Success)
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not create session"));
        return;
    }

    if (Menu != nullptr)
    {
        Menu->Teardown();
    }

    UEngine* Engine = GetEngine();
    if (!ensure(Engine != nullptr)) return;

    Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

    UWorld* World = GetWorld();
    if (!ensure(World != nullptr)) return;

    World->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
}

void UPuzzlePlatformsGameInstance::OnFindSessionsComplete(bool Success)
{
    UE_LOG(LogTemp, Warning, TEXT("Finished Find Session"));
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

void UPuzzlePlatformsGameInstance::LoadMenuWidget()
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
