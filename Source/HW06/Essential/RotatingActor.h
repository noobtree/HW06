// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RotatingActor.generated.h"

// Forward Declaration
class UInputMappingContext;
class UInputAction;

UCLASS()
class HW06_API ARotatingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARotatingActor();

protected:
	// DefaultSceneRoot Component
	TObjectPtr<USceneComponent> sceneComponent;
	// StaticMesh Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Rotating Control")
	TObjectPtr<UStaticMeshComponent> staticMeshComponent;

	// Rotating Speed per Seconds [deg/s]
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotating Control")
	FVector rotatingSpeed = FVector(0, 0, 90);

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
	void RotatingAction();
	// Function define the basic behaviour of "RotatingAction" method
	// When Blueprint was not override "RotatingAction" method, this method will be called
	virtual void RotatingAction_Implementation();
};
