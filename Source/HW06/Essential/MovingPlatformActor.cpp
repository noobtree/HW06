// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatformActor.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"

// Sets default values
AMovingPlatformActor::AMovingPlatformActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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

	// InputMappingContext 에셋 로드
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> imc_uasset(TEXT("/Game/Inputs/IMC_Player"));
	inputMappingContextAsset = imc_uasset.Succeeded() == true ? imc_uasset.Object : nullptr;

	// InputAction 에셋 로드
	static ConstructorHelpers::FObjectFinder<UInputAction> ia_uasset(TEXT("/Game/Inputs/IA_Interaction"));
	triggerInputActionAsset = ia_uasset.Succeeded() == true ? ia_uasset.Object : nullptr;
}

// Called when the game starts or when spawned
void AMovingPlatformActor::BeginPlay()
{
	Super::BeginPlay();

	// 엔진 유효성 확인
	check(GEngine != nullptr);
	PrimaryActorTick.bCanEverTick = true;

	// Debug 메시지 출력
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("MovingPlatformActor - BeginPlay!"));

	// 0번 인덱스의 PlayerController 얻기
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	if (playerController != nullptr)
	{
		// 현재 Actor에서 사용자 입력을 받을 수 있도록 허용하기
		EnableInput(playerController);

		// playerController 객체가 로컬 플레이어인지 확인
		if (ULocalPlayer* localPlayer = playerController->GetLocalPlayer())
		{
			// 로컬 플레이어의 입력 시스템 얻기
			if (UEnhancedInputLocalPlayerSubsystem* inputSubSystem = localPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				// InputMappingContext 에셋 등록
				if (inputMappingContextAsset != nullptr)
				{
					inputSubSystem->AddMappingContext(inputMappingContextAsset, 0);
				}
			}

			// 기존의 입력 컴포넌트를 향상된 입력 컴포넌트로 변환
			if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
			{
				// InputAction 이벤트 연결
				if (triggerInputActionAsset != nullptr)
				{
					// Triggered 이벤트에 MovingAction 함수 연결
					enhancedInputComponent->BindAction(triggerInputActionAsset, ETriggerEvent::Triggered, this, &AMovingPlatformActor::MovingAction);
				}
			}
		}
	}
}

// Called every frame
void AMovingPlatformActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//MovingAction();
}

void AMovingPlatformActor::MovingAction_Implementation()
{
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	FVector deltaLocation = movingSpeed * DeltaTime;

	bool bSweep = true;
	FHitResult sweepResult;

	AddActorLocalOffset(deltaLocation, bSweep, &sweepResult);
}

