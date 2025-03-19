// Raffiesaurus, 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RhythmGameplayController.h"
#include "RhythmGameplayWidget.h"
#include "RhythmHellCharacter.h"
#include "RhythmInputComponent.h"
#include "RhythmGameplayLevel.generated.h"

UCLASS()
class RHYTHMHELL_API ARhythmGameplayLevel : public AGameModeBase {
	GENERATED_BODY()

public:
	ARhythmGameplayLevel();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Setup")
	FString LevelJSONPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay")
	ARhythmGameplayController* GameplayController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	URhythmGameplayWidget* GameplayWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<URhythmGameplayWidget> GameplayWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void StartGameplay();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void PauseGameplay();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void ResumeGameplay();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void EndGameplay();

protected:
	UFUNCTION(BlueprintCallable, Category = "Level Setup")
	bool InitializeLevel();

	UFUNCTION(BlueprintCallable, Category = "Level Setup")
	void SetupPlayerCharacter();

	UFUNCTION()
	void OnLevelComplete(int32 TotalScore, int32 MaxCombo, const FString& Rank);

private:
	bool bIsLevelInitialized;

	bool bIsGameplayActive;

	bool bIsGameplayPaused;
};
