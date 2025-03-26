// Raffiesaurus, 2025

#include "RhythmGameInstance.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Misc/FileHelper.h"

URhythmGameInstance::URhythmGameInstance() {
	PlayerCoins = 0;
	CurrentPlayerName = "Player";
	SaveSlotName = "RhythmHellSave";
	UserIndex = 0;
}

void URhythmGameInstance::Init() {
	Super::Init();

	LoadSavedGame();
}

void URhythmGameInstance::SetVinylJSON(FString ChosenVinylJSON) {
	UE_LOG(LogTemp, Warning, TEXT("Data set: %s"), *ChosenVinylJSON);
	VinylJSON = ChosenVinylJSON;
}

FString URhythmGameInstance::GetVinylJSON() {
	return VinylJSON;
}

void URhythmGameInstance::SaveLevelScore(const FString& LevelName, int32 Score, int32 MaxCombo, const FString& Rank) {
	FSongScore* ExistingScore = PlayerScores.FindByPredicate([&](const FSongScore& InScore) {
		return InScore.SongName == LevelName;
	});

	if (ExistingScore) {
		if (Score > ExistingScore->HighScore) {
			ExistingScore->HighScore = Score;
			ExistingScore->BestCombo = FMath::Max(ExistingScore->BestCombo, MaxCombo);
			ExistingScore->BestRank = Rank;
			ExistingScore->TimesPlayed++;

			UE_LOG(LogTemp, Log, TEXT("Updated high score for %s: %d"), *LevelName, Score);
		} else {
			ExistingScore->TimesPlayed++;
			UE_LOG(LogTemp, Log, TEXT("Score not high enough to update record for %s"), *LevelName);
		}
	} else {
		FSongScore NewScore;
		NewScore.SongName = LevelName;
		NewScore.HighScore = Score;
		NewScore.BestCombo = MaxCombo;
		NewScore.BestRank = Rank;
		NewScore.TimesPlayed = 1;
		PlayerScores.Add(NewScore);

		UE_LOG(LogTemp, Log, TEXT("Added new high score for %s: %d"), *LevelName, Score);
	}

	SaveGameData();
}

int32 URhythmGameInstance::GetHighScore(const FString& LevelName) {
	const FSongScore* Score = PlayerScores.FindByPredicate([&](const FSongScore& InScore) {
		return InScore.SongName == LevelName;
	});

	return Score ? Score->HighScore : 0;
}

FString URhythmGameInstance::GetBestRank(const FString& LevelName) {
	const FSongScore* Score = PlayerScores.FindByPredicate([&](const FSongScore& InScore) {
		return InScore.SongName == LevelName;
	});

	return Score ? Score->BestRank : "N/A";
}

void URhythmGameInstance::AddCoins(int32 Amount) {
	if (Amount > 0) {
		PlayerCoins += Amount;
		UE_LOG(LogTemp, Log, TEXT("Added %d coins, new total: %d"), Amount, PlayerCoins);

		SaveGameData();
	}
}

bool URhythmGameInstance::SpendCoins(int32 Amount) {
	if (Amount <= 0) {
		return false;
	}

	if (PlayerCoins >= Amount) {
		PlayerCoins -= Amount;
		UE_LOG(LogTemp, Log, TEXT("Spent %d coins, remaining: %d"), Amount, PlayerCoins);

		SaveGameData();
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("Not enough coins to spend %d, current: %d"), Amount, PlayerCoins);
	return false;
}

void URhythmGameInstance::UnlockSong(const FString& SongName) {
	if (!UnlockedSongs.Contains(SongName)) {
		UnlockedSongs.Add(SongName);
		UE_LOG(LogTemp, Log, TEXT("Unlocked song: %s"), *SongName);

		SaveGameData();
	}
}

bool URhythmGameInstance::IsSongUnlocked(const FString& SongName) {
	return UnlockedSongs.Contains(SongName);
}

TArray<FString> URhythmGameInstance::GetAllUnlockedSongs() {
	return UnlockedSongs;
}

void URhythmGameInstance::SetPlayerName(const FString& NewName) {
	if (!NewName.IsEmpty()) {
		CurrentPlayerName = NewName;
		UE_LOG(LogTemp, Log, TEXT("Set player name to: %s"), *CurrentPlayerName);

		SaveGameData();
	}
}

void URhythmGameInstance::SaveGameData() {
	if (URhythmSaveGame* SaveGameInstance = Cast<URhythmSaveGame>(
		UGameplayStatics::CreateSaveGameObject(URhythmSaveGame::StaticClass()))) {
		SaveGameInstance->PlayerName = CurrentPlayerName;
		SaveGameInstance->PlayerCoins = PlayerCoins;
		SaveGameInstance->UnlockedSongs = UnlockedSongs;
		SaveGameInstance->PlayerScores = PlayerScores;

		if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, UserIndex)) {
			UE_LOG(LogTemp, Log, TEXT("Game saved successfully"));
		} else {
			UE_LOG(LogTemp, Error, TEXT("Failed to save game"));
		}
	}
}

void URhythmGameInstance::LoadSavedGame() {
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex)) {
		if (URhythmSaveGame* LoadedGame = Cast<URhythmSaveGame>(
			UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex))) {
			CurrentPlayerName = LoadedGame->PlayerName;
			PlayerCoins = LoadedGame->PlayerCoins;
			UnlockedSongs = LoadedGame->UnlockedSongs;
			PlayerScores = LoadedGame->PlayerScores;

			UE_LOG(LogTemp, Log, TEXT("Game loaded successfully for player: %s"), *CurrentPlayerName);
		} else {
			UE_LOG(LogTemp, Error, TEXT("Failed to load game"));
		}
	} else {
		UE_LOG(LogTemp, Log, TEXT("No save game found, using defaults"));
		PlayerCoins = 100;
		CurrentPlayerName = "Player";

		UnlockedSongs.Add("test");

		SaveGameData();
	}
}

void URhythmGameInstance::ExportPlayerStats() {
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	JsonObject->SetStringField("PlayerName", CurrentPlayerName);
	JsonObject->SetNumberField("TotalCoins", PlayerCoins);

	TArray<TSharedPtr<FJsonValue>> SongArray;
	for (const FString& Song : UnlockedSongs) {
		SongArray.Add(MakeShareable(new FJsonValueString(Song)));
	}
	JsonObject->SetArrayField("UnlockedSongs", SongArray);

	TArray<TSharedPtr<FJsonValue>> ScoresArray;
	for (const auto& [SongName, HighScore, BestCombo, BestRank, TimesPlayed] : PlayerScores) {
		const TSharedPtr<FJsonObject> ScoreObject = MakeShareable(new FJsonObject);
		ScoreObject->SetStringField("SongName", SongName);
		ScoreObject->SetNumberField("HighScore", HighScore);
		ScoreObject->SetNumberField("BestCombo", BestCombo);
		ScoreObject->SetStringField("BestRank", BestRank);
		ScoreObject->SetNumberField("TimesPlayed", TimesPlayed);

		ScoresArray.Add(MakeShareable(new FJsonValueObject(ScoreObject)));
	}
	JsonObject->SetArrayField("Scores", ScoresArray);

	FString OutputString;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	if (const FString ExportPath = FPaths::ProjectSavedDir() / TEXT("PlayerStats.json"); FFileHelper::SaveStringToFile(
		OutputString, *ExportPath)) {
		UE_LOG(LogTemp, Log, TEXT("Player stats exported to: %s"), *ExportPath);
	} else {
		UE_LOG(LogTemp, Error, TEXT("Failed to export player stats"));
	}
}
