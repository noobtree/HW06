// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MazeCell.generated.h"

/**
 * 
 */
USTRUCT()
struct HW06_API FMazeCell
{
	GENERATED_BODY()
public:
	// row Index
	UPROPERTY(EditAnywhere, Category = "MazeCell")
	int32 row = 0;
	// column Index
	UPROPERTY(EditAnywhere, Category = "MazeCell")
	int32 column = 0;

	// + row 방향 벽 존재 여부
	UPROPERTY(EditAnywhere, Category = "MazeCell|Wall")
	bool bHasPositiveRowWall = true;

	// - row 방향 벽 존재 여부
	UPROPERTY(EditAnywhere, Category = "MazeCell|Wall")
	bool bHasNegativeRowWall = true;

	// + column 방향 벽 존재 여부
	UPROPERTY(EditAnywhere, Category = "MazeCell|Wall")
	bool bHasPositiveColumnWall = true;

	// - column 방향 벽 존재 여부
	UPROPERTY(EditAnywhere, Category = "MazeCell|Wall")
	bool bHasNegativeColumnWall = true;

	// 다른 Cell에서 현재 Cell을 수정할 수 있는지 여부
	// 시작 지점에서 나아가는 길을 1개로 제한하기 위함
	UPROPERTY(EditAnywhere, Category = "MazeCell")
	bool bIsEditable = true;

	// 해당 Cell이 미로 내부 경로와 연결되어 있는지를 나타내는 변수
	UPROPERTY(EditAnywhere, Category = "MazeCell")
	bool bIsInMaze = false;
};
