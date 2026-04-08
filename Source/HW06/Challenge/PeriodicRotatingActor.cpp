// Fill out your copyright notice in the Description page of Project Settings.


#include "PeriodicRotatingActor.h"

// Sets default values
APeriodicRotatingActor::APeriodicRotatingActor()
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
void APeriodicRotatingActor::BeginPlay()
{
	Super::BeginPlay();

	// 엔진 유효성 확인
	check(GEngine != nullptr);

	// Debug 메시지 출력
	FString debugMessage = FString::Printf(TEXT("%s - BeginPlay!"), *GetName());
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, debugMessage);

	// rotatingHandler 타이머 설정 (5초 후부터 현재 인스턴스의 RotatingAction 함수를 일정 시간 간격으로 반복하여 호출한다.)
	bool bIsLoop = true;	// 반복 설정
	float startDelay = 5;	// 최초 시간 지연 5초
	GetWorldTimerManager().SetTimer(rotatingHandler, this, &APeriodicRotatingActor::RotatingAction, rotatingIntervalSeconds, bIsLoop, startDelay);
}

#if WITH_EDITOR
void APeriodicRotatingActor::PostEditChangeProperty(FPropertyChangedEvent& propertyChangedEvent)
{
	Super::PostEditChangeProperty(propertyChangedEvent);

	// Unreal Editor에서 rotatingIntervalSeconds 변수값 변경 시
	if (propertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(APeriodicRotatingActor, rotatingIntervalSeconds))
	{
		// Setter 함수 호출
		SetRotatingIntervalSeconds(rotatingIntervalSeconds);
	}
}
#endif //WITH_EDITOR

// Called every frame
void APeriodicRotatingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APeriodicRotatingActor::RotatingAction_Implementation()
{
	// 설정된 타이머의 시간 간격 얻기
	float DeltaTime = GetWorldTimerManager().GetTimerRate(rotatingHandler);

	// 시간 비율에 따른 변화량 계산
	FVector deltaEuler = rotatingSpeed * DeltaTime;

	// 회전 적용
	FRotator deltaRotator = FRotator::MakeFromEuler(deltaEuler);
	bool bSweep = true;
	AddActorLocalRotation(deltaRotator, bSweep);
}

void APeriodicRotatingActor::SetRotatingIntervalSeconds(const float newIntervalSeconds)
{
	// 변수값 변경
	rotatingIntervalSeconds = newIntervalSeconds;

	if (rotatingIntervalSeconds > 0)
	{
		// 기존 타이머가 실행중인 경우 현재까지의 회전을 적용
		if (GetWorldTimerManager().IsTimerActive(rotatingHandler) == true)
		{
			// 기존 타이머의 경과 시간 확인
			float DeltaTime = GetWorldTimerManager().GetTimerElapsed(rotatingHandler);

			// 시간 비율에 따른 변화량 계산
			FVector deltaEuler = rotatingSpeed * DeltaTime;

			// 회전 적용
			FRotator deltaRotator = FRotator::MakeFromEuler(deltaEuler);
			bool bSweep = true;
			AddActorLocalRotation(deltaRotator, bSweep);
		}

		// 게임 실행 중인 경우
		if (GetWorld() && GetWorld()->IsGameWorld() == true)
		{
			bool bIsLoop = true;
			// 타이머 설정 또는 덮어쓰기
			GetWorldTimerManager().SetTimer(rotatingHandler, this, &APeriodicRotatingActor::RotatingAction, rotatingIntervalSeconds, bIsLoop);
		}
	}
	else
	{
		// 타이머 중단
		GetWorldTimerManager().ClearTimer(rotatingHandler);
	}
}
