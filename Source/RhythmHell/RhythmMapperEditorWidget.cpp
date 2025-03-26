// Raffiesaurus, 2025

#include "RhythmMapperEditorWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Misc/FileHelper.h"

URhythmMapperEditorWidget::URhythmMapperEditorWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), AudioComponent(nullptr), SongAsset(nullptr), CompletedPercentage(0), TotalDuration(0) {
	BPM = 120.0f;
	StartOffset = 0.0f;
	LevelName = "New Level";
	LevelDifficulty = Difficulty::MEDIUM;
	CurrentTime = 0.0f;
	bIsPlaying = false;
}

void URhythmMapperEditorWidget::NativeConstruct() {
	Super::NativeConstruct();

	if (!AudioComponent->IsRegistered()) {
		AudioComponent->RegisterComponent();
	}
}

void URhythmMapperEditorWidget::NativeDestruct() {
	Super::NativeDestruct();

	if (bIsPlaying) {
		StopSong();
	}
}

void URhythmMapperEditorWidget::LoadSong(USoundWave* NewSongAsset) {
	StopSong();

	SongAsset = NewSongAsset;

	AudioComponent = UGameplayStatics::CreateSound2D(GetWorld(), SongAsset);
	AudioComponent->OnAudioPlaybackPercent.RemoveDynamic(this, &URhythmMapperEditorWidget::UpdateCurrentSongTime);
	AudioComponent->OnAudioPlaybackPercent.AddDynamic(this, &URhythmMapperEditorWidget::UpdateCurrentSongTime);
	AudioComponent->Stop();

	if (SongAsset && AudioComponent) {
		AudioComponent->SetSound(SongAsset);
		SongName = SongAsset->GetName();
		UE_LOG(LogTemp, Log, TEXT("Loaded song: %s"), *SongAsset->GetName());
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Failed to load song"));
	}
}

void URhythmMapperEditorWidget::PlaySong() {
	if (!IsSongLoaded()) {
		UE_LOG(LogTemp, Error, TEXT("Song's not loaded."))
		return;
	}

	if (bIsPlaying) {
		return;
	}

	AudioComponent->SetPaused(false);
	AudioComponent->Play();
	bIsPlaying = true;
	TotalDuration = AudioComponent->GetSound()->GetDuration();

	UE_LOG(LogTemp, Log, TEXT("Playing song: %f"), TotalDuration);
}

void URhythmMapperEditorWidget::PauseSong() {
	if (!bIsPlaying)
		return;

	AudioComponent->SetPaused(true);
	bIsPlaying = false;

	UE_LOG(LogTemp, Log, TEXT("Paused song at %.2f seconds"), CurrentTime);
}

void URhythmMapperEditorWidget::ResumeSong() {
	if (bIsPlaying)
		return;

	AudioComponent->SetPaused(false);
	bIsPlaying = true;

	UE_LOG(LogTemp, Log, TEXT("Resumed song at %.2f seconds"), CurrentTime);
}

void URhythmMapperEditorWidget::StopSong() {
	if (!IsSongLoaded())
		return;

	AudioComponent->OnAudioPlaybackPercent.RemoveDynamic(this, &URhythmMapperEditorWidget::UpdateCurrentSongTime);
	AudioComponent->Stop();
	bIsPlaying = false;
	CurrentTime = 0.0f;
	CompletedPercentage = 0.0f;

	UE_LOG(LogTemp, Log, TEXT("Stopped song"));
}

void URhythmMapperEditorWidget::AddHit(const FString& Direction, RhythmHitMarking HitType) {
	FRhythmHit Hit;
	Hit.TimeStamp = CurrentTime;
	Hit.Direction = Direction;
	Hit.HitType = HitType;

	Hits.Add(Hit);

	SortHits();

	UE_LOG(LogTemp, Log, TEXT("Added %s hit at %.2f seconds"), *Direction, CurrentTime);
}

void URhythmMapperEditorWidget::StartLongHit(const FString& Direction) {
	if (LongHitsInProgress.Contains(Direction)) {
		UE_LOG(LogTemp, Warning, TEXT("Long hit already in progress for %s direction"), *Direction);
		return;
	}

	LongHitsInProgress.Add(Direction, CurrentTime);

	UE_LOG(LogTemp, Log, TEXT("Started long %s hit at %.2f seconds"), *Direction, CurrentTime);
}

void URhythmMapperEditorWidget::EndLongHit(const FString& Direction) {
	if (!LongHitsInProgress.Contains(Direction)) {
		UE_LOG(LogTemp, Warning, TEXT("No long hit in progress for %s direction"), *Direction);
		return;
	}

	const float StartTime = LongHitsInProgress[Direction];

	LongHitsInProgress.Remove(Direction);

	FRhythmHit Hit;
	Hit.TimeStamp = StartTime;
	Hit.Direction = Direction;
	Hit.HitType = RhythmHitMarking::LONG_START;
	Hit.EndTimeStamp = CurrentTime;

	Hits.Add(Hit);

	SortHits();

	UE_LOG(LogTemp, Log, TEXT("Ended long %s hit at %.2f seconds (duration: %.2f)"),
	       *Direction, CurrentTime, CurrentTime - StartTime);
}

