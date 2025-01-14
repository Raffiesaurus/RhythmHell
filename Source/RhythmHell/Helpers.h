// Raffiesaurus, 2025

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class RhythmHitMarking : uint8 {
	BASIC		= 0		UMETA(DisplayName = "Hit"),
	LONG_START	= 1		UMETA(DisplayName = "Long Start"),
	LONG_END	= 2     UMETA(DisplayName = "Long End"),
};

UENUM(BlueprintType)
enum class Difficulty : uint8 {
	EASY	= 0		UMETA(DisplayName = "Easy"),
	MEDIUM	= 1		UMETA(DisplayName = "Medium"),
	HARD	= 2     UMETA(DisplayName = "Hard"),
	HELL	= 3		UMETA(DisplayName = "Hell"),
};
