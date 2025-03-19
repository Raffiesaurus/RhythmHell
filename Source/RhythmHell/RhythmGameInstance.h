// Raffiesaurus, 2025

#pragma once

#include "CoreMinimal.h"
#include "VinylRecord.h"
#include "Engine/GameInstance.h"
#include "RhythmGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class RHYTHMHELL_API URhythmGameInstance : public UGameInstance {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Game Data")
	FString VinylJSON;

public :
	UFUNCTION(BlueprintCallable, Category="Game Functions")
	void SetVinylJSON(FString ChosenVinylJSON);

	UFUNCTION(BlueprintCallable, Category="Game Functions")
	FString GetVinylJSON();
};
