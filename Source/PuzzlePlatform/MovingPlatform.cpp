// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	SetMobility(EComponentMobility::Movable);
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority()) {
		SetReplicates(true);
		SetReplicateMovement(true);
	}

	GlobalStartLocation = GetActorLocation();
	GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);
}

void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActiveTriggers > 0) 
	{
		if (HasAuthority()) {		
			FVector location = GetActorLocation();
			float journeyLength = (GlobalTargetLocation - GlobalStartLocation).Size();
			float journeyTravelled = (location - GlobalStartLocation).Size();

			if (journeyTravelled >= journeyLength) {
				FVector Swap = GlobalStartLocation;
				GlobalStartLocation = GlobalTargetLocation;
				GlobalTargetLocation = Swap;
			}

			FVector direction = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal();

			location += Speed * DeltaTime * direction;
			SetActorLocation(location);
		}
	}
}

void AMovingPlatform::AddActiveTrigger()
{
	ActiveTriggers++;
}

void AMovingPlatform::RemoveActiveTrigger()
{
	if (ActiveTriggers > 0) {
		ActiveTriggers--;
	}
}
