// Raffiesaurus, 2025

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Helpers.h"
#include "RhythmNote.generated.h"

UCLASS()
class RHYTHMHELL_API URhythmNote : public UUserWidget {
	GENERATED_BODY()

public:
	URhythmNote(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Rhythm Note")
	void InitializeNote(float InSpawnTime, float InHitTime, const FString& InDirection,
	                    RhythmHitMarking InHitType = RhythmHitMarking::BASIC, float InEndTime = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Rhythm Note")
	float CalculatePosition();

	UFUNCTION(BlueprintCallable, Category = "Rhythm Note")
	void SetHitResult(bool bWasHit, const FString& ResultText = "");

	UFUNCTION(BlueprintCallable, Category = "Rhythm Note")
	void SetCurrentTime(float InCurrentTime);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* NoteImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* NoteTailImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ResultText;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Rhythm Note")
	float SpawnTime;

	UPROPERTY(BlueprintReadOnly, Category = "Rhythm Note")
	float HitTime;

	UPROPERTY(BlueprintReadOnly, Category = "Rhythm Note")
	float EndTime;

	UPROPERTY(BlueprintReadOnly, Category = "Rhythm Note")
	FString Direction;

	UPROPERTY(BlueprintReadOnly, Category = "Rhythm Note")
	RhythmHitMarking HitType;

	UPROPERTY(BlueprintReadOnly, Category = "Rhythm Note")
	float CurrentTime;

	UPROPERTY(BlueprintReadOnly, Category = "Rhythm Note")
	bool bHasBeenHit;

	UFUNCTION(BlueprintNativeEvent, Category = "Rhythm Note")
	void UpdateNotePosition();
	virtual void UpdateNotePosition_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Rhythm Note")
	void UpdateLongNote();
	virtual void UpdateLongNote_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Rhythm Note")
	void OnNoteDestroy();
	virtual void OnNoteDestroy_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Rhythm Note")
	void PlayHitAnimation();
	virtual void PlayHitAnimation_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Rhythm Note")
	void PlayMissAnimation();
	virtual void PlayMissAnimation_Implementation();

	bool ShouldDestroyNote() const;
};