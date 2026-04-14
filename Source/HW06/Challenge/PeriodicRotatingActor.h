// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PeriodicRotatingActor.generated.h"

UCLASS()
class HW06_API APeriodicRotatingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APeriodicRotatingActor();

protected:
	// DefaultSceneRoot Component
	TObjectPtr<USceneComponent> sceneComponent;
	// StaticMesh Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> staticMeshComponent;

	// Rotating Speed per Seconds [deg/s]
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation Control")
	FVector rotatingSpeed = FVector(0, 0, 90);

	FTimerHandle rotatingHandler;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& propertyChangedEvent) override;
#endif //WITH_EDITOR

private:
	// 회전 적용 시간 간격
	UPROPERTY(EditAnywhere, BlueprintGetter = GetRotatingIntervalSeconds, BlueprintSetter = SetRotatingIntervalSeconds, Category = "Rotation Control")
	float rotatingIntervalSeconds = 0.1f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function which Blueprint can override
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RotatingAction();
	// Function define the basic behaviour of "RotatingAction" method
	// When Blueprint was not override "RotatingAction" method, this method will be called
	virtual void RotatingAction_Implementation();

	UFUNCTION(BlueprintPure, Category = "PeriodicRotatingActor|RotatingControl")
	inline float GetRotatingIntervalSeconds() const { return rotatingIntervalSeconds; }
	UFUNCTION(BlueprintCallable, Category = "PeriodicRotatingActor|RotatingControl")
	void SetRotatingIntervalSeconds(const float newIntervalSeconds);
};
