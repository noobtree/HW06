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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> staticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Control")
	bool bIsTowardStartLocation = false;
	// 시작 위치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Control")
	FVector startLocation;
	// 이동 방향
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Control")
	FVector moveDirection = FVector(0, 0, 0);
	// 이동 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Control")
	float moveDistance = 100;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& propertyChangedEvent) override;
#endif //WITH_EDITOR

protected:
	// 이동 적용 시간 간격
	UPROPERTY(EditAnywhere, BlueprintGetter = GetMovingIntervalSeconds, BlueprintSetter = SetMovingIntervalSeconds, Category = "Movement Control")
	float movingIntervalSeconds = 1.5f;

	// 주기적으로 연결된 함수를 호출하는 타이머
	FTimerHandle movingHandler;

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
	FORCEINLINE float GetMovingIntervalSeconds() const { return movingIntervalSeconds; }
	UFUNCTION(BlueprintCallable, Category = "PeriodicMovingPlatformActor|MovingControl")
	void SetMovingIntervalSeconds(const float newIntervalSeconds);
};
