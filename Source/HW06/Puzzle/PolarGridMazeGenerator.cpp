// Fill out your copyright notice in the Description page of Project Settings.


#include "PolarGridMazeGenerator.h"

// Sets default values
APolarGridMazeGenerator::APolarGridMazeGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 초기 컴포넌트 생성
	sceneComponent = CreateDefaultSubobject<USceneComponent>(FName("DefaultSceneRoot"));

	// Root 컴포넌트 설정
	SetRootComponent(sceneComponent);
}

// Called when the game starts or when spawned
void APolarGridMazeGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	// 미로 생성
	GenerateMaze();
}

// Called every frame
void APolarGridMazeGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APolarGridMazeGenerator::GenerateMaze()
{
	// 난수 생성 시드 초기화
	randStream.Initialize(randomSeed);

	// Cell 목록 초기화
	InitializeCells();
	
	// DFS 방식으로 길을 찾으며 미로 생성
	//RecursiveBacktracking();
	//Wilson();

	// 디버그 모드인 경우
	if (bIsDebugMode)
	{
		DrawDebugMaze();
	}
}

void APolarGridMazeGenerator::InitializeCells()
{
	polarGrid.Empty();

	// 중심 원 생성
	TArray<FPolarCell> centerRing;
	// 중심원에 Cell 1개 추가
	centerRing.Add(FPolarCell());

	// 맵에 Ring 추가
	polarGrid.Add(centerRing);

	// 외곽 원 생성 및 추가
	for (int32 r = 1; r <= ringCount; ++r)
	{
		// 외곽 원 생성
		TArray<FPolarCell> ring;

		// 원 분할 개수 계산
		int32 partitionCount = GetCellCountInRing(r);

		for (int32 i = 0; i < partitionCount; ++i)
		{
			// Cell 생성
			FPolarCell cell;
			cell.ringIndex = r;
			cell.cellIndex = i;

			// Ring에 Cell 추가
			ring.Add(cell);
		}

		// 맵에 Ring 추가
		polarGrid.Add(ring);
	}
}

int32 APolarGridMazeGenerator::GetCellCountInRing(int32 ringIndex) const
{
	return ringIndex > 0 ? baseCellCount * FMath::Pow(2.0, ringIndex -1) : 1;
}

void APolarGridMazeGenerator::RecursiveBacktracking()
{
	// 전체 Cell 방문 기록 초기화
	for (TArray<FPolarCell>& ring : polarGrid)
	{
		for (FPolarCell& cell : ring)
		{
			cell.bIsVisited = false;
		}
	}
	
	// 이동 경로 저장 스택
	TArray<TPair<int32, int32>> pathStack;

	// 시작 지점 설정
	pathStack.Add({ 0, 0 });
	// 시작 지점 방문 기록
	polarGrid[0][0].bIsVisited = true;

	// DFS 수행
	while (pathStack.Num() > 0)
	{
		// 최근 방문 Cell 확인
		TPair<int32, int32> currentPos = pathStack.Last();

		// 인접 Cell 목록 얻기
		TArray<TPair<int32, int32>> adjacentCellPositions = GetAdjacentCells(currentPos);

		// 아직 방문하지 않은 인접 Cell만 얻기
		TArray<TPair<int32, int32>> unvisitedAdjacentCellPositions;
		for (const TPair<int32, int32>& adjacentPos : adjacentCellPositions)
		{
			// 인접 Cell이 polarGrid에 존재하는지 확인
			if (IsValidCell(adjacentPos) == true)
			{
				// 해당 위치의 Cell을 방문하지 않았는지 확인
				if (polarGrid[adjacentPos.Key][adjacentPos.Value].bIsVisited == false)
				{
					unvisitedAdjacentCellPositions.Add(adjacentPos);
				}
			}
		}

		// 방문하지 않은 인접 Cell이 존재하는지 확인
		if (unvisitedAdjacentCellPositions.Num() > 0)
		{
			// 방문하지 않은 인접 Cell 목록 중에서 무작위 선택
			int32 randIndex = randStream.RandRange(0, unvisitedAdjacentCellPositions.Num() - 1);
			TPair<int32, int32> nextPos = unvisitedAdjacentCellPositions[randIndex];

			// 현재 위치의 Cell과 다음 위치의 Cell 사이에 위치한 벽 제거
			RemoveWallBetweenCells(currentPos, nextPos);

			// 이동할 위치의 Cell에 방문 기록
			polarGrid[nextPos.Key][nextPos.Value].bIsVisited = true;

			// 이동 경로 저장
			pathStack.Add(nextPos);
		}
		// 방문하지 않은 인접 Cell이 존재하지 않으면 이전 위치로 되돌아가기
		else
		{
			pathStack.Pop();
		}
	}

	// 출구 하나 생성 (가장 바깥 링의 랜덤 셀 바깥 벽 제거)
	if (polarGrid.Num() > 1)
	{
		int32 OuterRingIndex = polarGrid.Num() - 1;
		int32 ExitSector = randStream.RandRange(0, polarGrid[OuterRingIndex].Num() - 1);
		polarGrid[OuterRingIndex][ExitSector].bHasOuterWall = false;
	}
}

