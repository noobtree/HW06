// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingPlatformActor.generated.h"

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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> staticMeshComponent;

	// 시작 위치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Control")
	FVector startLocation;
	// 이동 방향
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Control")
	FVector moveDirection = FVector(0, 0, 0);
	// 이동 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Control")
	float maxRange = 100;
	// Moving Speed per Seconds [cm/s]
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Control")
	float movingSpeed = 100;

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
