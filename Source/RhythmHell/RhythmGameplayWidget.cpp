#include "RhythmGameplayWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Blueprint/WidgetTree.h"
#include "Animation/WidgetAnimation.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

URhythmGameplayWidget::URhythmGameplayWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), GameplayController(nullptr), UpLanePanel(nullptr), DownLanePanel(nullptr),
	  LeftLanePanel(nullptr),
	  RightLanePanel(nullptr),
	  ScoreText(nullptr), ComboText(nullptr),
	  ProgressBar(nullptr), TotalDuration(0) {
	NoteSpawnLeadTime = 2.0f;
	CurrentTime = 0.0f;
	bIsGameplayActive = false;
	NextSpawnIndex = 0;
}

void URhythmGameplayWidget::NativeConstruct() {
	Super::NativeConstruct();

	if (ScoreText) {
		ScoreText->SetText(FText::FromString("Score: 0"));
	}

	if (ComboText) {
		ComboText->SetText(FText::FromString("Combo: 0"));
	}

	if (ProgressBar) {
		ProgressBar->SetPercent(0.0f);
	}
}

void URhythmGameplayWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bIsGameplayActive || !GameplayController)
		return;

	if (GameplayController->CurrentLevel.Hits.IsValidIndex(NextSpawnIndex)) {
		if (const FRhythmHit& NextHit = GameplayController->CurrentLevel.Hits[NextSpawnIndex]; CurrentTime >= NextHit.
			TimeStamp - NoteSpawnLeadTime) {
			if (URhythmNote* NoteWidget = SpawnNoteWidget(NextHit)) {
				ActiveNotes.Add(NextSpawnIndex, NoteWidget);
			}
			NextSpawnIndex++;
		}
	}

	for (const auto& Pair : ActiveNotes) {
		if (Pair.Value) {
			Pair.Value->SetCurrentTime(CurrentTime);
		}
	}

	UpdateLongNotes();
}

void URhythmGameplayWidget::SetGameplayController(ARhythmGameplayController* InController) {
	GameplayController = InController;

	if (GameplayController) {
		GameplayController->OnRhythmHitResult.AddDynamic(this, &URhythmGameplayWidget::OnRhythmHitResult);
		GameplayController->OnComboUpdate.AddDynamic(this, &URhythmGameplayWidget::OnComboUpdate);
		GameplayController->OnLevelComplete.AddDynamic(this, &URhythmGameplayWidget::OnLevelComplete);
	}
}

void URhythmGameplayWidget::InitializeGameplayUI(const FRhythmLevel& Level) {
	ActiveNotes.Empty();
	CurrentTime = 0.0f;
	NextSpawnIndex = 0;
	bIsGameplayActive = true;

	TotalDuration = GameplayController->AudioComponent->GetSound()->GetDuration();
	GameplayController->AudioComponent->OnAudioPlaybackPercent.AddDynamic(
		this, &URhythmGameplayWidget::OnAudioPlayBackPercent);

	if (ScoreText) {
		ScoreText->SetText(FText::FromString("Score: 0"));
	}

	if (ComboText) {
		ComboText->SetText(FText::FromString("Combo: 0"));
	}

	if (ProgressBar) {
		ProgressBar->SetPercent(0.0f);
	}

	SetupLanes();
}

void URhythmGameplayWidget::ProcessInput(const FString& Direction) {
	if (GameplayController && bIsGameplayActive) {
		GameplayController->ProcessRhythmInput(Direction);

		ShowInputFeedback(Direction);
	}
}

UCanvasPanel* URhythmGameplayWidget::GetLanePanelForDirection(const FString& Direction) {
	if (Direction == "Up")
		return UpLanePanel;
	else if (Direction == "Down")
		return DownLanePanel;
	else if (Direction == "Left")
		return LeftLanePanel;
	else if (Direction == "Right")
		return RightLanePanel;

	return UpLanePanel;
}

