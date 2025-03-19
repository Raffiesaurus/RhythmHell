// Raffiesaurus, 2025


#include "RhythmGameInstance.h"

void URhythmGameInstance::SetVinylJSON(FString ChosenVinylJSON) {
	UE_LOG(LogTemp, Warning, TEXT("Data set: %s"), *ChosenVinylJSON);
	VinylJSON = ChosenVinylJSON;
}

FString URhythmGameInstance::GetVinylJSON() {
	return VinylJSON;
}
