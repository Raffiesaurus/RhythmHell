// Raffiesaurus, 2025

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputActionValue.h"
#include "RhythmGameplayController.h"
#include "RhythmInputComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RHYTHMHELL_API URhythmInputComponent : public UActorComponent {
	GENERATED_BODY()

public:
	URhythmInputComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm Gameplay")
	ARhythmGameplayController* GameplayController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* UpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* DownAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* LeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* RightAction;

	void BindInputActions(class UEnhancedInputComponent* EnhancedInputComponent);

	bool bIsInputEnabled;

	UFUNCTION(BlueprintCallable, Category = "Rhythm Gameplay")
	void SetInputEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Rhythm Gameplay")
	void SetGameplayController(ARhythmGameplayController* InController);

private:
	void HandleUpInput(const FInputActionValue& Value);
	void HandleDownInput(const FInputActionValue& Value);
	void HandleLeftInput(const FInputActionValue& Value);
	void HandleRightInput(const FInputActionValue& Value);

	void ProcessRhythmInput(const FString& Direction);
};