void APolarGridMazeGenerator::Wilson()
{
	// 모든 Cell 좌표들의 목록
	TArray<TPair<int32, int32>> allCellPositions;

	// 전체 Cell 초기화
	for (TArray<FPolarCell>& ring : polarGrid)
	{
		for (FPolarCell& cell : ring)
		{
			cell.bIsVisited = false;
			cell.bIsInMazeTree = false;

			// Cell 목록에 추가
			allCellPositions.Add({ cell.ringIndex, cell.cellIndex });
		}
	}

	// 미로를 구성할 Cell이 존재하지 않으면 함수 종료
	if (allCellPositions.Num() == 0)
	{
		return;
	}

	// 중심 원의 Cell을 미로를 구성하는 Tree에 포함된 것으로 설정
	if (IsValidCell({ 0,0 }) == true)
	{
		polarGrid[0][0].bIsInMazeTree = true;
	}

	// 모든 Cell이 미로를 구성하는 Tree에 포함될 때까지 반복
	while (true)
	{
		// 미로를 구성하는 Tree에 포함되지 않은 Cell들의 좌표를 저장하는 목록
		TArray<TPair<int32, int32>> excludedCellPositions;
		// Tree에 포함되지 않은 Cell 좌표 목록 추출
		for (const TPair<int32, int32>& cellPos : allCellPositions)
		{
			if (IsValidCell(cellPos) == true && polarGrid[cellPos.Key][cellPos.Value].bIsInMazeTree == false)
			{
				excludedCellPositions.Add(cellPos);
			}
		}

		// 전체 Cell이 Tree에 포함되어있으면 중단
		if (excludedCellPositions.Num() == 0)
		{
			break;
		}

		// Tree에 포함되지 않은 Cell 목록에서 무작위로 선택하여 경로 생성 시작점 설정
		int32 randIndex = randStream.RandRange(0, excludedCellPositions.Num() - 1);
		TPair<int32, int32> startPos = excludedCellPositions[randIndex];

		// 현재 루프 제거 무작위 이동 경로를 저장할 배열
		TArray<TPair<int32, int32>> path;
		// path 배열에서 특정 Cell의 인덱스 정보를 저장하는 맵
		TMap<uint64, int32> pathIndexMap;

		// Cell 좌표에 대한 고유값을 계산하는 함수
		auto GenerateKey = [](const TPair<int32, int32> cellPos) -> uint64
			{
				return (static_cast<uint64>(static_cast<uint32>(cellPos.Key)) << 32) | static_cast<uint32>(cellPos.Value);
			};

		// 이동 경로에 시작점 등록
		pathIndexMap.Add(GenerateKey(startPos), path.Num());
		path.Add(startPos);

		// 현재 위치를 시작 지점으로 설정
		TPair<int32, int32> currentPos = startPos;

		// 현재 위치의 Cell이 미로를 구성하는 Tree에 포함될 때까지 반복
		while (IsValidCell(currentPos) == true && polarGrid[currentPos.Key][currentPos.Value].bIsInMazeTree == false)
		{
			// 현재 위치의 Cell에 인접한 Cell들의 좌표 얻기
			TArray<TPair<int32, int32>> adjacentCells = GetAdjacentCells(currentPos);

			// 인접한 Cell이 존재하지 않으면 이동 중단
			if (adjacentCells.Num() == 0)
			{
				break;
			}

			// 인접한 Cell 목록 중에서 무작위 선택
			int32 randAdjacentIndex = randStream.RandRange(0, adjacentCells.Num() - 1);
			TPair<int32, int32> adjacentCellPos = adjacentCells[randAdjacentIndex];

			// 선택된 Cell의 고유 Key 값 계산
			uint64 adjacentCellKey = GenerateKey(adjacentCellPos);

			// 선택된 Cell이 현재 이동 경로에 포함되어있는지 확인
			if (pathIndexMap.Contains(adjacentCellKey) == true)
			{// ! 루프 발생 !
				// path에서 선택된 Cell의 인덱스 검색
				int32 loopStartCellIndex = pathIndexMap[adjacentCellKey];

				// 선택된 Cell 이후에 지나온 모든 Cell을 이동 경로를 삭제
				for (int32 pathIndex = path.Num() - 1; pathIndex > loopStartCellIndex; --pathIndex)
				{
					// 마지막에 지나온 Cell을 이동 경로에서 제거
					TPair<int32, int32> removedCellPos = path.Pop();

					// 제거된 Cell의 고유 Key를 통해 인덱스 정보 삭제
					uint64 removedCellKey = GenerateKey(removedCellPos);
					pathIndexMap.Remove(removedCellKey);
				}
			}
			else
			{
				// 선택된 Cell을 이동 경로에 추가
				pathIndexMap.Add(adjacentCellKey,path.Num());
				path.Add(adjacentCellPos);
			}

			// 선택된 Cell으로 이동
			currentPos = adjacentCellPos;
		}

		// 이동 경로에 포함된 Cell의 정보 수정
		for (int32 i = 0; i + 1 < path.Num(); ++i)
		{
			// Cell의 벽을 제거
			RemoveWallBetweenCells(path[i], path[i + 1]);

			// Cell을 미로를 구성하는 Tree에 포함된 것으로 설정
			polarGrid[path[i].Key][path[i].Value].bIsInMazeTree = true;
			polarGrid[path[i + 1].Key][path[i + 1].Value].bIsInMazeTree = true;
		}

		// 다른 Cell으로 이동하지 않은 경우
		// 경로 길이가 1이고 이미 트리에 닿아 종료된 경우 대비
		if (path.Num() == 1)
		{
			// 경로 시작 지점 Cell을 미로를 구성하는 Tree에 포함된 것으로 설정
			polarGrid[path[0].Key][path[0].Value].bIsInMazeTree = true;
		}
	}

	// 미로의 출구 생성
	if (polarGrid.Num() > 1)
	{
		int32 lastRingIndex = polarGrid.Num() - 1;
		int32 randCellIndex = randStream.RandRange(0, polarGrid[lastRingIndex].Num() - 1);
		
		// 출구가 존재하는 Cell의 바깥쪽 벽 제거
		polarGrid[lastRingIndex][randCellIndex].bHasOuterWall = false;
	}
	return;
}

