// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeGenerator.h"
#include "MazeCell.h"
#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
AMazeGenerator::AMazeGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 초기 컴포넌트 생성
	sceneComponent = CreateDefaultSubobject<USceneComponent>(FName("DefaultSceneRoot"));
	// Root 컴포넌트 설정
	SetRootComponent(sceneComponent);

	wallISMC = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName("ISM_Wall"));
	wallISMC->SetupAttachment(sceneComponent);
}

// Called when the game starts or when spawned
void AMazeGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	GenerateMaze();

	BuildMazeWalls();

	if (bDrawDebug == true)
	{
		DebugDrawMaze();
	}
}

// Called every frame
void AMazeGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMazeGenerator::GenerateMaze()
{
	// grid의 최소 크기 확인
	if (gridSize.X < 2 || gridSize.Y < 2)
	{
		return;
	}

	// 무작위 시드 설정
	randomStream.Initialize(FMath::Rand());

	// grid에 존재하는 모든 Cell 초기화
	InitializeGrid();

	// 미로에 포함되지 않은 모든 Cell의 위치{row, Column} 목록
	TArray<TPair<int32, int32>> excludedCellPositions;

	// 시작 영역을 제외한 Cell 검색
	for (const FMazeCell& cell : grid)
	{
		if (cell.bIsInMaze == false)
		{
			// 미로에 포함되지 않은 목록에 추가
			excludedCellPositions.Add({ cell.column, cell.row });
		}
	}

	// 초기 시작 지점 선택
	TPair<int32, int32> startCellPos = GetInitialStartPos();
	int32 initialPathSize = randomStream.RandRange(gridSize.GetMax(), gridSize.X + gridSize.Y);
	
	// 초기 경로 무작위 생성
	TArray<TPair<int32, int32>> initialPath = GenerateRandomPath(startCellPos, initialPathSize);
	
	// 미로 구성에 초기 경로 편입
	IncorporatePathToMaze(excludedCellPositions, initialPath);

	// 미로 구성에 편입되지 않은 Cell이 존재하지 않을 때까지 반복
	while (excludedCellPositions.Num() > 0)
	{
		// 미로에 편입되지 않은 무작위 Cell 선택
		int32 randIndex = randomStream.RandHelper(excludedCellPositions.Num());

		// 무작위 경로 생성
		TArray<TPair<int32, int32>> path = GenerateRandomPath(excludedCellPositions[randIndex]);

		// 미로 구성에 생성된 경로 편입
		IncorporatePathToMaze(excludedCellPositions, path);
	}
	
	// 출구 생성
	MakeMazeExit();
}

void AMazeGenerator::InitializeGrid()
{
#pragma region Initialize or Construct Cell

	// grid 초과분 Cell 삭제
	grid.SetNum(gridSize.X * gridSize.Y);

	// grid 초기화
	for (uint32 row = 0; row < gridSize.Y; ++row)
	{
		for (uint32 column = 0; column < gridSize.X; ++column)
		{
			// Cell 초기화
			FMazeCell& cell = grid[GetCellIndex({ column, row })];
			cell.row = row;
			cell.column = column;
			cell.bHasPositiveRowWall = true;
			cell.bHasNegativeRowWall = true;
			cell.bHasPositiveColumnWall = true;
			cell.bHasNegativeColumnWall = true;
			cell.bIsEditable = true;
			cell.bIsInMaze = false;
		}
	}

#pragma endregion

#pragma region Set Start Area

	// modify left top position of start area
	startAreaLeftTop.X = FMath::Clamp(startAreaLeftTop.X, 0, gridSize.X - 1);
	startAreaLeftTop.Y = FMath::Clamp(startAreaLeftTop.Y, 0, gridSize.Y - 1);

	// modify size of start area
	startAreaSizeDelta.X = ((startAreaLeftTop.X + startAreaSizeDelta.X) < gridSize.X) ? startAreaSizeDelta.X : gridSize.X - startAreaLeftTop.X;
	startAreaSizeDelta.Y = ((startAreaLeftTop.Y + startAreaSizeDelta.Y) < gridSize.Y) ? startAreaSizeDelta.Y : gridSize.Y - startAreaLeftTop.Y;

	for (uint32 deltaRow = 0; deltaRow < startAreaSizeDelta.Y; ++deltaRow)
	{
		for (uint32 deltaColumn = 0; deltaColumn < startAreaSizeDelta.X; ++deltaColumn)
		{
			// Cell 참조
			FMazeCell& cell = grid[GetCellIndex({ startAreaLeftTop.X + deltaColumn, startAreaLeftTop.Y + deltaRow })];

			// 최상단 Cell을 제외하고 상단 벽 제거
			if (deltaRow > 0)
			{
				cell.bHasNegativeRowWall = false;
			}
			// 최좌측 Cell을 제외하고 좌측 벽 제거
			if (deltaColumn > 0)
			{
				cell.bHasNegativeColumnWall = false;
			}
			// 최하단 Cell을 제외하고 하단 벽 제거
			if (deltaRow + 1 < startAreaSizeDelta.Y)
			{
				cell.bHasPositiveRowWall = false;
			}
			// 최우측 Cell을 제외하고 우측 벽 제거
			if (deltaColumn + 1 < startAreaSizeDelta.X)
			{
				cell.bHasPositiveColumnWall = false;
			}
			
			// 다른 Cell에서 벽을 수정하지 못하도록 설정
			cell.bIsEditable = false;
			// 미로에 포함된 것으로 설정
			cell.bIsInMaze = true;
		}
	}

#pragma endregion
}

