// Raffiesaurus, 2025

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"
/**
 *
 */

UINTERFACE(MinimalAPI)
class UInteractable :public UInterface {
	GENERATED_BODY()
};

class RHYTHMHELL_API IInteractable
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Highlight(bool bEnable);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteract(AActor* Interactor);
};
