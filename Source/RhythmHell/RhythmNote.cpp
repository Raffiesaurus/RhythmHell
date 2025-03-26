// Raffiesaurus, 2025

#include "RhythmNote.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"

URhythmNote::URhythmNote(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), NoteImage(nullptr), NoteTailImage(nullptr), ResultText(nullptr) {
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

	// if (NoteImage) {
	// }

	if (NoteTailImage) {
		if (HitType == RhythmHitMarking::LONG_START) {
			NoteTailImage->SetVisibility(ESlateVisibility::Visible);
		} else {
			NoteTailImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (HitType == RhythmHitMarking::LONG_START && NoteTailImage) {
		if (UCanvasPanelSlot* TailSlot = Cast<UCanvasPanelSlot>(NoteTailImage->Slot)) {
			TailSlot->SetSize(FVector2D(64.0f, 1.0f));
		}
	}
}

void URhythmNote::SetHitResult(bool bWasHit, const FString& Result) {
	bHasBeenHit = bWasHit;

	if (ResultText && !Result.IsEmpty()) {
		ResultText->SetText(FText::FromString(Result));
		ResultText->SetVisibility(ESlateVisibility::Visible);

		if (bWasHit) {
			PlayHitAnimation();
		} else {
			PlayMissAnimation();
		}
	}
}

void URhythmNote::SetCurrentTime(float InCurrentTime) {
	CurrentTime = InCurrentTime;
}

float URhythmNote::CalculatePosition() {
	const float TotalTravelTime = HitTime - SpawnTime;
	if (TotalTravelTime <= 0.0f) {
		return 1.0f;
	}

	const float ElapsedTime = CurrentTime - SpawnTime;
	const float Progress = FMath::Clamp(ElapsedTime / TotalTravelTime, 0.0f, 1.0f);

	return Progress;
}

void URhythmNote::UpdateNotePosition_Implementation() {
	if (!NoteImage) {
		return;
	}

	UCanvasPanelSlot* NoteSlot = Cast<UCanvasPanelSlot>(NoteImage->Slot);
	if (!NoteSlot) {
		return;
	}

	const float Position = CalculatePosition();
	constexpr float LaneLength = 300.0f;

	const float YPosition = (1.0f - Position) * LaneLength;

	const FVector2D CurrentPos = NoteSlot->GetPosition();
	NoteSlot->SetPosition(FVector2D(CurrentPos.X, YPosition));

	if (bHasBeenHit) {
		const float Alpha = FMath::Clamp(1.0f - ((CurrentTime - HitTime) / 0.5f), 0.0f, 1.0f);
		NoteImage->SetRenderOpacity(Alpha);

		if (ResultText) {
			ResultText->SetRenderOpacity(Alpha);
		}
	}
}

void URhythmNote::UpdateLongNote_Implementation() {
	if (!NoteTailImage || HitType != RhythmHitMarking::LONG_START) {
		return;
	}

	UCanvasPanelSlot* TailSlot = Cast<UCanvasPanelSlot>(NoteTailImage->Slot);
	if (!TailSlot) {
		return;
	}

	if (CurrentTime >= EndTime) {
		if (bHasBeenHit) {
			float Alpha = FMath::Clamp(1.0f - ((CurrentTime - EndTime) / 0.5f), 0.0f, 1.0f);
			NoteTailImage->SetRenderOpacity(Alpha);
		}
		return;
	}

	const float StartPos = FMath::Clamp((HitTime - CurrentTime) / (HitTime - SpawnTime), 0.0f, 1.0f);
	const float EndPos = FMath::Clamp((EndTime - CurrentTime) / (HitTime - SpawnTime), 0.0f, 5.0f);

	constexpr float LaneLength = 300.0f;

	const float TailLength = (EndPos - StartPos) * LaneLength;
	const float TailPosition = StartPos * LaneLength;

	const FVector2D CurrentSize = TailSlot->GetSize();
	TailSlot->SetSize(FVector2D(CurrentSize.X, TailLength));

	const FVector2D CurrentPos = TailSlot->GetPosition();
	TailSlot->SetPosition(FVector2D(CurrentPos.X, TailPosition));

	if (bHasBeenHit && CurrentTime >= HitTime) {
		NoteTailImage->SetColorAndOpacity(FLinearColor(0.0f, 1.0f, 0.3f, 1.0f));
	}
}

void URhythmNote::OnNoteDestroy_Implementation() {
	RemoveFromParent();
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

void URhythmNote::PlayHitAnimation_Implementation() {
	// This is a blueprint implementable function
	// You can override it in Blueprint to play animations
}

void URhythmNote::PlayMissAnimation_Implementation() {
	// This is a blueprint implementable function
	// You can override it in Blueprint to play animations
}
