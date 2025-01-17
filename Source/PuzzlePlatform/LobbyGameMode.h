// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PuzzlePlatformGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORM_API ALobbyGameMode : public APuzzlePlatformGameMode
{
	GENERATED_BODY()
	
public:

	void PostLogin(APlayerController* NewPlayer) override;

	void Logout(AController* Exiting) override;

private:
	void StartGame();

	uint32 NumberOfPlayers = 0;

	FTimerHandle GameStartTimer;
};
