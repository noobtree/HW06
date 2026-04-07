// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingPlatformActor.generated.h"

// Forward Declaration
class UInputMappingContext;
class UInputAction;

UCLASS()
class HW06_API AMovingPlatformActor : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AMovingPlatformActor();

protected:
	// DefaultSceneRoot Component
	TObjectPtr<USceneComponent> sceneComponent;
	// StaticMesh Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement Control")
	TObjectPtr<UStaticMeshComponent> staticMeshComponent;

	// Moving Speed per Seconds [cm/s]
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Control")
	FVector movingSpeed = FVector(100, 0, 0);

	// InputMappingContext Asset (EnhancedInput)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputMappingContext> inputMappingContextAsset;
	// InputAction Asset (EnhancedInput)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> triggerInputActionAsset;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function which Blueprint can override
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MovingAction();
	// Function define the basic behaviour of "MovingAction" method
	// When Blueprint was not override "MovingAction" method, this method will be called
	virtual void MovingAction_Implementation();
};