TArray<TPair<int32, int32>> APolarGridMazeGenerator::GetAdjacentCells(const TPair<int32, int32>& cellPosition) const
{
	// 인접 Cell의 좌표를 저장할 목록
	TArray<TPair<int32, int32>> adjacentCellPositions;

	// 기준 Cell의 좌표가 유효한지 확인
	if (IsValidCell(cellPosition) == false)
	{
		return adjacentCellPositions;
	}

	// 동일한 Ring에 위치한 Cell 개수 얻기
	const int32 cellCountInSameRing = polarGrid[cellPosition.Key].Num();

	// 동일한 Ring에 위치한 Cell 찾기
	if (cellPosition.Key > 0 && cellCountInSameRing > 1)
	{
		// 시계방향에 인접한 Cell의 cellIndex
		int32 clockwiseIndex = (cellPosition.Value + 1) % cellCountInSameRing;
		adjacentCellPositions.AddUnique({ cellPosition.Key, clockwiseIndex });

		// 반시계방향에 인접한 Cell의 cellIndex
		int32 counterclockwiseIndex = (cellPosition.Value + cellCountInSameRing - 1) % cellCountInSameRing;
		adjacentCellPositions.AddUnique({ cellPosition.Key, counterclockwiseIndex });
	}

	// 안쪽에 방향 Ring에 위치한 Cell 찾기
	if (cellPosition.Key > 0)
	{
		// 안쪽에 방향 Ring에 위치한 Cell 개수 얻기
		int32 cellCountInInnerRing = polarGrid[cellPosition.Key - 1].Num();

		if (cellCountInInnerRing == 1)
		{
			adjacentCellPositions.AddUnique({ cellPosition.Key - 1, 0 });
		}
		else
		{
			// 기준 Cell의 cellIndex를 안쪽 Ring의 cellIndex로 변환
			float cellCountRate = static_cast<float>(cellCountInInnerRing) / cellCountInSameRing;
			//float innerCellIndexRate = (static_cast<float>(cellPosition.Value) / cellCountInSameRing) * cellCountInInnerRing;
			int32 from = FMath::FloorToInt32(cellPosition.Value * cellCountRate);
			int32 upto = FMath::CeilToInt32(cellPosition.Value * cellCountRate);

			// 안쪽 Ring에서 인접한 Cell 추가
			for (int32 i = from; i <= upto; ++i)
			{
				// 인덱스 범위 조정
				int32 adjacentCellIndex = (i + cellCountInInnerRing) % cellCountInInnerRing;
				adjacentCellPositions.AddUnique({ cellPosition.Key - 1, adjacentCellIndex });
			}
		}
	}

	// 바깥쪽 뱡향 Ring에 위치한 Cell 찾기
	if (cellPosition.Key < polarGrid.Num() - 1)
	{
		// 바깥쪽 방향 Ring에 위치한 Cell 개수 얻기
		int32 cellCountInOuterRing = polarGrid[cellPosition.Key + 1].Num();

		// 기준 Cell이 중심원인 경우
		if (cellCountInSameRing == 1)
		{
			// 바깥쪽 Ring에 위치한 전체 Cell과 인접
			for (int32 i = 0; i < cellCountInOuterRing; ++i)
			{
				adjacentCellPositions.AddUnique({ cellPosition.Key + 1, i });
			}
		}
		else
		{
			// 기준 Cell의 cellIndex를 바깥쪽 Ring의 cellIndex로 변환
			float cellCountRate = static_cast<float>(cellCountInOuterRing) / cellCountInSameRing;
			int32 from = FMath::FloorToInt32((cellPosition.Value - 1) * cellCountRate) + 1;
			int32 upto = FMath::CeilToInt32((cellPosition.Value + 1) * cellCountRate) - 1;

			// 바깥쪽 Ring에서 인접한 Cell 추가
			for (int32 i = from; i <= upto; ++i)
			{
				// 인덱스 범위 제한
				int32 adjacentCellIndex = (i + cellCountInOuterRing) % cellCountInOuterRing;
				adjacentCellPositions.AddUnique({ cellPosition.Key + 1, adjacentCellIndex });
			}
		}
	}

	return adjacentCellPositions;
}

