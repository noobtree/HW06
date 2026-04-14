// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MazeGenerator.generated.h"

UCLASS()
class HW06_API AMazeGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMazeGenerator();

	// DefaultSceneRoot Component
	TObjectPtr<USceneComponent> sceneComponent;

#pragma region Maze Settings

	// DebugDraw 실행 여부
	UPROPERTY(EditAnywhere, Category = "Maze Generator")
	bool bDrawDebug = true;

	UPROPERTY(EditAnywhere, Category = "Maze Generator")
	TArray<struct FMazeCell> grid;

	// 각 Cell 사이의 거리
	UPROPERTY(EditAnywhere, Category = "Maze Generator")
	uint32 cellDistance;

	// 미로를 구성하는 Cell의 { column, row } 개수
	UPROPERTY(EditAnywhere, Category = "Maze Generator")
	FUint32Vector2 gridSize = FUint32Vector2(0, 0);

	// 시작 영역의 좌상단 위치 { minimumColumn, minimumRow }
	UPROPERTY(EditAnywhere, Category = "Maze Generator|Start Area")
	FUint32Vector2 startAreaLeftTop = FUint32Vector2(1, 1);

	// 시작 영역의 크기 { deltaColumn, deltaRow }
	UPROPERTY(EditAnywhere, Category = "Maze Generator|Start Area")
	FUint32Vector2 startAreaSizeDelta = FUint32Vector2(1, 1);

	// 시작 영역의 크기 { deltaColumn, deltaRow }
	UPROPERTY(EditAnywhere, Category = "Maze Generator")
	FUint32Vector2 exitCellPosition = FUint32Vector2(1, 1);

#pragma endregion

#pragma region Maze Wall

	UPROPERTY(EditAnywhere, Category = "Maze Generator|Wall")
	TObjectPtr<UStaticMesh> wallMeshAsset;

	UPROPERTY(EditAnywhere, Category = "Maze Generator|Wall")
	TObjectPtr<class UInstancedStaticMeshComponent> wallISMC;

#pragma endregion

	// 미로구조가 변경되는 시간 주기[sec]
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "10"), Category = "Maze Generator|Timer Setting")
	float intervalSeconds = 10;

#pragma region Trap

	// 설치되는 함정의 최대 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "100"), Category = "Maze Generator|Trap")
	int32 trapMaxCount = 5;

	// 생성된 함정들을 관리하기 위한 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze Generator|Trap")
	TArray<TObjectPtr<class ANeedleTrap>> spawnedTrapList;

	// 함정 생성 시 각 함정의 크기 비율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze Generator|Trap")
	FVector trapScale = FVector(1, 1, 1);

#pragma endregion

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 미로 난수 생성을 위한 변수
	FRandomStream randomStream;

	// 주기적으로 미로를 재생성 하기 위한 타이머
	FTimerHandle mazeTimerHandle;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 미로 구조를 생성하고, 그에 따른 벽을 생성하는 함수
	void GenerateMaze();

#pragma region Generate Maze

	// Wilson 알고리즘을 이용하여 미로 구조를 생성하는 함수
	UFUNCTION()
	void WilsonAlgorithmMaze();

	// grid를 구성하는 모든 Cell들을 초기화하는 함수
	void InitializeGrid();

	// 임의 위치의 Cell이 grid에 존재하는지 확인하는 함수
	bool IsValidCell(const TPair<int32, int32>& cellPos);

	// 임의의 Cell 좌표를 1차원 배열 상의 인덱스로 변환해주는 함수
	FORCEINLINE int32 GetCellIndex(const TPair<int32, int32>& cellPos)
	{
		return static_cast<int32>(cellPos.Value * gridSize.X + cellPos.Key);
	}

	// 최초 출발 지점을 무작위로 설정하는 함수
	TPair<int32, int32> GetInitialStartPos();

	/// <summary>
	/// 특정 지점에서 시작하는 최대 N개의 Cell로 이루어진 경로를 생성하는 함수
	/// </summary>
	/// <param name="startPosition">경로의 시작 지점</param>
	/// <param name="cellCountLimit">경로를 구성하는 최대 Cell의 개수 (-1 : 제한 없음)</param>
	TArray<TPair<int32, int32>> GenerateRandomPath(TPair<int32, int32>& startPosition, uint32 cellCountLimit = -1);

	/// <summary>
	/// 미로에 특정 경로를 편입시키는 함수
	/// </summary>
	/// <param name="excludedCellPositions">미로에 포함되지 않은 모든 Cell들의 위치를 저장하는 배열</param>
	/// <param name="path">미로에 편입하려는 경로</param>
	void IncorporatePathToMaze(TArray<TPair<int32, int32>>& excludedCellPositions, const TArray<TPair<int32, int32>>& path);

	// 임의 좌표에 위치한 Cell을 중심으로 인접한 Cell의 위치 목록을 반환하는 함수
	TArray<TPair<int32, int32>> GetAdjacentCellPositions(const TPair<int32, int32> cellPos);

	// 임의 두 좌표에 해당하는 Cell 사이의 벽을 제거하는 함수
	void RemoveWallBetweenCells(const TPair<int32, int32>& lhs, const TPair<int32, int32>& rhs);

	// 미로의 출구를 생성하는 함수
	void MakeMazeExit();

#pragma endregion

#pragma region Generate Wall

	// 기존에 생성되어있던 미로의 벽을 제거하는 함수
	void InitializeMazeWalls();

	// 현재 미로에 맞게 벽을 생성하는 함수
	void BuildMazeWalls();

#pragma endregion

#pragma region Generate Trap

	void InitializeTrap();
	void GenerateTrap();

#pragma endregion

#pragma region Debug

	// DebugDraw- 함수를 사용하여 생성된 미로를 그리는 함수
	void DebugDrawMaze() const;

#pragma endregion
};
