// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PolarCell.generated.h"

/**
 * 원형 미로 생성을 위한 노드
 */
USTRUCT(BlueprintType)
struct HW06_API FPolarCell
{
	GENERATED_BODY()
public:
	// Ring 인덱스
	UPROPERTY()
	int32 ringIndex = 0;

	// Section 인덱스
	UPROPERTY()
	int32 cellIndex = 0;

	// 안쪽(중심쪽) 벽 존재 여부
	UPROPERTY()
	bool bHasInnerWall = true;

	// 바깥쪽(외곽쪽) 벽 존재 여부
	UPROPERTY()
	bool bHasOuterWall = true;

	// 시계방향 벽 존재 여부
	UPROPERTY()
	bool bHasClockwiseWall = true;

	// 반시계방향 벽 존재 여부
	UPROPERTY()
	bool bHasCounterclockwiseWall = true;

	// 미로 생성 시 중복 방지를 위한 플래그
	UPROPERTY()
	bool bIsVisited = false;

	// Cell이 미로 구조에 포함되었는지 나타내는 플래그
	UPROPERTY()
	bool bIsInMazeTree = false;
};