bool APolarGridMazeGenerator::IsValidCell(const TPair<int32,int32>& cellPosition) const
{
	if (polarGrid.IsValidIndex(cellPosition.Key) == true)
	{
		if (polarGrid[cellPosition.Key].IsValidIndex(cellPosition.Value) == true)
		{
			return true;
		}
	}
	return false;
}

void APolarGridMazeGenerator::RemoveWallBetweenCells(const TPair<int32, int32>& lhs, const TPair<int32, int32>& rhs)
{
	if (IsValidCell(lhs) == false || IsValidCell(rhs) == false)
	{
		return;
	}

	// 동일한 Ring에 위치한 Cell의 경우
	if (lhs.Key == rhs.Key)
	{
		// Ring에 위치한 Cell 개수 얻기
		const int32 cellCountInSameRing = polarGrid[lhs.Key].Num();

		// lhs 기준 시계방향 확인(rhs 기준 반시계방향)
		int32 lClockwiseIndex = (lhs.Value + 1) % cellCountInSameRing;

		if (rhs.Value == lClockwiseIndex)
		{
			// lhs 좌표에 위치한 Cell의 시계방향 벽 제거
			polarGrid[lhs.Key][lhs.Value].bHasClockwiseWall = false;
			// rhs 좌표에 위치한 Cell의 반시계방향 벽 제거
			polarGrid[rhs.Key][rhs.Value].bHasCounterclockwiseWall = false;
			return;
		}

		// rhs 기준 시계방향 확인(lhs 기준 반시계방향)
		int32 rClockwiseIndex = (rhs.Value + 1) % cellCountInSameRing;

		if (lhs.Value == rClockwiseIndex)
		{
			// rhs 좌표에 위치한 Cell의 시계방향 벽 제거
			polarGrid[rhs.Key][rhs.Value].bHasClockwiseWall = false;
			// lhs 좌표에 위치한 Cell의 반시계방향 벽 제거
			polarGrid[lhs.Key][lhs.Value].bHasCounterclockwiseWall = false;
			return;
		}
	}

	// lhs 좌표가 rhs 좌표보다 안쪽 Ring에 위치한 경우
	if (lhs.Key < rhs.Key)
	{
		// lhs 좌표에 위치한 Cell의 바깥쪽 벽 제거
		polarGrid[lhs.Key][lhs.Value].bHasOuterWall = false;
		// rhs 좌표에 위치한 Cell의 안쪽 벽 제거
		polarGrid[rhs.Key][rhs.Value].bHasInnerWall = false;
		return;
	}

	// lhs 좌표가 rhs 좌표보다 바깥쪽 Ring에 위치한 경우
	if (lhs.Key > rhs.Key)
	{
		// lhs 좌표에 위치한 Cell의 안쪽 벽 제거
		polarGrid[lhs.Key][lhs.Value].bHasInnerWall = false;
		// rhs 좌표에 위치한 Cell의 바깥쪽 벽 제거
		polarGrid[rhs.Key][rhs.Value].bHasOuterWall = false;
		return;
	}

	return;
}

