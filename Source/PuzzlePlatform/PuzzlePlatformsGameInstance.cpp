// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformsGameInstance.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "PlatformTriggerAlt.h"
#include "OnlineSessionSettings.h"

#include "Blueprint/UserWidget.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/MenuWidget.h"

const static FName SESSION_NAME = TEXT("My Session Game");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

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
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnJoinSessionComplete);
        }
    }
    else 
    {
        UE_LOG(LogTemp, Warning, TEXT("[UPuzzlePlatformsGameInstance][Init]: Found NO subsystem"));
    }

	UE_LOG(LogTemp, Warning, TEXT("[UPuzzlePlatformsGameInstance][Init]: Found class %s"), *MenuClass->GetName());
}

void UPuzzlePlatformsGameInstance::Host(FString ServerName)
{
    DesiredServerName = ServerName;
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
        if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
        {
            SessionSettings.bIsLANMatch = true;
        }
        else
        {
            SessionSettings.bIsLANMatch = false;
        }
        SessionSettings.NumPublicConnections = 2;
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.bUsesPresence = true;
        SessionSettings.bUseLobbiesIfAvailable = true;
        SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

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

void UPuzzlePlatformsGameInstance::RefreshServerList()
{
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    if (SessionSearch.IsValid())
    {
        //SessionSearch->bIsLanQuery = true;
        SessionSearch->MaxSearchResults = 100;
        SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
        UE_LOG(LogTemp, Warning, TEXT("Starting Find Session"));
        SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
    }
}

void UPuzzlePlatformsGameInstance::OnFindSessionsComplete(bool Success)
{
    if (Success && SessionSearch.IsValid() && Menu != nullptr)
    {
        TArray<FServerData> ServerNames;
        for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
        {
            UE_LOG(LogTemp, Warning, TEXT("Found session names: %s"), *SearchResult.GetSessionIdStr());
            FServerData Data;
            Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
            Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
            Data.HostUsername = SearchResult.Session.OwningUserName;
            FString ServerName;
            if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
            {
                Data.Name = ServerName;
            }
            else
            {
                Data.Name = "Could not find name.";
            }
            ServerNames.Add(Data);
        }
        Menu->SetServerList(ServerNames);
        UE_LOG(LogTemp, Warning, TEXT("Finished Find Session"));
    }
}

void UPuzzlePlatformsGameInstance::Join(uint32 Index)
{
    if (!SessionInterface.IsValid()) return;
    if (!SessionSearch.IsValid()) return;

    if (Menu != nullptr)
    {
        Menu->Teardown();
    }

    SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}

void UPuzzlePlatformsGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (!SessionInterface.IsValid()) return;

    FString Address;
    if (!SessionInterface->GetResolvedConnectString(SessionName, Address)) {
        UE_LOG(LogTemp, Warning, TEXT("Could not get connect string."));
        return;
    }

    UEngine* Engine = GetEngine();
    if (!ensure(Engine != nullptr)) return;

    Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));

    APlayerController* PlayerController = GetFirstLocalPlayerController();
    if (!ensure(PlayerController != nullptr)) return;

    PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
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