URhythmNote* URhythmGameplayWidget::SpawnNoteWidget(const FRhythmHit& Hit) {
	if (!NoteWidgetClass)
		return nullptr;

	UCanvasPanel* LanePanel = GetLanePanelForDirection(Hit.Direction);
	if (!LanePanel)
		return nullptr;

	URhythmNote* NoteWidget = CreateWidget<URhythmNote>(this, NoteWidgetClass);
	if (!NoteWidget)
		return nullptr;

	if (UCanvasPanelSlot* PanelSlot = LanePanel->AddChildToCanvas(NoteWidget)) {
		PanelSlot->SetAlignment(FVector2D(0.5f, 0.0f));
		PanelSlot->SetSize(FVector2D(64.0f, 64.0f));
	}

	NoteWidget->InitializeNote(
		CurrentTime,
		Hit.TimeStamp,
		Hit.Direction,
		Hit.HitType,
		Hit.EndTimeStamp
	);

	return NoteWidget;
}

void URhythmGameplayWidget::UpdateLongNotes() {
	for (auto It = ActiveNotes.CreateIterator(); It; ++It) {
		if (!GameplayController->CurrentLevel.Hits.IsValidIndex(It.Key())) {
			continue;
		}

		if (const FRhythmHit& Hit = GameplayController->CurrentLevel.Hits[It.Key()]; Hit.HitType == RhythmHitMarking::LONG_START) {
			if (URhythmNote* NoteWidget = It.Value()) {
				if (CurrentTime > Hit.EndTimeStamp + GameplayController->GoodHitWindow) {
					It.RemoveCurrent();
				}
			}
		}
	}
}

void URhythmGameplayWidget::OnRhythmHitResult(ERhythmHitResult Result, const FString& Direction) {
	for (auto It = ActiveNotes.CreateIterator(); It; ++It) {
		if (const FRhythmHit& Hit = GameplayController->CurrentLevel.Hits[It.Key()]; Hit.Direction == Direction && FMath::Abs(Hit.TimeStamp - CurrentTime) <= GameplayController->GoodHitWindow *
			1.5f) {
			if (It.Value()) {
				FString ResultText;
				switch (Result) {
				case ERhythmHitResult::PERFECT:
					ResultText = "PERFECT!";
					break;
				case ERhythmHitResult::GOOD:
					ResultText = "GOOD";
					break;
				case ERhythmHitResult::MISS:
					ResultText = "MISS";
					break;
				}

				It.Value()->SetHitResult(Result != ERhythmHitResult::MISS, ResultText);
			}

			if (Hit.HitType != RhythmHitMarking::LONG_START || Result == ERhythmHitResult::MISS) {
				It.RemoveCurrent();
			}

			break;
		}
	}

	if (ScoreText && GameplayController) {
		ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), GameplayController->TotalScore)));
	}

	ShowHitResult(Result, Direction);
}

void URhythmGameplayWidget::OnComboUpdate(int32 ComboCount) {
	if (ComboText) {
		if (ComboCount > 0) {
			ComboText->SetText(FText::FromString(FString::Printf(TEXT("Combo: %d"), ComboCount)));
			ComboText->SetVisibility(ESlateVisibility::Visible);
		} else {
			ComboText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void URhythmGameplayWidget::OnLevelComplete(int32 TotalScore, int32 MaxCombo, const FString& Rank) {
	bIsGameplayActive = false;
	GameplayController->AudioComponent->OnAudioPlaybackPercent.RemoveDynamic(
		this, &URhythmGameplayWidget::OnAudioPlayBackPercent);
	ShowLevelComplete(TotalScore, MaxCombo, Rank);
}

void URhythmGameplayWidget::OnAudioPlayBackPercent(const USoundWave* SoundWave, const float Percentage) {
	if (ProgressBar) {
		ProgressBar->SetPercent(Percentage);
	}
	CurrentTime = (Percentage * TotalDuration);
}

void URhythmGameplayWidget::SetupLanes_Implementation() {
	// Blueprint implementable function to set up lanes based on game settings
	// Default implementation does nothing - override in Blueprint
}

void URhythmGameplayWidget::ShowInputFeedback_Implementation(const FString& Direction) {
	// Blueprint implementable function to show input feedback
	// Default implementation does nothing - override in Blueprint
}

void URhythmGameplayWidget::ShowHitResult_Implementation(ERhythmHitResult Result, const FString& Direction) {
	// Blueprint implementable function to show hit result feedback
	// Default implementation does nothing - override in Blueprint
}

void URhythmGameplayWidget::ShowLevelComplete_Implementation(int32 TotalScore, int32 MaxCombo, const FString& Rank) {
	// Blueprint implementable function to show level complete screen
	// Default implementation does nothing - override in Blueprint
}
