// Raffiesaurus, 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"
#include "BeatMap.generated.h"

USTRUCT(BlueprintType)
struct FNoteData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeStamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lane;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString NoteType;

	FNoteData()
		: TimeStamp(0.0f)
		, Lane(0)
		, Duration(0.0f)
		, NoteType("tap") {}
};

UCLASS()
class RHYTHMHELL_API ABeatMap : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABeatMap();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beat Map")
	TArray<FNoteData> Notes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beat Map")
	float BPM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beat Map")
	float Offset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beat Map")
	USoundBase* MusicTrack;

	UFUNCTION(BlueprintCallable, Category = "Beat Map")
	void AddNote(float TimeStamp, int32 Lane, float Duration = 0.0f, FString NoteType = "tap");

	UFUNCTION(BlueprintCallable, Category = "Beat Map")
	bool LoadFromJSON(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "Beat Map")
	bool SaveToJSON(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "Beat Map")
	void SortNotesByTime();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
