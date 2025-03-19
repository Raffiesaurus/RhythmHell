#include "MapperWidget.h"

#include "EditorUtilityWidgetComponents.h"
#include "Components/ButtonSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/AssetThumbnailWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Widgets/SWindow.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/FileHelper.h"
#include "Kismet/GameplayStatics.h"
#include "Algo/RandomShuffle.h"
#include "Components/AudioComponent.h"

DEFINE_LOG_CATEGORY(LogAudioPlayer);

UMapperWidget::UMapperWidget()
	: FolderLocationTextBox(nullptr)
	  , SoundSelectorView(nullptr), SoundThumbnailWidget(nullptr)
	  , PlayPauseButton(nullptr)
	  , StopButton(nullptr)
	  , NextButton(nullptr)
	  , PrevButton(nullptr)
	  , PlayText(nullptr)
	  , CurrentSongText(nullptr)
	  , AudioComponent(nullptr)
	  , CurrentSoundWave(nullptr)
	  , CurrentTrackIndex(0)
	  , EditorWorld(nullptr)
	  , bIsEditorFocused(false)
	  , ConstructCount(0) {}

UMapperWidget::~UMapperWidget() {
	if (UUserWidget::GetWorld()) {
		UUserWidget::GetWorld()->GetTimerManager().ClearTimer(TrackEndTimerHandle);
	}
}

//------------------------------------------------------------------------------
// Widget Creation Methods
//------------------------------------------------------------------------------

void UMapperWidget::CreateFolderTextBox(UPanelWidget* RootPanel) {
	FolderLocationTextBox = WidgetTree->ConstructWidget<UEditorUtilityEditableTextBox>(
		UEditorUtilityEditableTextBox::StaticClass());
	RootPanel->AddChild(FolderLocationTextBox);
	UCanvasPanelSlot* FolderTextBoxSlot = Cast<UCanvasPanelSlot>(FolderLocationTextBox->Slot);
	FolderTextBoxSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 0.0f));
	FolderTextBoxSlot->SetOffsets(FMargin(0, 30, 0, 30));
	FolderLocationTextBox->OnTextCommitted.AddDynamic(this, &UMapperWidget::AssignFolderPath);
}

void UMapperWidget::CreateThumbnailWidget(UPanelWidget* RootPanel) {
	SoundThumbnailWidget = WidgetTree->ConstructWidget<UAssetThumbnailWidget>(
		UAssetThumbnailWidget::StaticClass());
	SoundThumbnailWidget->SetResolution(FIntPoint(1920, 70));
	RootPanel->AddChild(SoundThumbnailWidget);
	UCanvasPanelSlot* ThumbnailSlot = Cast<UCanvasPanelSlot>(SoundThumbnailWidget->Slot);
	ThumbnailSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 0.0f));
	ThumbnailSlot->SetOffsets(FMargin(0, 80, 0, 80));
}

void UMapperWidget::CreateCurrentSongText(UPanelWidget* RootPanel) {
	CurrentSongText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	RootPanel->AddChild(CurrentSongText);
	UCanvasPanelSlot* TextSlot = Cast<UCanvasPanelSlot>(CurrentSongText->Slot);
	TextSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 0.0f));
	TextSlot->SetOffsets(FMargin(0, 160, 0, 0));

	FSlateFontInfo FontToUse = CurrentSongText->GetFont();
	FontToUse.Size = 14;
	CurrentSongText->SetFont(FontToUse);
	CurrentSongText->SetText(FText::FromString("No song loaded"));
	CurrentSongText->SetJustification(ETextJustify::Center);
}

void UMapperWidget::CreatePlayButton(UPanelWidget* RootPanel) {
	PlayPauseButton = WidgetTree->ConstructWidget<UEditorUtilityButton>(
		UEditorUtilityButton::StaticClass());
	RootPanel->AddChild(PlayPauseButton);
	UCanvasPanelSlot* PlayButtonSlot = Cast<UCanvasPanelSlot>(PlayPauseButton->Slot);
	PlayButtonSlot->SetAnchors(FAnchors(0.4f, 0.0f, 0.6f, 0.0f));
	PlayButtonSlot->SetOffsets(FMargin(0, 200, 0, 30));
	CreatePlayText();
	PlayPauseButton->OnClicked.AddDynamic(this, &UMapperWidget::OnPlayPauseButtonClick);
}

