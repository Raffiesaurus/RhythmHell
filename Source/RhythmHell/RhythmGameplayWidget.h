#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RhythmGameplayController.h"
#include "RhythmNote.h"
#include "RhythmGameplayWidget.generated.h"

UCLASS()
class RHYTHMHELL_API URhythmGameplayWidget : public UUserWidget {
	GENERATED_BODY()

public:
	URhythmGameplayWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Rhythm Gameplay")
	void SetGameplayController(ARhythmGameplayController* InController);

	UFUNCTION(BlueprintCallable, Category = "Rhythm Gameplay")
	void InitializeGameplayUI(const FRhythmLevel& Level);

	UFUNCTION(BlueprintCallable, Category = "Rhythm Gameplay")
	void ProcessInput(const FString& Direction);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm Settings")
	float NoteSpawnLeadTime;

	UPROPERTY(BlueprintReadOnly, Category = "Rhythm Gameplay")
	ARhythmGameplayController* GameplayController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm UI")
	TSubclassOf<URhythmNote> NoteWidgetClass;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* UpLanePanel;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* DownLanePanel;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* LeftLanePanel;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* RightLanePanel;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ScoreText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ComboText;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ProgressBar;

protected:
	UPROPERTY()
	TMap<int32, URhythmNote*> ActiveNotes;

	UPROPERTY(BlueprintReadOnly)
	float CurrentTime;
	float TotalDuration;

	bool bIsGameplayActive;

	int32 NextSpawnIndex;

	UFUNCTION(BlueprintCallable, Category = "Rhythm UI")
	UCanvasPanel* GetLanePanelForDirection(const FString& Direction);

	UFUNCTION(BlueprintCallable, Category = "Rhythm UI")
	URhythmNote* SpawnNoteWidget(const FRhythmHit& Hit);

	UFUNCTION(BlueprintCallable, Category = "Rhythm UI")
	void UpdateLongNotes();

	UFUNCTION()
	void OnRhythmHitResult(ERhythmHitResult Result, const FString& Direction);

	UFUNCTION()
	void OnComboUpdate(int32 ComboCount);

	UFUNCTION()
	void OnLevelComplete(int32 TotalScore, int32 MaxCombo, const FString& Rank);

	UFUNCTION(BlueprintNativeEvent, Category = "Rhythm UI")
	void SetupLanes();
	virtual void SetupLanes_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Rhythm UI")
	void ShowInputFeedback(const FString& Direction);
	virtual void ShowInputFeedback_Implementation(const FString& Direction);

	UFUNCTION(BlueprintNativeEvent, Category = "Rhythm UI")
	void ShowHitResult(ERhythmHitResult Result, const FString& Direction);
	virtual void ShowHitResult_Implementation(ERhythmHitResult Result, const FString& Direction);

	UFUNCTION(BlueprintNativeEvent, Category = "Rhythm UI")
	void ShowLevelComplete(int32 TotalScore, int32 MaxCombo, const FString& Rank);
	virtual void ShowLevelComplete_Implementation(int32 TotalScore, int32 MaxCombo, const FString& Rank);

	UFUNCTION()
	void OnAudioPlayBackPercent(const USoundWave* SoundWave, const float Percentage);
};
