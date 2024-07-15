// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/TextBlock.h"
#include "ServerRow.h"

UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));
	if (!ensure(ServerRowBPClass.Class != nullptr)) return;

	ServerRowClass = ServerRowBPClass.Class;
}

bool UMainMenu::Initialize()
{
	bool success = Super::Initialize();
	if (!success) {
		return false;
	}

	if (HostButton == nullptr || JoinButton == nullptr || BackButton == nullptr || ConfirmJoinButton == nullptr || ExitButton == nullptr) {
		return false;
	}

	HostButton->OnClicked.AddDynamic(this, &UMainMenu::HostService);
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);
	BackButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);
	ConfirmJoinButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);
	ExitButton->OnClicked.AddDynamic(this, &UMainMenu::ExitGame);

	return true;
}

void UMainMenu::HostService()
{
	UE_LOG(LogTemp, Warning, TEXT("[UMainMenu][HostService]: I am going to host the server!"));
	if (MenuInterface != nullptr) 
	{
		MenuInterface->Host();
	}
}

void UMainMenu::SetServerList(TArray<FString> ServerNames)
{
	UWorld* World = this->GetWorld();
	if (!ensure(World != nullptr)) return;

	ServerList->ClearChildren();

	uint32 i = 0;
	for (const FString& ServerName : ServerNames)
	{
		UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);
		if (!ensure(Row != nullptr)) return;

		Row->ServerName->SetText(FText::FromString(ServerName));
		Row->Setup(this, i);
		++i;

		ServerList->AddChild(Row);
	}
}

void UMainMenu::SelectIndex(uint32 Index)
{
	SelectedIndex = Index;
}

void UMainMenu::JoinServer()
{
	if (SelectedIndex.IsSet())
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index %d."), SelectedIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index not set."));
	}
	if (MenuInterface == nullptr)
	{
		return;
	}

	/*const FString address = IPAddressField->GetText().ToString();
	MenuInterface->Join(address);*/

	MenuInterface->Join("");
}

void UMainMenu::OpenJoinMenu() 
{
	if (MenuSwitcher == nullptr || JoinMenu == nullptr)
	{
		return;
	}

	MenuSwitcher->SetActiveWidget(JoinMenu);
	if (MenuInterface != nullptr) {
		MenuInterface->RefreshServerList();
	}
}

void UMainMenu::OpenMainMenu()
{
	if (MenuSwitcher == nullptr || MainMenu == nullptr)
	{
		return;
	}

	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::ExitGame()
{
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

	playerController->ConsoleCommand("quit");
}
