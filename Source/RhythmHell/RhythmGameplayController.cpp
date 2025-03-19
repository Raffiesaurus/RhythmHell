// Raffiesaurus, 2025

#include "RhythmGameplayController.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

ARhythmGameplayController::ARhythmGameplayController() {
	PrimaryActorTick.bCanEverTick = true;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	RootComponent = AudioComponent;

	PerfectHitWindow = 0.05f;
	GoodHitWindow = 0.15f;
	NoteSpeed = 1.0f;

	CurrentTime = 0.0f;
	bIsPlaying = false;
	bIsPaused = false;
	NextHitIndex = 0;
	CurrentCombo = 0;
	MaxCombo = 0;
	TotalScore = 0;
}

void ARhythmGameplayController::BeginPlay() {
	Super::BeginPlay();
}

void ARhythmGameplayController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (bIsPlaying && !bIsPaused) {
		CurrentTime += DeltaTime;
		CheckForMissedHits();

		if (NextHitIndex >= CurrentLevel.Hits.Num() && AudioComponent->IsPlaying() == false) {
			EndLevel();
		}
	}
}

bool ARhythmGameplayController::LoadLevelFromJSON(const FString& FilePath) {
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *FilePath)) {
		UE_LOG(LogTemp, Error, TEXT("Failed to load rhythm level file: %s"), *FilePath);
		return false;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("Failed to parse rhythm level JSON: %s"), *FilePath);
		return false;
	}

	CurrentLevel.LevelName = JsonObject->GetStringField(TEXT("levelName"));
	CurrentLevel.AudioPath = JsonObject->GetStringField(TEXT("audioPath"));
	CurrentLevel.BPM = JsonObject->GetNumberField(TEXT("bpm"));
	CurrentLevel.StartOffset = JsonObject->GetNumberField(TEXT("startOffset"));

	FString DifficultyStr = JsonObject->GetStringField(TEXT("difficulty"));
	if (DifficultyStr == "EASY")
		CurrentLevel.LevelDifficulty = Difficulty::EASY;
	else if (DifficultyStr == "MEDIUM")
		CurrentLevel.LevelDifficulty = Difficulty::MEDIUM;
	else if (DifficultyStr == "HARD")
		CurrentLevel.LevelDifficulty = Difficulty::HARD;
	else if (DifficultyStr == "HELL")
		CurrentLevel.LevelDifficulty = Difficulty::HELL;

	CurrentLevel.Hits.Empty();
	const TArray<TSharedPtr<FJsonValue>>* HitsArray;
	if (JsonObject->TryGetArrayField(TEXT("hits"), HitsArray)) {
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

			CurrentLevel.Hits.Add(Hit);
		}
	}

	CurrentLevel.Hits.Sort([](const FRhythmHit& A, const FRhythmHit& B) {
		return A.TimeStamp < B.TimeStamp;
	});

	if (USoundWave* SoundWave = LoadObject<USoundWave>(nullptr, *CurrentLevel.AudioPath)) {
		AudioComponent->SetSound(SoundWave);
	} else {
		UE_LOG(LogTemp, Error, TEXT("Failed to load audio file: %s"), *CurrentLevel.AudioPath);
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Successfully loaded rhythm level: %s"), *CurrentLevel.LevelName);
	return true;
}

void ARhythmGameplayController::StartLevel() {
	CurrentTime = 0.0f;
	NextHitIndex = 0;
	CurrentCombo = 0;
	MaxCombo = 0;
	TotalScore = 0;

	AudioComponent->Play();

	bIsPlaying = true;
	bIsPaused = false;

	UE_LOG(LogTemp, Log, TEXT("Starting rhythm level: %s"), *CurrentLevel.LevelName);
}

void ARhythmGameplayController::PauseLevel() {
	if (bIsPlaying && !bIsPaused) {
		bIsPaused = true;
		AudioComponent->SetPaused(bIsPaused);

		UE_LOG(LogTemp, Log, TEXT("Paused rhythm level"));
	}
}

void ARhythmGameplayController::ResumeLevel() {
	if (bIsPlaying && bIsPaused) {
		bIsPaused = false;
		AudioComponent->SetPaused(bIsPaused);

		UE_LOG(LogTemp, Log, TEXT("Resumed rhythm level"));
	}
}