void UMapperWidget::CreatePlayText() {
	PlayText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	PlayPauseButton->AddChild(PlayText);
	FSlateFontInfo FontToUse = PlayText->GetFont();
	FontToUse.Size = 12;
	PlayText->SetText(FText::FromString("PLAY"));
	PlayText->SetFont(FontToUse);
	UButtonSlot* PlayTextSlot = Cast<UButtonSlot>(PlayText->Slot);
	PlayTextSlot->SetHorizontalAlignment(HAlign_Center);
	PlayTextSlot->SetVerticalAlignment(VAlign_Center);
}

void UMapperWidget::CreateStopButton(UPanelWidget* RootPanel) {
	StopButton = WidgetTree->ConstructWidget<UEditorUtilityButton>(
		UEditorUtilityButton::StaticClass());
	RootPanel->AddChild(StopButton);
	UCanvasPanelSlot* StopButtonSlot = Cast<UCanvasPanelSlot>(StopButton->Slot);
	StopButtonSlot->SetAnchors(FAnchors(0.4f, 0.0f, 0.6f, 0.0f));
	StopButtonSlot->SetOffsets(FMargin(0, 250, 0, 30));
	CreateStopText();
	StopButton->OnClicked.AddDynamic(this, &UMapperWidget::OnStopAudio);
}

void UMapperWidget::CreateStopText() const {
	UTextBlock* StopText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	StopButton->AddChild(StopText);
	FSlateFontInfo FontToUse = StopText->GetFont();
	FontToUse.Size = 12;
	StopText->SetText(FText::FromString("STOP"));
	StopText->SetFont(FontToUse);
	UButtonSlot* StopTextSlot = Cast<UButtonSlot>(StopText->Slot);
	StopTextSlot->SetHorizontalAlignment(HAlign_Center);
	StopTextSlot->SetVerticalAlignment(VAlign_Center);
}

void UMapperWidget::CreateNextPrevButtons(UPanelWidget* RootPanel) {
	// Next Button
	NextButton = WidgetTree->ConstructWidget<UEditorUtilityButton>(
		UEditorUtilityButton::StaticClass());
	RootPanel->AddChild(NextButton);
	UCanvasPanelSlot* NextButtonSlot = Cast<UCanvasPanelSlot>(NextButton->Slot);
	NextButtonSlot->SetAnchors(FAnchors(0.6f, 0.0f, 0.7f, 0.0f));
	NextButtonSlot->SetOffsets(FMargin(10, 225, 0, 30));

	UTextBlock* NextText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	NextButton->AddChild(NextText);
	FSlateFontInfo NextFontToUse = NextText->GetFont();
	NextFontToUse.Size = 12;
	NextText->SetText(FText::FromString("->|"));
	NextText->SetFont(NextFontToUse);
	UButtonSlot* NextTextSlot = Cast<UButtonSlot>(NextText->Slot);
	NextTextSlot->SetHorizontalAlignment(HAlign_Center);
	NextTextSlot->SetVerticalAlignment(VAlign_Center);

	NextButton->OnClicked.AddDynamic(this, &UMapperWidget::OnNextTrack);

	// Previous Button
	PrevButton = WidgetTree->ConstructWidget<UEditorUtilityButton>(
		UEditorUtilityButton::StaticClass());
	RootPanel->AddChild(PrevButton);
	UCanvasPanelSlot* PrevButtonSlot = Cast<UCanvasPanelSlot>(PrevButton->Slot);
	PrevButtonSlot->SetAnchors(FAnchors(0.3f, 0.0f, 0.4f, 0.0f));
	PrevButtonSlot->SetOffsets(FMargin(0, 225, 10, 30));

	UTextBlock* PrevText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	PrevButton->AddChild(PrevText);
	FSlateFontInfo PrevFontToUse = PrevText->GetFont();
	PrevFontToUse.Size = 12;
	PrevText->SetText(FText::FromString("|<-"));
	PrevText->SetFont(PrevFontToUse);
	UButtonSlot* PrevTextSlot = Cast<UButtonSlot>(PrevText->Slot);
	PrevTextSlot->SetHorizontalAlignment(HAlign_Center);
	PrevTextSlot->SetVerticalAlignment(VAlign_Center);

	PrevButton->OnClicked.AddDynamic(this, &UMapperWidget::OnPrevTrack);
}

