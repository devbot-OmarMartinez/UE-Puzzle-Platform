// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformsGameInstance.h"
#include "Engine/Engine.h"

UPuzzlePlatformsGameInstance::UPuzzlePlatformsGameInstance(const FObjectInitializer& ObjectInitializer)
{
	UE_LOG(LogTemp, Warning, TEXT("[UPuzzlePlatformsGameInstance]: This is a constructor!"));
}

void UPuzzlePlatformsGameInstance::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("[UPuzzlePlatformsGameInstance][Init]: Init"));
}

void UPuzzlePlatformsGameInstance::Host()
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hosting!"));

        UWorld* world = GetWorld();
        if (world != nullptr) {
            world->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
        }
    }
}

void UPuzzlePlatformsGameInstance::Join(const FString& Adress)
{
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
