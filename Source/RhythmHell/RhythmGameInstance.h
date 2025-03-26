// Raffiesaurus, 2025

#pragma once

#include "CoreMinimal.h"
#include "VinylRecord.h"
#include "Engine/GameInstance.h"
#include "GameFramework/SaveGame.h"
#include "RhythmGameInstance.generated.h"

/**
 * Structure to store a player's score for a particular song
 */
USTRUCT(BlueprintType)
struct FSongScore {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Score")
	FString SongName;

	UPROPERTY(BlueprintReadWrite, Category = "Score")
	int32 HighScore = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Score")
	int32 BestCombo = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Score")
	FString BestRank = "F";

	UPROPERTY(BlueprintReadWrite, Category = "Score")
	int32 TimesPlayed = 0;
};

/**
 * Save game class to store all player data
 */
UCLASS()
class RHYTHMHELL_API URhythmSaveGame : public USaveGame {
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Save Data")
	FString PlayerName;

	UPROPERTY(VisibleAnywhere, Category = "Save Data")
	int32 PlayerCoins;

	UPROPERTY(VisibleAnywhere, Category = "Save Data")
	TArray<FString> UnlockedSongs;

	UPROPERTY(VisibleAnywhere, Category = "Save Data")
	TArray<FSongScore> PlayerScores;
};

/**
 * Main game instance class for RhythmHell
 */
UCLASS()
class RHYTHMHELL_API URhythmGameInstance : public UGameInstance {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="Game Data")
	FString VinylJSON;

	UPROPERTY(VisibleAnywhere, Category = "Player Data")
	FString CurrentPlayerName;

	UPROPERTY(VisibleAnywhere, Category = "Player Data")
	int32 PlayerCoins;

	UPROPERTY(VisibleAnywhere, Category = "Player Data")
	TArray<FString> UnlockedSongs;

	UPROPERTY(VisibleAnywhere, Category = "Player Data")
	TArray<FSongScore> PlayerScores;

	UPROPERTY(VisibleAnywhere, Category = "Save System")
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = "Save System")
	int32 UserIndex;

public:
	URhythmGameInstance();

	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category="Game Functions")
	void SetVinylJSON(FString ChosenVinylJSON);

	UFUNCTION(BlueprintCallable, Category="Game Functions")
	FString GetVinylJSON();

	// Score management
	UFUNCTION(BlueprintCallable, Category = "Player Data")
	void SaveLevelScore(const FString& LevelName, int32 Score, int32 MaxCombo, const FString& Rank);

	UFUNCTION(BlueprintCallable, Category = "Player Data")
	int32 GetHighScore(const FString& LevelName);

	UFUNCTION(BlueprintCallable, Category = "Player Data")
	FString GetBestRank(const FString& LevelName);

	// Currency management
	UFUNCTION(BlueprintCallable, Category = "Player Data")
	void AddCoins(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Player Data")
	bool SpendCoins(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Player Data")
	int32 GetPlayerCoins() const { return PlayerCoins; }

	// Song unlocking system
	UFUNCTION(BlueprintCallable, Category = "Player Data")
	void UnlockSong(const FString& SongName);

	UFUNCTION(BlueprintCallable, Category = "Player Data")
	bool IsSongUnlocked(const FString& SongName);

	UFUNCTION(BlueprintCallable, Category = "Player Data")
	TArray<FString> GetAllUnlockedSongs();

	// Player profile
	UFUNCTION(BlueprintCallable, Category = "Player Data")
	void SetPlayerName(const FString& NewName);

	UFUNCTION(BlueprintCallable, Category = "Player Data")
	FString GetPlayerName() const { return CurrentPlayerName; }

	// Save system
	UFUNCTION(BlueprintCallable, Category = "Save System")
	void SaveGameData();

	UFUNCTION(BlueprintCallable, Category = "Save System")
	void LoadSavedGame();

	// Statistics and utilities
	UFUNCTION(BlueprintCallable, Category = "Player Data")
	void ExportPlayerStats();

	UFUNCTION(BlueprintCallable, Category = "Player Data")
	TArray<FSongScore> GetAllScores() const { return PlayerScores; }
};