//------------------------------------------------------------------------------
// File Handling Methods
//------------------------------------------------------------------------------
void UMapperWidget::AssignFolderPath(const FText& InText, ETextCommit::Type CommitMethod) {
	if (CommitMethod == ETextCommit::OnEnter) {
		FolderPath = InText.ToString();
		LoadFolderData();
	}
}

void UMapperWidget::LoadFolderData() {
	const FString FolderDirectory = FolderPath;
	if (!FPaths::DirectoryExists(FolderDirectory)) {
		UE_LOG(LogAudioPlayer, Warning, TEXT("Directory does not exist: %s"), *FolderDirectory);
		return;
	}

	AudioDataCache.Empty();
	AudioTracks.Empty();

	TArray<FString> WAVFiles;
	IFileManager::Get().FindFiles(WAVFiles, *FolderDirectory, TEXT("*.wav"));

	for (const FString& FileName : WAVFiles) {
		FString FullFilePath = FPaths::Combine(FolderDirectory, FileName);

		FAudioTrack Track;
		Track.FilePath = FullFilePath;
		Track.DisplayName = FPaths::GetBaseFilename(FileName);

		AudioTracks.Add(Track);
		UE_LOG(LogAudioPlayer, Warning, TEXT("Found audio file: %s (%s)"), *Track.FilePath, *Track.DisplayName);
	}

	Algo::RandomShuffle(AudioTracks);

	if (AudioTracks.Num() > 0) {
		CurrentTrackIndex = 0;
		UpdateCurrentSongDisplay();
	}

	UE_LOG(LogAudioPlayer, Warning, TEXT("Found %d audio files"), AudioTracks.Num());
}

bool UMapperWidget::LoadAudioFile(const FString& FilePath) {
	if (AudioDataCache.Contains(FilePath)) {
		return true;
	}

	TArray<uint8> AudioData;
	if (!FFileHelper::LoadFileToArray(AudioData, *FilePath)) {
		UE_LOG(LogAudioPlayer, Warning, TEXT("Failed to load audio file: %s"), *FilePath);
		return false;
	}

	// Verify minimum WAV size
	if (AudioData.Num() < 44) {
		UE_LOG(LogAudioPlayer, Warning, TEXT("File too small to be a valid WAV: %s"), *FilePath);
		return false;
	}

	AudioDataCache.Add(FilePath, AudioData);
	return true;
}