void APolarGridMazeGenerator::DrawDebugMaze() const
{
	UWorld* world = GetWorld();
	if (world == nullptr)
	{
		return;
	}

	// 원의 중심 좌표
	const FVector centerLocation = GetActorLocation();
	// 원의 곡률
	const int32 circleSegments = 128;
	// 선 색상
	const FColor debugColor = FColor::Red;
	// 영구 지속
	const bool bIsDrawPersistent = true;
	// 지속 시간
	const float lifeTime = 0.0f;
	// 렌더링 우선순위
	const uint8 depthPriority = 0;
	// 선 굵기
	const float thickness = 2.0f;
	// 원과 수평을 이루는 앞쪽 방향
	const FVector yAxisOfCircle = FVector(1, 0, 0);
	// 원과 수평을 이루는 우측 방향
	const FVector zAxisOfCircle = FVector(0, 1, 0);

	// 중심 원 그리기
	//DrawDebugCircle(world, centerLocation, centerRadius, circleSegments, FColor::White, bIsDrawPersistent, lifeTime, depthPriority, thickness, yAxisOfCircle, zAxisOfCircle, false);

	// 외곽 Ring 그리기
	for (int32 r = 1; r < polarGrid.Num(); ++r)
	{
		// Ring의 안쪽 반지름
		const float innerRadius = centerRadius + ((r - 1) * ringDistance);
		// Ring의 바깥쪽 반지름
		const float outerRadius = centerRadius + (r * ringDistance);

		// Ring에 존재하는 Cell의 개수
		const int32 cellCountInRing = polarGrid[r].Num();
		// 개별 Cell의 중심각
		const float eachCellRad = (2 * PI) / cellCountInRing;

		// 각 Cell 그리기
		for (float i = 0; i < cellCountInRing; ++i)
		{
			// Cell 참조
			const FPolarCell& Cell = polarGrid[r][i];

			// Ring의 중심에서 Cell을 가리키는 방향
			FVector cellDirection = FVector(FMath::Cos((i - 0.5f) * eachCellRad), FMath::Sin((i - 0.5f) * eachCellRad), 0);

			// Cell의 안쪽 벽 그리기
			if (Cell.bHasInnerWall == true)
			{
				// center에서 radius만큼 direction 방향으로 떨어진 지점을 기준으로, (시계방향 & 반시계방향)양쪽으로 angleWidth[rad]만큼 호를 그린다.
				DrawDebugCircleArc(world, centerLocation, innerRadius, cellDirection, eachCellRad * 0.5f, circleSegments, debugColor, bIsDrawPersistent, lifeTime, depthPriority, thickness);
			}

			// Cell의 바깥쪽 벽 그리기
			if (Cell.bHasOuterWall == true)
			{
				DrawDebugCircleArc(world, centerLocation, outerRadius, cellDirection, eachCellRad * 0.5f, circleSegments, debugColor, bIsDrawPersistent, lifeTime, depthPriority, thickness);
			}

			// Cell의 반시계방향 벽 그리기
			if (Cell.bHasCounterclockwiseWall == true)
			{
				// 반시계방향(시작점)을 가리키는 방향 벡터
				FVector counterclockwiseDirection = FVector(FMath::Cos(i * eachCellRad), FMath::Sin(i * eachCellRad), 0);

				FVector startLocation = centerLocation + (innerRadius * counterclockwiseDirection);
				FVector endLocation = centerLocation + (outerRadius * counterclockwiseDirection);
				DrawDebugLine(world, startLocation, endLocation, debugColor, bIsDrawPersistent, lifeTime, depthPriority, thickness);
			}

			// Cell의 시계방향 벽 그리기
			if (Cell.bHasClockwiseWall == true)
			{
				// 시계방향(끝점)을 가리키는 방향 벡터
				FVector clockwiseDirection = FVector(FMath::Cos((i + 1) * eachCellRad), FMath::Sin((i + 1) * eachCellRad), 0);

				FVector startLocation = centerLocation + (innerRadius * clockwiseDirection);
				FVector endLocation = centerLocation + (outerRadius * clockwiseDirection);
				DrawDebugLine(world, startLocation, endLocation, debugColor, bIsDrawPersistent, lifeTime, depthPriority, thickness);
			}
		}
	}
}

FVector APolarGridMazeGenerator::GetCellWorldLocation(TPair<int32, int32> cellPosition) const
{
	// Ring의 중심 좌표 얻기
	const FVector center = GetActorLocation();

	// 유효하지 않은 Cell 또는 ring 인덱스가 0인 경우 중심 좌표 반환
	if (IsValidCell(cellPosition) == false || cellPosition.Key == 0)
	{
		return center;
	}

	// Ring에 존재하는 Cell의 개수
	int32 cellCountInRing = polarGrid[cellPosition.Key].Num();

	// Ring의 반지름
	float ringRadius = centerRadius + ((cellPosition.Key - 0.5f) * ringDistance);

	// 개별 Cell의 중심각
	float eachCellDeg = 360.0f / cellCountInRing;

	// Cell의 위치 계산
	FVector cellLocation = center + (ringRadius * FVector::ForwardVector.RotateAngleAxis(cellPosition.Key * eachCellDeg, FVector::UpVector));
	return cellLocation;
}
