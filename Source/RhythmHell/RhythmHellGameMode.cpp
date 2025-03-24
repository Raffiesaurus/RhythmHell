// Copyright Epic Games, Inc. All Rights Reserved.

#include "RhythmHellGameMode.h"
#include "UObject/ConstructorHelpers.h"

ARhythmHellGameMode::ARhythmHellGameMode() {
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Player/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != nullptr) {
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}