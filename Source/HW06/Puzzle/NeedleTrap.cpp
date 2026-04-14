// Fill out your copyright notice in the Description page of Project Settings.


#include "NeedleTrap.h"

// Sets default values
ANeedleTrap::ANeedleTrap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 초기 컴포넌트 생성
	sceneComponent = CreateDefaultSubobject<USceneComponent>(FName("DefaultSceneRoot"));
	trapFrame = CreateDefaultSubobject<UStaticMeshComponent>(FName("TrapFrame"));
	trapSubject = CreateDefaultSubobject<UStaticMeshComponent>(FName("TrapSubject"));
	trapTimeline = CreateDefaultSubobject<UTimelineComponent>(FName("TimelineComponent"));


	// 임의 경로에 위치하는 특정 타입의 에셋 로드
	static ConstructorHelpers::FObjectFinder<UStaticMesh> trapFrameMeshAsset(TEXT("/Game/Geometries/SM_Trap_Frame"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> trapSubjectMeshAsset(TEXT("/Game/Geometries/SM_Trap_Needle"));

	// 컴포넌트에 로드된 에셋 할당
	if (trapFrameMeshAsset.Succeeded() == true)
	{
		trapFrame->SetStaticMesh(trapFrameMeshAsset.Object);
	}
	if (trapSubjectMeshAsset.Succeeded() == true)
	{
		trapSubject->SetStaticMesh(trapSubjectMeshAsset.Object);
	}

	// Root 컴포넌트 설정
	SetRootComponent(sceneComponent);

	// StaticMeshComponent의 부모 설정
	trapFrame->SetupAttachment(sceneComponent);
	trapSubject->AttachToComponent(trapFrame, FAttachmentTransformRules::KeepRelativeTransform);

	// StaticMeshComponent 초기 위치 조정
	initaialLocalLocation = FVector(0, 0, -100);
	invokedLocalLocation = FVector(0, 0, 0);
	trapSubject->SetRelativeLocation(initaialLocalLocation, false, nullptr, ETeleportType::ResetPhysics);
}

// Called when the game starts or when spawned
void ANeedleTrap::BeginPlay()
{
	Super::BeginPlay();
	
	check(GEngine != nullptr);

	// Timeline에 설정한 CurveFloat를 따라 변화하는 float 값을 처리하는 함수 연결
	UpdateFunctionFloat.BindDynamic(this, &ANeedleTrap::UpdateTrapSubjectLocalLocation);

	// UCurveFloat 에셋이 유효하면 델리게이트 UpdateFunctionFloat 연결
	if (trapTimelineFloatCurve != nullptr)
	{
		trapTimeline->AddInterpFloat(trapTimelineFloatCurve, UpdateFunctionFloat);
	}

	// 5초마다 함정 발동
	GetWorldTimerManager().SetTimer(trapTimerHandle, this, &ANeedleTrap::InvokeTrap, 5, true, 1);
}

// Called every frame
void ANeedleTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANeedleTrap::InvokeTrap_Implementation()
{
	// 타임라인을 처음부터 시작
	trapTimeline->PlayFromStart();
}

void ANeedleTrap::UpdateTrapSubjectLocalLocation(float timelineOutput)
{
	// set trapSubject mesh's new relative location based on the output from Timeline Curve
	FVector subjectLocalLocation = FMath::Lerp(initaialLocalLocation, invokedLocalLocation, timelineOutput);
	trapSubject->SetRelativeLocation(subjectLocalLocation, true);
}