void ARhythmGameplayController::EndLevel() {
	if (bIsPlaying) {
		bIsPlaying = false;
		AudioComponent->Stop();

		const FString Rank = CalculateRank();

		OnLevelComplete.Broadcast(TotalScore, MaxCombo, Rank);

		UE_LOG(LogTemp, Log, TEXT("Completed rhythm level: %s, Score: %d, Max Combo: %d, Rank: %s"),
		       *CurrentLevel.LevelName, TotalScore, MaxCombo, *Rank);
	}
}

void ARhythmGameplayController::ProcessRhythmInput(const FString& Direction) {
	if (!bIsPlaying || bIsPaused)
		return;

	int32 HitIndex = FindNearestHit(Direction);

	if (HitIndex != INDEX_NONE) {
		const FRhythmHit& Hit = CurrentLevel.Hits[HitIndex];

		const float TimeDifference = FMath::Abs(CurrentTime - Hit.TimeStamp);

		const ERhythmHitResult Result = CalculateHitResult(TimeDifference);

		UpdateScore(Result);

		OnRhythmHitResult.Broadcast(Result, Direction);

		if (Result != ERhythmHitResult::MISS) {
			NextHitIndex = FMath::Max(NextHitIndex, HitIndex + 1);
		}
	} else {
		UpdateScore(ERhythmHitResult::MISS);
		OnRhythmHitResult.Broadcast(ERhythmHitResult::MISS, Direction);
	}
}

int32 ARhythmGameplayController::FindNearestHit(const FString& Direction) {
	for (int32 i = NextHitIndex; i < CurrentLevel.Hits.Num(); ++i) {
		const FRhythmHit& Hit = CurrentLevel.Hits[i];

		if (Hit.Direction == Direction) {
			if (const float TimeDifference = FMath::Abs(CurrentTime - Hit.TimeStamp); TimeDifference <= GoodHitWindow *
				1.5f) {
				return i;
			}

			if (Hit.TimeStamp > CurrentTime + GoodHitWindow * 2.0f) {
				break;
			}
		}
	}

	return INDEX_NONE;
}

ERhythmHitResult ARhythmGameplayController::CalculateHitResult(const float TimeDifference) const {
	if (TimeDifference <= PerfectHitWindow) {
		return ERhythmHitResult::PERFECT;
	} else if (TimeDifference <= GoodHitWindow) {
		return ERhythmHitResult::GOOD;
	} else {
		return ERhythmHitResult::MISS;
	}
}

void ARhythmGameplayController::UpdateScore(ERhythmHitResult Result) {
	switch (Result) {
	case ERhythmHitResult::PERFECT:
		TotalScore += PERFECT_SCORE;
		CurrentCombo++;
		break;

	case ERhythmHitResult::GOOD:
		TotalScore += GOOD_SCORE;
		CurrentCombo++;
		break;

	case ERhythmHitResult::MISS:
		CurrentCombo = 0;
		break;
	}

	if (CurrentCombo > MaxCombo) {
		MaxCombo = CurrentCombo;
	}

	OnComboUpdate.Broadcast(CurrentCombo);
}

FString ARhythmGameplayController::CalculateRank() const {
	const int32 MaxPossibleScore = CurrentLevel.Hits.Num() * PERFECT_SCORE;

	if (const float ScorePercentage = static_cast<float>(TotalScore) / static_cast<float>(MaxPossibleScore);
		ScorePercentage >= 0.95f)
		return TEXT("S");
	else if (ScorePercentage >= 0.9f)
		return TEXT("A");
	else if (ScorePercentage >= 0.8f)
		return TEXT("B");
	else if (ScorePercentage >= 0.7f)
		return TEXT("C");
	else if (ScorePercentage >= 0.6f)
		return TEXT("D");
	else
		return TEXT("F");
}

void ARhythmGameplayController::CheckForMissedHits() {
	while (NextHitIndex < CurrentLevel.Hits.Num()) {
		if (const FRhythmHit& Hit = CurrentLevel.Hits[NextHitIndex]; CurrentTime > Hit.TimeStamp + GoodHitWindow) {
			UpdateScore(ERhythmHitResult::MISS);
			OnRhythmHitResult.Broadcast(ERhythmHitResult::MISS, Hit.Direction);
			NextHitIndex++;
		} else {
			break;
		}
	}
}