UMapperWidget::FWavFormat UMapperWidget::ParseWavHeader(const TArray<uint8>& WavData) {
	FWavFormat Result;
	Result.DataOffset = 0;
	Result.DataSize = 0;
	Result.SampleRate = 44100; // Default
	Result.Channels = 2; // Default
	Result.BitsPerSample = 16; // Default
	Result.DurationSeconds = 0; // Default

	// Check RIFF header
	if (WavData.Num() < 12 ||
		WavData[0] != 'R' || WavData[1] != 'I' || WavData[2] != 'F' || WavData[3] != 'F' ||
		WavData[8] != 'W' || WavData[9] != 'A' || WavData[10] != 'V' || WavData[11] != 'E') {
		UE_LOG(LogAudioPlayer, Warning, TEXT("Invalid WAV header - missing RIFF/WAVE markers"));
		return Result;
	}

	// Find the 'fmt ' chunk (yes there is a space after t, and yes I wasted 18365985 hours because I thought it was a typo)
	int32 FmtOffset = -1;
	for (int32 i = 12; i < WavData.Num() - 8; i++) {
		if (WavData[i] == 'f' && WavData[i + 1] == 'm' && WavData[i + 2] == 't' && WavData[i + 3] == ' ') {
			FmtOffset = i;
			break;
		}
	}

	if (FmtOffset != -1 && FmtOffset + 24 <= WavData.Num()) {
		// Parse the format chunk
		Result.Channels = WavData[FmtOffset + 10] | (WavData[FmtOffset + 11] << 8);
		Result.SampleRate = WavData[FmtOffset + 12] | (WavData[FmtOffset + 13] << 8) |
			(WavData[FmtOffset + 14] << 16) | (WavData[FmtOffset + 15] << 24);
		Result.BitsPerSample = WavData[FmtOffset + 22] | (WavData[FmtOffset + 23] << 8);

		UE_LOG(LogAudioPlayer, Warning, TEXT("WAV Format: Channels=%d, SampleRate=%d, BitsPerSample=%d"),
		       Result.Channels, Result.SampleRate, Result.BitsPerSample);
	}

	// Find the 'data' chunk
	for (int32 i = 12; i < WavData.Num() - 8; i++) {
		if (WavData[i] == 'd' && WavData[i + 1] == 'a' && WavData[i + 2] == 't' && WavData[i + 3] == 'a') {
			// Get data chunk size
			Result.DataSize = WavData[i + 4] | (WavData[i + 5] << 8) | (WavData[i + 6] << 16) | (WavData[i + 7] << 24);
			Result.DataOffset = i + 8; // Skip "data" and size field

			UE_LOG(LogAudioPlayer, Warning, TEXT("WAV Data: Offset=%d, Size=%d"), Result.DataOffset, Result.DataSize);
			break;
		}
	}

	// Calculate duration based on the WAV parameters
	if (Result.SampleRate > 0 && Result.Channels > 0 && Result.BitsPerSample > 0 && Result.DataSize > 0) {
		const int32 TotalSamples = (Result.DataSize * 8) / (Result.BitsPerSample * Result.Channels);
		Result.DurationSeconds = static_cast<float>(TotalSamples) / static_cast<float>(Result.SampleRate);
		UE_LOG(LogAudioPlayer, Warning, TEXT("Calculated WAV duration: %f seconds"), Result.DurationSeconds);
	} else {
		Result.DurationSeconds = 0.0f;
	}

	return Result;
}

void UMapperWidget::UpdateCurrentSongDisplay() {
	if (CurrentSongText && AudioTracks.IsValidIndex(CurrentTrackIndex)) {
		const FString SongDisplayText = FString::Printf(TEXT("Now Playing: %s"),
		                                                *AudioTracks[CurrentTrackIndex].DisplayName);
		CurrentSongText->SetText(FText::FromString(SongDisplayText));
	} else if (CurrentSongText) {
		CurrentSongText->SetText(FText::FromString("No song loaded"));
	}
}

//------------------------------------------------------------------------------
// Audio Control Methods
//------------------------------------------------------------------------------
void UMapperWidget::OnPlayPauseButtonClick() {
	switch (CurrentAudioState) {
	case EAudioState::Stopped:
		PlayCurrentTrack();
		break;

	case EAudioState::Playing:
		PausePlayback();
		break;

	case EAudioState::Paused:
		ResumePlayback();
		break;
	}
}

void UMapperWidget::OnStopAudio() {
	if (GetWorld()) {
		GetWorld()->GetTimerManager().ClearTimer(TrackEndTimerHandle);
	}

	if (AudioComponent && AudioComponent->IsValidLowLevel()) {
		AudioComponent->Stop();
		CurrentAudioState = EAudioState::Stopped;
		UpdateButtonLabels();
		UE_LOG(LogAudioPlayer, Warning, TEXT("Playback stopped"));
	}
}

void UMapperWidget::OnNextTrack() {
	if (AudioTracks.Num() == 0) {
		return;
	}

	if (GetWorld()) {
		GetWorld()->GetTimerManager().ClearTimer(TrackEndTimerHandle);
	}

	CurrentTrackIndex = (CurrentTrackIndex + 1) % AudioTracks.Num();

	if (AudioComponent && AudioComponent->IsValidLowLevel()) {
		AudioComponent->Stop();
	}

	UpdateCurrentSongDisplay();

	if (CurrentAudioState == EAudioState::Playing) {
		PlayCurrentTrack();
	} else {
		OnStopAudio();
	}
}