void URhythmMapperEditorWidget::RemoveHit(int32 Index) {
	if (Hits.IsValidIndex(Index)) {
		const FRhythmHit Hit = Hits[Index];

		Hits.RemoveAt(Index);

		UE_LOG(LogTemp, Log, TEXT("Removed %s hit at %.2f seconds"), *Hit.Direction, Hit.TimeStamp);
	}
}

void URhythmMapperEditorWidget::ClearAllHits() {
	const int32 NumHits = Hits.Num();

	Hits.Empty();

	LongHitsInProgress.Empty();

	UE_LOG(LogTemp, Log, TEXT("Cleared all hits (%d total)"), NumHits);
}

bool URhythmMapperEditorWidget::SaveLevelToJSON(const FString& FileName) {
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	JsonObject->SetStringField("levelName", LevelName);
	JsonObject->SetStringField("audioPath", SongAsset ? SongAsset->GetPathName() : "");
	JsonObject->SetNumberField("bpm", BPM);
	JsonObject->SetNumberField("startOffset", StartOffset);

	FString DifficultyStr;
	switch (LevelDifficulty) {
	case Difficulty::EASY:
		DifficultyStr = "EASY";
		break;
	case Difficulty::MEDIUM:
		DifficultyStr = "MEDIUM";
		break;
	case Difficulty::HARD:
		DifficultyStr = "HARD";
		break;
	case Difficulty::HELL:
		DifficultyStr = "HELL";
		break;
	}
	JsonObject->SetStringField("difficulty", DifficultyStr);

	TArray<TSharedPtr<FJsonValue>> HitsArray;
	for (const FRhythmHit& Hit : Hits) {
		const TSharedPtr<FJsonObject> HitObject = MakeShareable(new FJsonObject);

		HitObject->SetNumberField("timestamp", Hit.TimeStamp);
		HitObject->SetStringField("direction", Hit.Direction);

		FString HitTypeStr;
		switch (Hit.HitType) {
		case RhythmHitMarking::BASIC:
			HitTypeStr = "BASIC";
			break;
		case RhythmHitMarking::LONG_START:
			HitTypeStr = "LONG_START";
			break;
		case RhythmHitMarking::LONG_END:
			HitTypeStr = "LONG_END";
			break;
		}
		HitObject->SetStringField("hitType", HitTypeStr);

		if (Hit.HitType == RhythmHitMarking::LONG_START) {
			HitObject->SetNumberField("endTimestamp", Hit.EndTimeStamp);
		}

		HitsArray.Add(MakeShareable(new FJsonValueObject(HitObject)));
	}
	JsonObject->SetArrayField("hits", HitsArray);

	FString OutputString;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	if (const FString FileLocation = FPaths::ProjectContentDir() + TEXT("JSONs/") + FileName + TEXT(".json");
		FFileHelper::SaveStringToFile(OutputString, *FileLocation)) {
		UE_LOG(LogTemp, Log, TEXT("Saved level to %s"), *FileLocation);
		return true;
	} else {
		UE_LOG(LogTemp, Error, TEXT("Failed to save level to %s"), *FileLocation);
		return false;
	}
}

void URhythmMapperEditorWidget::AutoPlaceHits(int32 BeatsPerMeasure, int32 NumMeasures) {
	if (BPM <= 0.0f) {
		UE_LOG(LogTemp, Warning, TEXT("Invalid BPM: %.2f"), BPM);
		return;
	}

	ClearAllHits();

	const float BeatDuration = 60.0f / BPM;

	const int32 TotalBeats = BeatsPerMeasure * NumMeasures;

	TArray<FString> Directions = {"Up", "Down", "Left", "Right"};

	for (int32 Beat = 0; Beat < TotalBeats; Beat++) {
		float Time = StartOffset + (Beat * BeatDuration);

		FString Direction = Directions[Beat % Directions.Num()];

		FRhythmHit Hit;
		Hit.TimeStamp = Time;
		Hit.Direction = Direction;
		Hit.HitType = RhythmHitMarking::BASIC;

		Hits.Add(Hit);
	}

	SortHits();

	UE_LOG(LogTemp, Log, TEXT("Auto-placed %d hits over %d measures"), TotalBeats, NumMeasures);
}

