// Raffiesaurus, 2025

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RhythmMapperBFL.generated.h"

/**
 * 
 */
UCLASS()
class RHYTHMHELL_API URhythmMapperBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Save")
	static bool FileSaveString(FString SaveTextB, FString FileNameB);

	UFUNCTION(BlueprintPure, Category = "Save")
	static bool FileLoadString(FString FileNameA, FString& SaveTextA);


};
