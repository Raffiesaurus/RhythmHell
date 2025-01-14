// Raffiesaurus, 2025

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BeatMap.h"
#include "AudioDevice.h"
#include "BeatMapEditor.generated.h"

UCLASS()
class RHYTHMHELL_API ABeatMapEditor : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABeatMapEditor();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Editor")
	ABeatMap* CurrentBeatMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Editor")
	float PlaybackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Editor")
	float CurrentTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Editor")
	bool bIsPlaying;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Editor")
	float GridSnap;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* EditorInputMapping;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* PlayPauseAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* StopAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* DeleteNoteAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* SeekAction;

	UFUNCTION(BlueprintCallable, Category = "Editor")
	void PlayPause();

	UFUNCTION(BlueprintCallable, Category = "Editor")
	void Stop();

	UFUNCTION(BlueprintCallable, Category = "Editor")
	void SeekTo(float Time);

	UFUNCTION(BlueprintCallable, Category = "Editor")
	void SetPlaybackSpeed(float Speed);

	UFUNCTION(BlueprintCallable, Category = "Editor")
	void AddNoteAtTime(float Time, int32 Lane, float Duration, FString NoteType);

	UFUNCTION(BlueprintCallable, Category = "Editor")
	void DeleteNote(int32 NoteIndex);

	UFUNCTION(BlueprintCallable, Category = "Editor")
	void ModifyNote(int32 NoteIndex, float NewTime, int32 NewLane, float NewDuration);

private:
	UAudioComponent* AudioComponent;
	void UpdateAudioPlayback();
};