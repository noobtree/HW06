// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatformActor.h"

// Sets default values
AMovingPlatformActor::AMovingPlatformActor()
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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("/Game/Geometries/SM_Wall"));

	// 로드한 에셋이 유효한지 확인
	if (meshAsset.Succeeded() == true)
	{	// 로드 성공
		// 컴포넌트의 StaticMesh 프로퍼티에 로드된 에셋을 할당
		staticMeshComponent->SetStaticMesh(meshAsset.Object);
	}
}

// Called when the game starts or when spawned
void AMovingPlatformActor::BeginPlay()
{
	Super::BeginPlay();

	// 엔진 유효성 확인
	check(GEngine != nullptr);

	// Debug 메시지 출력
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("MovingPlatformActor - BeginPlay!"));

	startLocation = GetActorLocation();
}

// Called every frame
void AMovingPlatformActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MovingAction();
}

void AMovingPlatformActor::MovingAction_Implementation()
{
	// 프레임 시간 간격 얻기
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	// 선형 보간에 사용될 계수를 위한 변수 계산
	float t = GetWorld()->GetTimeSeconds();
	float frequency = (2 * PI) / (maxRange / movingSpeed);

	// 끝 지점 좌표 계산
	FVector endLocation = startLocation + maxRange * moveDirection.GetSafeNormal();

	// 위치 선형 보간
	float lerpFactor = 0.5 * FMath::Cos(frequency * t - PI) + 0.5f;
	FVector newActorLocation = FMath::Lerp(startLocation, endLocation, lerpFactor);

	// 이동 적용
	bool bSweep = true;
	FHitResult sweepResult;
	SetActorLocation(newActorLocation, bSweep, &sweepResult);
}

