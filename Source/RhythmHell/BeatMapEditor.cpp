// Raffiesaurus, 2025


#include "BeatMapEditor.h"
#include "Kismet/GameplayStatics.h"

ABeatMapEditor::ABeatMapEditor() {
	PrimaryActorTick.bCanEverTick = true;
	PlaybackSpeed = 1.0f;
	CurrentTime = 0.0f;
	bIsPlaying = false;
	GridSnap = 0.25f;
}

void ABeatMapEditor::BeginPlay() {
	Super::BeginPlay();

	if (CurrentBeatMap && CurrentBeatMap->MusicTrack) {
		AudioComponent = UGameplayStatics::CreateSound2D(this, CurrentBeatMap->MusicTrack);
		AudioComponent->SetPaused(true);
	}
}

void ABeatMapEditor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (bIsPlaying) {
		CurrentTime += DeltaTime * PlaybackSpeed;
		UpdateAudioPlayback();
	}
}

void ABeatMapEditor::PlayPause() {
	bIsPlaying = !bIsPlaying;
	if (AudioComponent) {
		if (bIsPlaying) {
			AudioComponent->Play(CurrentTime);
			AudioComponent->SetPitchMultiplier(PlaybackSpeed);
		} else {
			AudioComponent->SetPaused(true);
		}
	}
}

void ABeatMapEditor::Stop() {
	bIsPlaying = false;
	CurrentTime = 0.0f;
	if (AudioComponent) {
		AudioComponent->Stop();
	}
}

void ABeatMapEditor::SeekTo(float Time) {
	CurrentTime = FMath::Max(0.0f, Time);
	if (AudioComponent) {
		AudioComponent->Stop();
		if (bIsPlaying) {
			AudioComponent->Play(CurrentTime);
		}
	}
}

void ABeatMapEditor::SetPlaybackSpeed(float Speed) {
	PlaybackSpeed = FMath::Clamp(Speed, 0.25f, 2.0f);
	if (AudioComponent) {
		AudioComponent->SetPitchMultiplier(PlaybackSpeed);
	}
}

void ABeatMapEditor::AddNoteAtTime(float Time, int32 Lane, float Duration, FString NoteType) {
	if (CurrentBeatMap) {
		float SnappedTime = FMath::RoundToFloat(Time / GridSnap) * GridSnap;
		CurrentBeatMap->AddNote(SnappedTime, Lane, Duration, NoteType);
	}
}

void ABeatMapEditor::DeleteNote(int32 NoteIndex) {
	if (CurrentBeatMap && CurrentBeatMap->Notes.IsValidIndex(NoteIndex)) {
		CurrentBeatMap->Notes.RemoveAt(NoteIndex);
	}
}

void ABeatMapEditor::ModifyNote(int32 NoteIndex, float NewTime, int32 NewLane, float NewDuration) {
	if (CurrentBeatMap && CurrentBeatMap->Notes.IsValidIndex(NoteIndex)) {
		float SnappedTime = FMath::RoundToFloat(NewTime / GridSnap) * GridSnap;
		CurrentBeatMap->Notes[NoteIndex].TimeStamp = SnappedTime;
		CurrentBeatMap->Notes[NoteIndex].Lane = NewLane;
		CurrentBeatMap->Notes[NoteIndex].Duration = NewDuration;
		CurrentBeatMap->SortNotesByTime();
	}
}

void ABeatMapEditor::UpdateAudioPlayback() {
	if (AudioComponent && CurrentBeatMap && CurrentBeatMap->MusicTrack) {
		float Duration = CurrentBeatMap->MusicTrack->Duration;
		if (CurrentTime >= Duration) {
			Stop();
		}
	}
}
