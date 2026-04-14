// Fill out your copyright notice in the Description page of Project Settings.


#include "RotatingActor.h"

// Sets default values
ARotatingActor::ARotatingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 초기 컴포넌트 생성
	sceneComponent = CreateDefaultSubobject<USceneComponent>(FName("DefaultSceneRoot"));
	staticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMeshComponent"));

	// Root 컴포넌트 설정
	SetRootComponent(sceneComponent);

	// StaticMeshComponent의 부모 설정
	staticMeshComponent->SetupAttachment(sceneComponent);

	// 임의 경로에 위치하는 특정 타입의 에셋 로드
	// FilePath = (SolutionDir)\Contents\Geometries\SM_Pillar.uaaset
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("/Game/Geometries/SM_Pillar"));

	// 로드한 에셋이 유효한지 확인
	if (meshAsset.Succeeded() == true)
	{	// 로드 성공
		// 컴포넌트의 StaticMesh 프로퍼티에 로드된 에셋을 할당
		staticMeshComponent->SetStaticMesh(meshAsset.Object);
	}
}

// Called when the game starts or when spawned
void ARotatingActor::BeginPlay()
{
	Super::BeginPlay();

	// 엔진 유효성 확인
	check(GEngine != nullptr);

	// Debug 메시지 출력
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("RotatingActor - BeginPlay!"));
}

// Called every frame
void ARotatingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotatingAction();
}

void ARotatingActor::RotatingAction_Implementation()
{
	// 프레임 시간 간격 얻기
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	// 시간 비율에 따른 변화량 계산
	FVector deltaEuler = rotatingSpeed * DeltaTime;

	// 회전 적용
	FRotator deltaRotator = FRotator::MakeFromEuler(deltaEuler);
	bool bSweep = true;
	AddActorLocalRotation(deltaRotator, bSweep);
}

