// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PeriodicMovingPlatformActor.generated.h"

UCLASS()
class HW06_API APeriodicMovingPlatformActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APeriodicMovingPlatformActor();

protected:
	// DefaultSceneRoot Component
	TObjectPtr<USceneComponent> sceneComponent;
	// StaticMesh Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PeriodicMovingPlatformActor|Rendering")
	TObjectPtr<UStaticMeshComponent> staticMeshComponent;

	// Moving Speed per Seconds [cm/s]
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PeriodicMovingPlatformActor|MovingControl")
	FVector movingSpeed = FVector(100, 0, 0);

	FTimerHandle movingHandler;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& propertyChangedEvent) override;
#endif //WITH_EDITOR

private:
	// 회전 적용 시간 간격
	UPROPERTY(EditAnywhere, BlueprintGetter = GetMovingIntervalSeconds, BlueprintSetter = SetMovingIntervalSeconds, Category = "PeriodicMovingPlatformActor|MovingControl")
	float movingIntervalSeconds = 0.1f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function which Blueprint can override
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MovingAction();
	// Function define the basic behaviour of "MovingAction" method
	// When Blueprint was not override "MovingAction" method, this method will be called
	virtual void MovingAction_Implementation();

	UFUNCTION(BlueprintPure, Category = "PeriodicMovingPlatformActor|MovingControl")
	inline float GetMovingIntervalSeconds() const { return movingIntervalSeconds; }
	UFUNCTION(BlueprintCallable, Category = "PeriodicMovingPlatformActor|MovingControl")
	void SetMovingIntervalSeconds(const float newIntervalSeconds);
};
