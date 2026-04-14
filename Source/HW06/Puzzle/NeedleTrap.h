// Fill out your copyright notice in the Description page of Project Settings.
// 주기적으로 InvokeTrap 함수를 호출, 설정한 Timeline에 따라 작동
// Timeline에서 설정한 값을 따라 trapSubject의 Mesh가 위아래로 반복하여 이동

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"	// UTimelineComponent, FOnTimelineFloat
#include "NeedleTrap.generated.h"

UCLASS()
class HW06_API ANeedleTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANeedleTrap();

	// DefaultSceneRoot Component
	TObjectPtr<USceneComponent> sceneComponent;

	// 함정의 외부 틀
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "NeedleTrap|Mesh")
	TObjectPtr<UStaticMeshComponent> trapFrame;
	// 함정의 데미지를 주는 주체
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "NeedleTrap|Mesh")
	TObjectPtr<UStaticMeshComponent> trapSubject;

	// 함정을 부드럽게 동작 시키기 위한 Timeline 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "NeedleTrap|Timeline")
	TObjectPtr<class UTimelineComponent> trapTimeline;
	// TimelineComponent에 적용할 float 흐름을 정의한 에셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NeedleTrap|Timeline")
	TObjectPtr<UCurveFloat> trapTimelineFloatCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NeedleTrap|Settings")
	FVector initaialLocalLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NeedleTrap|Settings")
	FVector invokedLocalLocation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 함정을 발동시키는 함수
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InvokeTrap();
	// 함정 발동 기본 동작을 정의하는 함수
	virtual void InvokeTrap_Implementation();

private:
	// 함정을 주기적으로 발동시키기 위한 타이머
	FTimerHandle trapTimerHandle;

	// Timeline에 적용된 곡선을 따라 float 값이 갱신될 때, 호출할 함수들을 담는 델리게이트
	FOnTimelineFloat UpdateFunctionFloat;

	// Timeline에서 값을 받아서 함정의 동작을 적용하는
	UFUNCTION()
	void UpdateTrapSubjectLocalLocation(float timelineOutput);

};
