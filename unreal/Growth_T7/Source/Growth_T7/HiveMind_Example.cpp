// Fill out your copyright notice in the Description page of Project Settings.


#include "HiveMind_Example.h"

void AHiveMind_Example::TestFunctionHiddenFromBlueprint()
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("C++ only function activated!"));
}


void AHiveMind_Example::TestFunction()
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Test function activated!"));

	TestFunctionHiddenFromBlueprint();
}

void AHiveMind_Example::TestFunctionWithParams(int64 SomeInt)
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Integer value: %i"), SomeInt));
}

// Sets default values
AHiveMind_Example::AHiveMind_Example()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHiveMind_Example::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHiveMind_Example::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DelegateExample.Broadcast(DeltaTime, "Delta Time"); //output example
}

