// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PolarCell.h"
#include "PolarGridMazeGenerator.generated.h"

UCLASS()
class HW06_API APolarGridMazeGenerator : public AActor
{
	GENERATED_BODY()
	
public:
	// DefaultSceneRoot Component
	TObjectPtr<USceneComponent> sceneComponent;

	// 생성 시드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze Settings")
	int32 randomSeed = 271828;

	// 미로 생성 시 디버그 선 표시 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze Settings")
	bool bIsDebugMode = true;

	// 중심(시작) 원 반지름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "50.0"), Category = "Maze Settings|Polar")
	float centerRadius = 200.0;

	// 중심 원을 제외한 원의 개수 (최외곽 원 포함
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"), Category = "Maze Settings|Polar")
	float ringCount = 4;

	// 각 Ring 사이의 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "50.0"), Category = "Maze Settings|Polar")
	float ringDistance = 200;

	// 최초 원 분할 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "4"), Category = "Maze Settings|Polar")
	int32 baseCellCount = 4;

	// Sets default values for this actor's properties
	APolarGridMazeGenerator();

protected:
	// 원형 격자 맵
	TArray<TArray<FPolarCell>> polarGrid;

	// 난수 생성기
	FRandomStream randStream;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 미로를 생성하는 함수
	void GenerateMaze();

	// polarGrid에 존재하는 Cell 정보를 초기화하는 함수
	void InitializeCells();

	// 임의 순서에 해당하는 Ring에 위치한 Cell의 개수를 반환하는 함수
	int32 GetCellCountInRing(int32 ringIndex) const;

	// 미로의 길을 찾는 함수
	void RecursiveBacktracking();
	void Wilson();

	// 기준 Cell에 인접한 Cell의 좌표 목록을 반환하는 함수
	TArray<TPair<int32, int32>> GetAdjacentCells(const TPair<int32, int32>& cellPosition) const;

	// 임의 좌표가 현재 미로 내부에 위치하는지 확인하는 함수
	bool IsValidCell(const TPair<int32,int32>& cellPosition) const;

	// 임의 두 좌표에 해당하는 Cell 사이의 벽을 제거하는 함수
	void RemoveWallBetweenCells(const TPair<int32, int32>& lhs, const TPair<int32, int32>& rhs);

	// DebugDraw- 함수를 사용하여 생성된 미로를 그리는 함수
	void DrawDebugMaze() const;

	// polarGrid에서 해당 위치에 존재하는 Cell의 위치 좌표를 반환하는 함수
	FVector GetCellWorldLocation(TPair<int32,int32> cellPosition) const;
};
