// Raffiesaurus, 2025

#pragma once

#include "CoreMinimal.h"
#include "WidgetBlueprint.h"
#include "VinylInfoWidget.generated.h"

/**
 *
 */
UCLASS()
class RHYTHMHELL_API UVinylInfoWidget : public UWidgetBlueprint
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vinyl Info")
	FString SongName;

	UFUNCTION(BlueprintCallable, Category = "Vinyl Info")
	FString GetSongName() const { return SongName; }
};