void URhythmMapperEditorWidget::QuantizeHits() {
	if (BPM <= 0.0f) {
		UE_LOG(LogTemp, Warning, TEXT("Invalid BPM: %.2f"), BPM);
		return;
	}

	const float BeatDuration = 60.0f / BPM;

	for (FRhythmHit& Hit : Hits) {
		const float AdjustedTime = Hit.TimeStamp - StartOffset;

		const int32 NearestBeat = FMath::RoundToInt(AdjustedTime / BeatDuration);

		Hit.TimeStamp = StartOffset + (NearestBeat * BeatDuration);

		if (Hit.HitType == RhythmHitMarking::LONG_START) {
			const float AdjustedEndTime = Hit.EndTimeStamp - StartOffset;

			const int32 NearestEndBeat = FMath::RoundToInt(AdjustedEndTime / BeatDuration);

			Hit.EndTimeStamp = StartOffset + (NearestEndBeat * BeatDuration);
		}
	}

	SortHits();

	UE_LOG(LogTemp, Log, TEXT("Quantized %d hits"), Hits.Num());
}

TArray<FRhythmHit> URhythmMapperEditorWidget::GetHitsInTimeRange(float StartTime, float EndTime) {
	TArray<FRhythmHit> HitsInRange;

	for (const FRhythmHit& Hit : Hits) {
		if (Hit.TimeStamp >= StartTime && Hit.TimeStamp <= EndTime) {
			HitsInRange.Add(Hit);
		} else if (Hit.HitType == RhythmHitMarking::LONG_START && Hit.EndTimeStamp >= StartTime && Hit.TimeStamp <=
			EndTime) {
			HitsInRange.Add(Hit);
		}
	}

	return HitsInRange;
}

void URhythmMapperEditorWidget::
UpdateCurrentSongTime(const USoundWave* PlayingSoundWave, const float PlaybackPercent) {
	CurrentTime = PlaybackPercent * TotalDuration;
	CompletedPercentage = PlaybackPercent;
}

void URhythmMapperEditorWidget::SortHits() {
	Hits.Sort([](const FRhythmHit& A, const FRhythmHit& B) {
		return A.TimeStamp < B.TimeStamp;
	});
}

bool URhythmMapperEditorWidget::IsSongLoaded() const {
	return (SongAsset != nullptr && AudioComponent != nullptr);
}

bool URhythmMapperEditorWidget::LoadLevelFromJSON(const FString& FileName) {
	FString JsonString;
	const FString FullPath = FPaths::ProjectContentDir() + TEXT("JSONs/") + FileName + TEXT(".json");
	if (!FFileHelper::LoadFileToString(JsonString, *FullPath)) {
		UE_LOG(LogTemp, Error, TEXT("Failed to load rhythm level file: %s"), *FullPath);
		return false;
	}

	TSharedPtr<FJsonObject> JsonObject;
	if (const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString); !
		FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("Failed to parse rhythm level JSON: %s"), *FullPath);
		return false;
	}

	ClearAllHits();

	LevelName = JsonObject->GetStringField(TEXT("levelName"));

	if (const FString AudioPath = JsonObject->GetStringField(TEXT("audioPath")); !AudioPath.IsEmpty()) {
		SongAsset = LoadObject<USoundWave>(nullptr, *AudioPath);
		if (SongAsset) {
			LoadSong(SongAsset);
		}
	}

	BPM = JsonObject->GetNumberField(TEXT("bpm"));
	StartOffset = JsonObject->GetNumberField(TEXT("startOffset"));

	if (const FString DifficultyStr = JsonObject->GetStringField(TEXT("difficulty")); DifficultyStr == "EASY")
		LevelDifficulty = Difficulty::EASY;
	else if (DifficultyStr == "MEDIUM")
		LevelDifficulty = Difficulty::MEDIUM;
	else if (DifficultyStr == "HARD")
		LevelDifficulty = Difficulty::HARD;
	else if (DifficultyStr == "HELL")
		LevelDifficulty = Difficulty::HELL;

	if (const TArray<TSharedPtr<FJsonValue>>* HitsArray; JsonObject->TryGetArrayField(TEXT("hits"), HitsArray)) {
		for (const TSharedPtr<FJsonValue>& HitValue : *HitsArray) {
			const TSharedPtr<FJsonObject>& HitObject = HitValue->AsObject();

			FRhythmHit Hit;
			Hit.TimeStamp = HitObject->GetNumberField(TEXT("timestamp"));
			Hit.Direction = HitObject->GetStringField(TEXT("direction"));

			if (FString HitTypeStr = HitObject->GetStringField(TEXT("hitType")); HitTypeStr == "BASIC")
				Hit.HitType = RhythmHitMarking::BASIC;
			else if (HitTypeStr == "LONG_START")
				Hit.HitType = RhythmHitMarking::LONG_START;
			else if (HitTypeStr == "LONG_END")
				Hit.HitType = RhythmHitMarking::LONG_END;

			if (Hit.HitType == RhythmHitMarking::LONG_START) {
				Hit.EndTimeStamp = HitObject->GetNumberField(TEXT("endTimestamp"));
			}

			Hits.Add(Hit);
		}
	}

	SortHits();

	UE_LOG(LogTemp, Log, TEXT("Loaded level from %s with %d hits"), *FullPath, Hits.Num());
	return true;
}