void UMapperWidget::OnPrevTrack() {
	if (AudioTracks.Num() == 0) {
		return;
	}

	if (GetWorld()) {
		GetWorld()->GetTimerManager().ClearTimer(TrackEndTimerHandle);
	}

	CurrentTrackIndex = (CurrentTrackIndex - 1);
	if (CurrentTrackIndex < 0) {
		CurrentTrackIndex = AudioTracks.Num() - 1;
	}

	const bool bWasPlaying = (CurrentAudioState == EAudioState::Playing);

	if (AudioComponent && AudioComponent->IsValidLowLevel()) {
		AudioComponent->Stop();
	}

	UpdateCurrentSongDisplay();

	if (bWasPlaying) {
		PlayCurrentTrack();
	} else {
		OnStopAudio();
	}
}

void UMapperWidget::HandleTrackEnd() {
	UE_LOG(LogAudioPlayer, Warning, TEXT("Track ended, advancing to next track"));

	if (CurrentAudioState == EAudioState::Stopped) {
		return;
	}

	OnNextTrack();
}

void UMapperWidget::EnsureAudioComponentExists() {
	if (!EditorWorld) {
		EditorWorld = GEditor->GetEditorWorldContext().World();
		if (!EditorWorld) {
			UE_LOG(LogAudioPlayer, Warning, TEXT("Failed to get Editor World"));
			return;
		}
	}

	if (!AudioComponent || !AudioComponent->IsValidLowLevel()) {
		USoundWave* DummySound = NewObject<USoundWave>(GetTransientPackage());

		AudioComponent = UGameplayStatics::CreateSound2D(
			EditorWorld,
			DummySound,
			1.0f,
			1.0f,
			0.0f,
			nullptr,
			true,
			false
		);

		if (!AudioComponent || !AudioComponent->IsValidLowLevel()) {
			UE_LOG(LogAudioPlayer, Warning, TEXT("Failed to create AudioComponent"));
			return;
		}

		UE_LOG(LogAudioPlayer, Warning, TEXT("Created new AudioComponent"));
	}
}

void UMapperWidget::PlayCurrentTrack() {
	if (AudioTracks.Num() == 0) {
		UE_LOG(LogAudioPlayer, Warning, TEXT("No tracks available to play"));
		return;
	}

	if (CurrentTrackIndex >= AudioTracks.Num()) {
		CurrentTrackIndex = 0;
	}

	FAudioTrack& CurrentTrack = AudioTracks[CurrentTrackIndex];
	UE_LOG(LogAudioPlayer, Warning, TEXT("Playing track: %s (%s)"),
	       *CurrentTrack.FilePath, *CurrentTrack.DisplayName);

	if (!LoadAudioFile(CurrentTrack.FilePath)) {
		UE_LOG(LogAudioPlayer, Warning, TEXT("Failed to load audio file"));
		return;
	}

	TArray<uint8>& AudioData = AudioDataCache[CurrentTrack.FilePath];

	const FWavFormat WavFormat = ParseWavHeader(AudioData);
	if (WavFormat.DataOffset == 0 || WavFormat.DataSize == 0) {
		UE_LOG(LogAudioPlayer, Warning, TEXT("Failed to parse WAV header"));
		return;
	}

	if (!CurrentSoundWave || !CurrentSoundWave->IsValidLowLevel()) {
		CurrentSoundWave = NewObject<USoundWaveProcedural>(GetTransientPackage());
	}

	CurrentSoundWave->ResetAudio();
	CurrentSoundWave->SetSampleRate(WavFormat.SampleRate);
	CurrentSoundWave->NumChannels = WavFormat.Channels;
	CurrentSoundWave->bLooping = false;

	if (WavFormat.DataOffset + WavFormat.DataSize <= AudioData.Num()) {
		CurrentSoundWave->QueueAudio(
			AudioData.GetData() + WavFormat.DataOffset,
			WavFormat.DataSize
		);

		EnsureAudioComponentExists();

		if (AudioComponent && AudioComponent->IsValidLowLevel()) {
			if (GetWorld()) {
				GetWorld()->GetTimerManager().ClearTimer(TrackEndTimerHandle);
			}

			AudioComponent->Stop();
			AudioComponent->SetSound(CurrentSoundWave);
			AudioComponent->SetVolumeMultiplier(1.0f);
			AudioComponent->SetPaused(false);
			AudioComponent->Play();

			CurrentAudioState = EAudioState::Playing;
			UpdateButtonLabels();
			UpdateCurrentSongDisplay();

			if (WavFormat.DurationSeconds > 0.0f) {
				float TimerDuration = WavFormat.DurationSeconds + 0.5f;

				UE_LOG(LogAudioPlayer, Warning, TEXT("Setting track end timer for %f seconds"), TimerDuration);

				GetWorld()->GetTimerManager().SetTimer(
					TrackEndTimerHandle,
					this,
					&UMapperWidget::HandleTrackEnd,
					TimerDuration,
					false
				);
			}
		}
	}
}