bool AMazeGenerator::IsValidCell(const TPair<int32, int32>& cellPos)
{
	if (cellPos.Key < 0 || cellPos.Key >= static_cast<int32>(gridSize.X))
	{
		return false;
	}
	if (cellPos.Value < 0 || cellPos.Value >= static_cast<int32>(gridSize.Y))
	{
		return false;
	}
	// 인덱스 계산
	int32 index = GetCellIndex(cellPos);

	// 인덱스가 유효한지 검사
	return grid.IsValidIndex(index);
}

TPair<int32, int32> AMazeGenerator::GetInitialStartPos()
{
	// 무작위 방향
	int32 randomDirection = randomStream.RandRange(0, 3);

	// 시작 영역의 좌상단 기준 계산
	TPair<int32, int32> cellPosition = { startAreaLeftTop.X, startAreaLeftTop.Y };

	switch (randomDirection)
	{
	case 0:		// Left (Negative Column)
	{
		// row 증가
		cellPosition.Value += startAreaSizeDelta.Y / 2;
	}
		break;
	case 1:		// Top (Negative Row)
	{
		// column 증가
		cellPosition.Key += startAreaSizeDelta.X / 2;
	}
		break;
	case 2:		// Right (Positive Column)
	{
		// column 증가
		cellPosition.Key += startAreaSizeDelta.X - 1;
		// row 증가
		cellPosition.Value += startAreaSizeDelta.Y / 2;
	}
		break;
	case 3:		// Bottom (Positive Row)
	{
		// column 증가
		cellPosition.Key += startAreaSizeDelta.X / 2;
		// row 증가
		cellPosition.Value += startAreaSizeDelta.Y - 1;
	}
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Random Value for selecting start direction Error"));
		break;
	}
	return cellPosition;
}

