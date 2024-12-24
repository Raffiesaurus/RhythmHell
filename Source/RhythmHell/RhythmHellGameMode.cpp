// Copyright Epic Games, Inc. All Rights Reserved.

#include "RhythmHellGameMode.h"
#include "RhythmHellCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARhythmHellGameMode::ARhythmHellGameMode() {
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Extras/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL) {
		DefaultPawnClass = PlayerPawnBPClass.Class;
	} else {
		UE_LOG(LogTemp, Warning, TEXT("NULL"));
	}
}