void UMapperWidget::PausePlayback() {
	if (GetWorld() && TrackEndTimerHandle.IsValid()) {
		GetWorld()->GetTimerManager().PauseTimer(TrackEndTimerHandle);
	}

	if (AudioComponent && AudioComponent->IsValidLowLevel() && AudioComponent->IsPlaying()) {
		AudioComponent->SetPaused(true);
		CurrentAudioState = EAudioState::Paused;
		UpdateButtonLabels();
		UE_LOG(LogAudioPlayer, Warning, TEXT("Playback paused"));
	}
}

void UMapperWidget::ResumePlayback() {
	if (GetWorld() && TrackEndTimerHandle.IsValid()) {
		GetWorld()->GetTimerManager().UnPauseTimer(TrackEndTimerHandle);
	}

	if (AudioComponent && AudioComponent->IsValidLowLevel() && AudioComponent->bIsPaused) {
		AudioComponent->SetPaused(false);
		CurrentAudioState = EAudioState::Playing;
		UpdateButtonLabels();
		UE_LOG(LogAudioPlayer, Warning, TEXT("Playback resumed"));
	}
}

void UMapperWidget::UpdateButtonLabels() const {
	if (!PlayText) {
		return;
	}

	switch (CurrentAudioState) {
	case EAudioState::Stopped:
		PlayText->SetText(FText::FromString("PLAY"));
		break;

	case EAudioState::Playing:
		PlayText->SetText(FText::FromString("PAUSE"));
		break;

	case EAudioState::Paused:
		PlayText->SetText(FText::FromString("RESUME"));
		break;
	}
}

//------------------------------------------------------------------------------
// Native Widget Overrides
//------------------------------------------------------------------------------
void UMapperWidget::NativePreConstruct() {
	Super::NativePreConstruct();

	UWidget* RootWidget = WidgetTree->RootWidget;
	UPanelWidget* RootPanel = Cast<UPanelWidget>(RootWidget);

	if (WidgetTree && ConstructCount == 0) {
		ConstructCount++;
		EditorWorld = GEditor->GetEditorWorldContext().World();
		CreateFolderTextBox(RootPanel);
		CreateThumbnailWidget(RootPanel);
		CreateCurrentSongText(RootPanel);
		CreatePlayButton(RootPanel);
		CreateStopButton(RootPanel);
		CreateNextPrevButtons(RootPanel);
	}
}

void UMapperWidget::NativeConstruct() {
	Super::NativeConstruct();
}

void UMapperWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	bool bCurrentFocusState = false;
	if (const TSharedPtr<SWindow> ActiveWindow = FSlateApplication::Get().GetActiveTopLevelWindow(); ActiveWindow.
		IsValid()) {
		bCurrentFocusState = FSlateApplication::Get().HasFocusedDescendants(ActiveWindow.ToSharedRef());
	}

	if (bCurrentFocusState != bIsEditorFocused) {
		bIsEditorFocused = bCurrentFocusState;
		if (!bIsEditorFocused && AudioComponent && AudioComponent->IsPlaying()) {
			AudioComponent->SetPaused(true);
			PlayText->SetText(FText::FromString("RESUME"));

			if (GetWorld() && TrackEndTimerHandle.IsValid()) {
				GetWorld()->GetTimerManager().PauseTimer(TrackEndTimerHandle);
			}
		} else if (bIsEditorFocused && AudioComponent && AudioComponent->bIsPaused && CurrentAudioState ==
			EAudioState::Playing) {
			AudioComponent->SetPaused(false);
			PlayText->SetText(FText::FromString("PAUSE"));

			if (GetWorld() && TrackEndTimerHandle.IsValid()) {
				GetWorld()->GetTimerManager().UnPauseTimer(TrackEndTimerHandle);
			}
		}
	}
}

void UMapperWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();
}
