// Raffiesaurus, 2025

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Components/AudioComponent.h"
#include "Helpers.h"
#include "RhythmGameplayController.h"
#include "RhythmMapperBFL.h"
#include "RhythmMapperEditorWidget.generated.h"

UCLASS()
class RHYTHMHELL_API URhythmMapperEditorWidget : public UEditorUtilityWidget {
	GENERATED_BODY()

public:
	URhythmMapperEditorWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundWave* SongAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm")
	float BPM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm")
	float StartOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm")
	float CompletedPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm")
	float TotalDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FString LevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FString SongName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	Difficulty LevelDifficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm")
	TArray<FRhythmHit> Hits;

	UPROPERTY(BlueprintReadOnly, Category = "Audio")
	float CurrentTime;

	UPROPERTY(BlueprintReadOnly, Category = "Audio")
	bool bIsPlaying;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void LoadSong(USoundWave* NewSongAsset);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlaySong();

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PauseSong();

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void ResumeSong();

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StopSong();

	UFUNCTION(BlueprintCallable, Category = "Editing")
	void AddHit(const FString& Direction, RhythmHitMarking HitType = RhythmHitMarking::BASIC);

	UFUNCTION(BlueprintCallable, Category = "Editing")
	void StartLongHit(const FString& Direction);

	UFUNCTION(BlueprintCallable, Category = "Editing")
	void EndLongHit(const FString& Direction);

	UFUNCTION(BlueprintCallable, Category = "Editing")
	void RemoveHit(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Editing")
	void ClearAllHits();

	UFUNCTION(BlueprintCallable, Category = "File")
	bool SaveLevelToJSON(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "File")
	bool LoadLevelFromJSON(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "Editing")
	void AutoPlaceHits(int32 BeatsPerMeasure, int32 NumMeasures);

	UFUNCTION(BlueprintCallable, Category = "Editing")
	void QuantizeHits();

	UFUNCTION(BlueprintCallable, Category = "Editing")
	TArray<FRhythmHit> GetHitsInTimeRange(float StartTime, float EndTime);

	UFUNCTION(BlueprintCallable, Category = "Audio Update")
	void UpdateCurrentSongTime(const USoundWave* PlayingSoundWave, const float PlaybackPercent);

protected:
	TMap<FString, float> LongHitsInProgress;

	void SortHits();

	bool IsSongLoaded() const;
};
