// Fill out your copyright notice in the Description page of Project Settings.

#include "PlatformTriggerAlt.h"
#include "Engine/Engine.h"
#include "Components/BoxComponent.h"
#include "MovingPlatform.h"

// Sets default values
APlatformTriggerAlt::APlatformTriggerAlt()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(FName("TriggerVolume"));
	if (!ensure(TriggerVolume != nullptr)) return;
	RootComponent = TriggerVolume;
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APlatformTriggerAlt::OnOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &APlatformTriggerAlt::OnOverlapEnd);
}

// Called when the game starts or when spawned
void APlatformTriggerAlt::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlatformTriggerAlt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlatformTriggerAlt::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("[PlatformTrigger][OnOverlapBegin]: This is a warning log message!"));
	for (AMovingPlatform* platform : PlatformsToTrigger)
	{
		platform->AddActiveTrigger();
	}
}

void APlatformTriggerAlt::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("[PlatformTrigger][OnOverlapEnd]: End!"));
	for (AMovingPlatform* platform : PlatformsToTrigger)
	{
		platform->RemoveActiveTrigger();
	}
}
