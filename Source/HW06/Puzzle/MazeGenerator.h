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

	UPROPERTY(EditAnywhere, Category = "Maze Generator|Mesh")
	TObjectPtr<UStaticMesh> wallMeshAsset;

	UPROPERTY(EditAnywhere, Category = "Maze Generator|Mesh")
	TObjectPtr<class UInstancedStaticMeshComponent> wallISMC;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FRandomStream randomStream;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma region Generate Maze

	// 미로를 생성하는 함수
	UFUNCTION()
	void GenerateMaze();

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

#pragma region Spawn Wall for each Cells

	// 기존에 생성되어있던 미로의 벽을 제거하는 함수
	void InitializeMazeWalls();

	// 현재 미로에 맞게 벽을 생성하는 함수
	void BuildMazeWalls();

#pragma endregion


#pragma region Debug

	// DebugDraw- 함수를 사용하여 생성된 미로를 그리는 함수
	void DebugDrawMaze() const;

#pragma endregion
};
