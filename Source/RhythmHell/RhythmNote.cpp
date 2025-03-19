// Raffiesaurus, 2025

#include "RhythmNote.h"
#include "Components/TextBlock.h"

URhythmNote::URhythmNote(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	SpawnTime = 0.0f;
	HitTime = 0.0f;
	EndTime = 0.0f;
	Direction = "Up";
	HitType = RhythmHitMarking::BASIC;
	CurrentTime = 0.0f;
	bHasBeenHit = false;
}

void URhythmNote::NativeConstruct() {
	Super::NativeConstruct();

	if (ResultText) {
		ResultText->SetVisibility(ESlateVisibility::Hidden);
	}

	if (NoteTailImage && HitType != RhythmHitMarking::LONG_START) {
		NoteTailImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void URhythmNote::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);


	UpdateNotePosition();


	if (HitType == RhythmHitMarking::LONG_START) {
		UpdateLongNote();
	}

	if (ShouldDestroyNote()) {
		OnNoteDestroy();
	}
}

void URhythmNote::InitializeNote(float InSpawnTime, float InHitTime, const FString& InDirection,
                                 RhythmHitMarking InHitType, float InEndTime) {
	SpawnTime = InSpawnTime;
	HitTime = InHitTime;
	Direction = InDirection;
	HitType = InHitType;
	EndTime = InEndTime;
	bHasBeenHit = false;

	if (NoteImage) {
		// You can set different images based on the direction in Blueprint
	}

	if (NoteTailImage) {
		if (HitType == RhythmHitMarking::LONG_START) {
			NoteTailImage->SetVisibility(ESlateVisibility::Visible);
		} else {
			NoteTailImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void URhythmNote::SetHitResult(bool bWasHit, const FString& Result) {
	bHasBeenHit = bWasHit;

	if (ResultText && !Result.IsEmpty()) {
		ResultText->SetText(FText::FromString(Result));
		ResultText->SetVisibility(ESlateVisibility::Visible);
	}
}

void URhythmNote::SetCurrentTime(float InCurrentTime) {
	CurrentTime = InCurrentTime;
}

float URhythmNote::CalculatePosition() const {
	const float TotalTravelTime = HitTime - SpawnTime;
	if (TotalTravelTime <= 0.0f) {
		return 1.0f;
	}

	const float ElapsedTime = CurrentTime - SpawnTime;
	const float Progress = FMath::Clamp(ElapsedTime / TotalTravelTime, 0.0f, 1.0f);

	return Progress;
}

bool URhythmNote::ShouldDestroyNote() const {
	if (bHasBeenHit) {
		return (CurrentTime > HitTime + 0.5f);
	}

	if (HitType == RhythmHitMarking::BASIC) {
		return (CurrentTime > HitTime + 0.5f);
	} else if (HitType == RhythmHitMarking::LONG_START) {
		return (CurrentTime > EndTime + 0.5f);
	}

	return false;
}
