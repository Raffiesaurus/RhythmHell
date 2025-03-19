// Raffiesaurus, 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Helpers.h"
#include "RhythmGameplayController.generated.h"

USTRUCT(BlueprintType)
struct FRhythmHit {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm")
	float TimeStamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm")
	FString Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm")
	RhythmHitMarking HitType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm")
	float EndTimeStamp;

	FRhythmHit()
		: TimeStamp(0.0f)
		  , Direction("Up")
		  , HitType(RhythmHitMarking::BASIC)
		  , EndTimeStamp(0.0f) {}

	FRhythmHit(float InTimeStamp, const FString& InDirection, RhythmHitMarking InHitType = RhythmHitMarking::BASIC,
	           float InEndTimeStamp = 0.0f)
		: TimeStamp(InTimeStamp)
		  , Direction(InDirection)
		  , HitType(InHitType)
		  , EndTimeStamp(InEndTimeStamp) {}
};

USTRUCT(BlueprintType)
struct FRhythmLevel {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm")
	FString LevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm")
	FString AudioPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm")
	float BPM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm")
	float StartOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm")
	TArray<FRhythmHit> Hits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm")
	Difficulty LevelDifficulty;

	FRhythmLevel()
		: LevelName("New Level")
		  , AudioPath("")
		  , BPM(120.0f)
		  , StartOffset(0.0f)
		  , LevelDifficulty(Difficulty::MEDIUM) {}
};

UENUM(BlueprintType)
enum class ERhythmHitResult : uint8 {
	PERFECT = 0 UMETA(DisplayName = "Perfect"),
	GOOD = 1 UMETA(DisplayName = "Good"),
	MISS = 2 UMETA(DisplayName = "Miss")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRhythmHitResultDelegate, ERhythmHitResult, Result, const FString&,
                                             Direction);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComboUpdateDelegate, int32, ComboCount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLevelCompleteDelegate, int32, TotalScore, int32, MaxCombo,
                                               const FString&, Rank);

UCLASS()
class RHYTHMHELL_API ARhythmGameplayController : public AActor {
	GENERATED_BODY()

public:
	ARhythmGameplayController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm")
	FRhythmLevel CurrentLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm Settings")
	float PerfectHitWindow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm Settings")
	float GoodHitWindow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rhythm Settings")
	float NoteSpeed;

	UFUNCTION(BlueprintCallable, Category = "Rhythm")
	bool LoadLevelFromJSON(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "Rhythm")
	void StartLevel();

	UFUNCTION(BlueprintCallable, Category = "Rhythm")
	void PauseLevel();

	UFUNCTION(BlueprintCallable, Category = "Rhythm")
	void ResumeLevel();

	UFUNCTION(BlueprintCallable, Category = "Rhythm")
	void EndLevel();

	UFUNCTION(BlueprintCallable, Category = "Rhythm")
	void ProcessRhythmInput(const FString& Direction);

	UPROPERTY(BlueprintAssignable, Category = "Rhythm Events")
	FOnRhythmHitResultDelegate OnRhythmHitResult;

	UPROPERTY(BlueprintAssignable, Category = "Rhythm Events")
	FOnComboUpdateDelegate OnComboUpdate;

	UPROPERTY(BlueprintAssignable, Category = "Rhythm Events")
	FOnLevelCompleteDelegate OnLevelComplete;

	int32 TotalScore;

private:
	float CurrentTime;

	bool bIsPlaying;
	bool bIsPaused;

	int32 NextHitIndex;
	int32 CurrentCombo;
	int32 MaxCombo;

	const int32 PERFECT_SCORE = 100;
	const int32 GOOD_SCORE = 50;

	int32 FindNearestHit(const FString& Direction);

	ERhythmHitResult CalculateHitResult(float TimeDifference) const;

	void UpdateScore(ERhythmHitResult Result);

	FString CalculateRank() const;

	void CheckForMissedHits();
};