TArray<TPair<int32, int32>> AMazeGenerator::GenerateRandomPath(TPair<int32, int32>& startPosition, uint32 cellCountLimit)
{
	// 이동 경로를 저장하는 배열
	TArray<TPair<int32, int32>> path;

	// 시작 지점이 유효한지 확인
	if (IsValidCell(startPosition) == false)
	{
		return path;
	}

	// 현재 위치를 대표하는 변수
	TPair<int32, int32> currentPos = startPosition;
	// 경로에 현재 위치 추가
	path.Add(currentPos);

	// 이동하며 path에 좌표 저장
	do
	{
		// 인접한 Cell 좌표 목록 얻기
		TArray<TPair<int32, int32>> adjacentCellPositions = GetAdjacentCellPositions(currentPos);

		// 인접한 Cell 좌표 목록에서 무작위로 유효한 Cell 좌표 선택
		while (adjacentCellPositions.Num() > 0)
		{
			// 인접한 Cell 좌표 목록에서 무작위 선택
			int32 randIndex = randomStream.RandRange(0, adjacentCellPositions.Num() - 1);
			TPair<int32, int32> adjacentCellPos = adjacentCellPositions[randIndex];

			// 선택된 Cell 좌표 유효성 검사
			if (IsValidCell(adjacentCellPos) == true)
			{
				// 선택된 좌표의 Cell으로 연결할 수 있는지 확인
				if (grid[GetCellIndex(adjacentCellPos)].bIsEditable == true)
				{
					// 이동
					currentPos = adjacentCellPos;
					break;
				}
			}

			// 인접한 Cell 좌표 목록에서 선택된 좌표 삭제
			adjacentCellPositions.RemoveAt(randIndex);
		}

		// 이동할 수 있는 인접 Cell 좌표가 존재하지 않으면 이동 중단
		if (adjacentCellPositions.Num() == 0)
		{
			break;
		}

		// 기존 경로에 존재하는 좌표로 이동하였는지 확인
		int32 pathIndex = path.IndexOfByPredicate([currentPos](TPair<int32, int32> pos)->bool
			{
				return (pos.Key == currentPos.Key) && (pos.Value == currentPos.Value);
			});

		if (pathIndex == INDEX_NONE)
		{
			// 경로에 현재 위치 추가
			path.Add(currentPos);

			// 경로 생성 Cell 제한 개수 확인
			if (path.Num() == cellCountLimit)
			{
				// 이동 중단
				break;
			}
		}
		else
		{
			// 해당 좌표 이후에 이동한 경로 삭제
			path.SetNum(pathIndex + 1);
		}
	}
	// 현재 좌표가 유효하지 않거나, 미로에 편입될 때까지 반복
	while (IsValidCell(currentPos) == true && grid[GetCellIndex(currentPos)].bIsInMaze == false);

	return path;
}

void AMazeGenerator::IncorporatePathToMaze(TArray<TPair<int32, int32>>& excludedCellPositions, const TArray<TPair<int32, int32>>& path)
{
	// 이동 경로를 따라 벽 제거 및 미로에 편입
	for (int i = 0; i + 1 < path.Num(); ++i)
	{
		TPair<int32, int32> lhs = path[i];
		TPair<int32, int32> rhs = path[i + 1];

		// 벽 제거
		RemoveWallBetweenCells(path[i], path[i + 1]);

		// 미로 편입
		grid[GetCellIndex(lhs)].bIsInMaze = true;
		grid[GetCellIndex(rhs)].bIsInMaze = true;

		// 미로에 포함되지 않은 목록에서 삭제
		excludedCellPositions.Remove(lhs);
		excludedCellPositions.Remove(rhs);
	}

	// 다른 Cell로 이동하지 않은 경우 대비
	if (path.Num() == 1)
	{
		// 미로에 편입
		grid[GetCellIndex(path[0])].bIsInMaze = true;

		// 미로에 포함되지 않은 목록에서 삭제
		excludedCellPositions.Remove(path[0]);
	}
}

TArray<TPair<int32, int32>> AMazeGenerator::GetAdjacentCellPositions(const TPair<int32, int32> cellPos)
{
	TArray<TPair<int32, int32>> adjacentCellPositions;
	adjacentCellPositions.Add({ cellPos.Key - 1, cellPos.Value });		// Left (Negative Column)
	adjacentCellPositions.Add({ cellPos.Key, cellPos.Value - 1 });		// Top (Negative Row)
	adjacentCellPositions.Add({ cellPos.Key + 1, cellPos.Value });		// Right (Positive Column)
	adjacentCellPositions.Add({ cellPos.Key, cellPos.Value + 1 });		// Bottom (Positive Row)

	return adjacentCellPositions;
}

