// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HiveMind_Example.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSomeTypeDelegate, float, SomeFloat, FString, SomeString); //Creates delagate

UCLASS()
class GROWTH_T7_API AHiveMind_Example : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHiveMind_Example();

	UPROPERTY(BlueprintAssignable) //Makes delegate available in blueprint.
	FSomeTypeDelegate DelegateExample;

	void TestFunctionHiddenFromBlueprint();

	UFUNCTION(BlueprintCallable) //Makes function available in blueprint.
	void TestFunction();

	UFUNCTION(BlueprintCallable)
	void TestFunctionWithParams(int64 SomeInt);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
