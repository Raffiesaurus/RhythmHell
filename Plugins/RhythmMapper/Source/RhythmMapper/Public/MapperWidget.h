#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Sound/SoundWaveProcedural.h"
#include "MapperWidget.generated.h"

class UEditorUtilityEditableTextBox;
class UEditorUtilityButton;
class USinglePropertyView;
class UAssetThumbnailWidget;
class UTextBlock;
class UAudioComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogAudioPlayer, Warning, All);

/**
 * Audio player widget for WAV files
 */
UCLASS()
class UMapperWidget : public UEditorUtilityWidget {
	GENERATED_BODY()

public:
	UMapperWidget();
	virtual ~UMapperWidget();

protected:
	// Native overrides
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	// virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

private:
	// Track struct to store audio files info
	struct FAudioTrack {
		FString FilePath;
		FString DisplayName;
	};

	// WAV format data structure
	struct FWavFormat {
		int32 DataOffset;
		int32 DataSize;
		int32 SampleRate;
		int32 Channels;
		int32 BitsPerSample;
		float DurationSeconds;
	};

	// Audio playback state
	enum class EAudioState {
		Stopped,
		Playing,
		Paused
	};

	// Widget creation methods
	void CreateFolderTextBox(UPanelWidget* RootPanel);
	void CreateThumbnailWidget(UPanelWidget* RootPanel);
	void CreateCurrentSongText(UPanelWidget* RootPanel);
	void CreatePlayButton(UPanelWidget* RootPanel);
	void CreatePlayText();
	void CreateStopButton(UPanelWidget* RootPanel);
	void CreateStopText() const;
	void CreateNextPrevButtons(UPanelWidget* RootPanel);

	// Audio file handling
	UFUNCTION()
	void AssignFolderPath(const FText& InText, ETextCommit::Type CommitMethod);

	void LoadFolderData();
	bool LoadAudioFile(const FString& FilePath);
	FWavFormat ParseWavHeader(const TArray<uint8>& WavData);
	void UpdateCurrentSongDisplay();

	// Playback control
	UFUNCTION()
	void OnPlayPauseButtonClick();

	UFUNCTION()
	void OnStopAudio();

	UFUNCTION()
	void OnNextTrack();

	UFUNCTION()
	void OnPrevTrack();

	UFUNCTION()
	void HandleTrackEnd();

	void EnsureAudioComponentExists();
	void PlayCurrentTrack();
	void PausePlayback();
	void ResumePlayback();
	void UpdateButtonLabels() const;

	// Widget state
	FString FolderPath;

	UPROPERTY()
	UEditorUtilityEditableTextBox* FolderLocationTextBox;

	UPROPERTY()
	USinglePropertyView* SoundSelectorView;

	UPROPERTY()
	UAssetThumbnailWidget* SoundThumbnailWidget;

	UPROPERTY()
	UEditorUtilityButton* PlayPauseButton;

	UPROPERTY()
	UEditorUtilityButton* StopButton;

	UPROPERTY()
	UEditorUtilityButton* NextButton;

	UPROPERTY()
	UEditorUtilityButton* PrevButton;

	UPROPERTY()
	UTextBlock* PlayText;

	UPROPERTY()
	UTextBlock* CurrentSongText;

	// Audio state
	UPROPERTY()
	UAudioComponent* AudioComponent;

	UPROPERTY()
	USoundWaveProcedural* CurrentSoundWave;

	TArray<FAudioTrack> AudioTracks;
	TMap<FString, TArray<uint8>> AudioDataCache;
	int32 CurrentTrackIndex;
	EAudioState CurrentAudioState = EAudioState::Stopped;

	UPROPERTY()
	UWorld* EditorWorld;

	bool bIsEditorFocused;
	int32 ConstructCount;

	// Timer for track auto-advance
	FTimerHandle TrackEndTimerHandle;
};