void AMazeGenerator::RemoveWallBetweenCells(const TPair<int32, int32>& lhs, const TPair<int32, int32>& rhs)
{
	if (IsValidCell(lhs) == false || IsValidCell(rhs) == false)
	{
		return;
	}

	int32 lhsIndex = GetCellIndex(lhs);
	int32 rhsIndex = GetCellIndex(rhs);

	// 동일한 Row에 위치한 경우
	if (lhs.Value == rhs.Value)
	{
		// lhs 기준 우측에 rhs가 존재
		if (lhs.Key < rhs.Key)
		{
			// lhs의 우측 벽 삭제
			grid[lhsIndex].bHasPositiveColumnWall = false;
			// lhs의 좌측 벽 삭제
			grid[rhsIndex].bHasNegativeColumnWall = false;
			return;
		}
		// lhs 기준 좌측에 rhs가 존재
		if (lhs.Key > rhs.Key)
		{
			// lhs의 좌측 벽 삭제
			grid[lhsIndex].bHasNegativeColumnWall = false;
			// lhs의 우측 벽 삭제
			grid[rhsIndex].bHasPositiveColumnWall = false;
			return;
		}
	}

	// lhs 기준 rhs가 하단에 존재
	if (lhs.Value < rhs.Value)
	{
		// lhs의 하단 벽 삭제
		grid[lhsIndex].bHasPositiveRowWall = false;
		// lhs의 상단 벽 삭제
		grid[rhsIndex].bHasNegativeRowWall = false;
		return;
	}

	// lhs 기준 rhs가 하단에 존재
	if (lhs.Value > rhs.Value)
	{
		// lhs의 상단 벽 삭제
		grid[lhsIndex].bHasNegativeRowWall = false;
		// lhs의 하단 벽 삭제
		grid[rhsIndex].bHasPositiveRowWall = false;
	}
	return;
}

void AMazeGenerator::MakeMazeExit()
{
	// modify exit position
	exitCellPosition.X = FMath::Clamp(exitCellPosition.X, 0, gridSize.X - 1);
	exitCellPosition.Y = FMath::Clamp(exitCellPosition.Y, 0, gridSize.Y - 1);

	// 출구 생성
	FMazeCell& exitCell = grid[GetCellIndex({ exitCellPosition.X, exitCellPosition.Y })];
	if (exitCell.column == 0)	// 좌측 방향 출구 생성
	{
		exitCell.bHasNegativeColumnWall = false;
	}
	else if (exitCell.row == 0)	// 상단 방향 출구 생성
	{
		exitCell.bHasNegativeRowWall = false;
	}
	else if (exitCell.column == gridSize.X - 1)	// 우측 방향 출구 생성
	{
		exitCell.bHasPositiveColumnWall = false;
	}
	else if (exitCell.row == gridSize.Y - 1)	// 하단 방향 출구 생성
	{
		exitCell.bHasPositiveRowWall = false;
	}
}

void AMazeGenerator::InitializeMazeWalls()
{
	if (wallISMC != nullptr)
	{
		wallISMC->ClearInstances();
	}
}

void AMazeGenerator::BuildMazeWalls()
{
	// 필요한 프로퍼티 nullptr 검사
	if (wallISMC == nullptr || wallMeshAsset == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Null Reference Access Execption"));
		return;
	}

	// 기존 벽 초기화
	InitializeMazeWalls();
	
	// StaticMesh 에셋 설정
	wallISMC->SetStaticMesh(wallMeshAsset);

	// 미로 생성 기준 위치
	const FVector MAZELEFTTOP = GetActorLocation();
	
	// 사용하려는 벽 Mesh의 크기
	const FVector MESHSIZE = wallMeshAsset->GetBoundingBox().GetSize();
	// 적용해야 할 scale
	const FVector WALLSCALE = FVector(static_cast<float>(cellDistance) / MESHSIZE.X, 0.5f, 1);

	// 각 Cell의 좌측, 상단 벽 생성 (마지막 행, 열의 경우 우측, 하단 벽 추가로 생성)
	for (const FMazeCell& cell : grid)
	{
		FVector cellLeftTop = MAZELEFTTOP + cellDistance * FVector(cell.row, cell.column, 0);
		FVector cellRightBottom = cellLeftTop + FVector(cellDistance, cellDistance, 0);

		// 좌측 벽 생성
		if (cell.bHasNegativeColumnWall == true)
		{
			// Transformm 계산
			FVector location = cellLeftTop + FVector(0.5f * cellDistance, 0, 0);
			FRotator rotation = FRotator::MakeFromEuler(FVector(0, 0, 0));
			FTransform transform = FTransform(rotation, location, WALLSCALE);

			// 로컬 좌표계로 Mesh Instance 추가
			wallISMC->AddInstance(transform, true);
		}
		// 상단 벽 생성
		if (cell.bHasNegativeRowWall == true)
		{
			// Transformm 계산
			FVector location = cellLeftTop + FVector(0, 0.5f * cellDistance, 0);
			FRotator rotation = FRotator::MakeFromEuler(FVector(0, 0, 90));
			FTransform transform = FTransform(rotation, location, WALLSCALE);

			// 로컬 좌표계로 Mesh Instance 추가
			wallISMC->AddInstance(transform, true);
		}

		// 마지막 행, 열의 경우 우측, 하단 벽 생성
		if (cell.row + 1 == gridSize.Y || cell.column + 1 == gridSize.X)
		{
			// 우측 벽 생성
			if (cell.bHasPositiveColumnWall == true)
			{
				// Transformm 계산
				FVector location = cellRightBottom - FVector(0.5f * cellDistance, 0, 0);
				FRotator rotation = FRotator::MakeFromEuler(FVector(0, 0, 0));
				FTransform transform = FTransform(rotation, location, WALLSCALE);

				// 로컬 좌표계로 Mesh Instance 추가
				wallISMC->AddInstance(transform, true);
			}
			// 하단 벽 생성
			if (cell.bHasPositiveRowWall == true)
			{
				// Transformm 계산
				FVector location = cellRightBottom - FVector(0, 0.5f * cellDistance, 0);
				FRotator rotation = FRotator::MakeFromEuler(FVector(0, 0, 90));
				FTransform transform = FTransform(rotation, location, WALLSCALE);

				// 로컬 좌표계로 Mesh Instance 추가
				wallISMC->AddInstance(transform, true);
			}
		}
	}
}

void AMazeGenerator::DebugDrawMaze() const
{
	UWorld* world = GetWorld();
	if (world == nullptr)
	{
		return;
	}

	// 원의 중심 좌표
	const FVector MAZELEFTTOP = GetActorLocation() + 100 * FVector::DownVector;
	// 선 색상
	const FColor LINECOLOR = FColor::Red;
	// 영구 지속
	const bool BPERSISTENT = true;
	// 지속 시간
	const float LIFETIME = 0.0f;
	// 렌더링 우선순위
	const uint8 DEPTH = 0;
	// 선 굵기
	const float THICKNESS = 10.0f;

	// grid에 존재하는 전체 Cell 그리기
	for (const FMazeCell& cell : grid)
	{
		FVector cellLeftTop = MAZELEFTTOP + cellDistance * FVector(cell.row, cell.column, 0);
		FVector cellRightBottom = cellLeftTop + FVector(cellDistance, cellDistance, 0);

		// 좌측 벽 그리기
		if (cell.bHasNegativeColumnWall == true)
		{
			DrawDebugLine(world, cellLeftTop, cellLeftTop + FVector(cellDistance, 0, 0), LINECOLOR, BPERSISTENT, LIFETIME, DEPTH, THICKNESS);
		}
		// 상단 벽 그리기
		if (cell.bHasNegativeRowWall == true)
		{
			DrawDebugLine(world, cellLeftTop, cellLeftTop + FVector(0, cellDistance, 0), LINECOLOR, BPERSISTENT, LIFETIME, DEPTH, THICKNESS);
		}
		// 우측 벽 그리기
		if (cell.bHasPositiveColumnWall == true)
		{
			DrawDebugLine(world, cellRightBottom, cellRightBottom - FVector(cellDistance, 0, 0), LINECOLOR, BPERSISTENT, LIFETIME, DEPTH, THICKNESS);
		}
		// 하단 벽 그리기
		if (cell.bHasPositiveRowWall == true)
		{
			DrawDebugLine(world, cellRightBottom, cellRightBottom - FVector(0, cellDistance, 0), LINECOLOR, BPERSISTENT, LIFETIME, DEPTH, THICKNESS);
		}
	}
}
