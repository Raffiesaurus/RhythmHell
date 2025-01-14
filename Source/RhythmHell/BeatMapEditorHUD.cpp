// Raffiesaurus, 2025


#include "BeatMapEditorHUD.h"
#include "BeatMapEditor.h"
#include "Engine/Canvas.h"

ABeatMapEditorHUD::ABeatMapEditorHUD() {
	TimelineZoom = 1.0f;
	TimelineScroll = 0.0f;
	SelectedNoteIndex = -1;
}

void ABeatMapEditorHUD::DrawHUD() {
	Super::DrawHUD();
	PlaybackCanvas = Canvas;

	DrawTimeline();
	DrawGrid();
	DrawNotes();

}

void ABeatMapEditorHUD::DrawTimeline() {
	ABeatMapEditor* EditorMode = Cast<ABeatMapEditor>(GetWorld()->GetAuthGameMode());
	if (!EditorMode || !EditorMode->CurrentBeatMap)
		return;

	// Draw timeline background
	float Width = PlaybackCanvas->SizeX * 0.8f;
	float Height = PlaybackCanvas->SizeY * 0.7f;
	float StartX = PlaybackCanvas->SizeX * 0.1f;
	float StartY = PlaybackCanvas->SizeY * 0.15f;

	FLinearColor BackgroundColor(0.1f, 0.1f, 0.1f, 1.0f);
	DrawRect(BackgroundColor, StartX, StartY, Width, Height);

	// Draw playhead
	float PlayheadX = TimeToScreenX(EditorMode->CurrentTime);
	FLinearColor PlayheadColor(1.0f, 1.0f, 1.0f, 1.0f);
	DrawLine(PlayheadX, StartY, PlayheadX, StartY + Height, PlayheadColor);

	if (EditorMode->bIsPlaying) {
		//TimelineScroll = FMath::Clamp(EditorMode->CurrentTime - 0.1f, 0.0f, 999999999.999f);
		//TimelineScroll = FMath::Clamp(PlayheadX - 0.1f, 0.0f, 999999999.999f);
		TimelineScroll = EditorMode->CurrentTime;
		//TimelineScroll += 0.015;
	}
}

void ABeatMapEditorHUD::DrawNotes() {
	ABeatMapEditor* EditorMode = Cast<ABeatMapEditor>(GetWorld()->GetAuthGameMode());
	if (!EditorMode || !EditorMode->CurrentBeatMap)
		return;

	for (int32 i = 0; i < EditorMode->CurrentBeatMap->Notes.Num(); ++i) {
		const FNoteData& Note = EditorMode->CurrentBeatMap->Notes[i];

		float NoteX = TimeToScreenX(Note.TimeStamp);
		float NoteY = PlaybackCanvas->SizeY * (0.15f + (0.175f * Note.Lane));
		float NoteWidth = 20.0f;
		float NoteHeight = PlaybackCanvas->SizeY * 0.15f;

		// Different colors for different note types
		FLinearColor NoteColor;
		if (Note.NoteType == "tap")
			NoteColor = FLinearColor(0.2f, 0.6f, 1.0f, 1.0f);
		else if (Note.NoteType == "hold")
			NoteColor = FLinearColor(1.0f, 0.6f, 0.2f, 1.0f);
		else
			NoteColor = FLinearColor(0.6f, 1.0f, 0.2f, 1.0f);

		// Highlight selected note
		if (i == SelectedNoteIndex) {
			NoteColor *= 1.5f;
			DrawRect(FLinearColor(1.0f, 1.0f, 1.0f, 0.3f),
				NoteX - 2, NoteY - 2,
				NoteWidth + 4, NoteHeight + 4);
		}

		// Draw the note
		DrawRect(NoteColor, NoteX, NoteY, NoteWidth, NoteHeight);

		// Draw hold note duration
		if (Note.Duration > 0.0f) {
			float HoldEndX = TimeToScreenX(Note.TimeStamp + Note.Duration);
			DrawRect(NoteColor * 0.7f,
				NoteX + NoteWidth, NoteY + (NoteHeight * 0.4f),
				HoldEndX - (NoteX + NoteWidth), NoteHeight * 0.2f);
		}
	}
}

void ABeatMapEditorHUD::DrawGrid() {
	ABeatMapEditor* EditorMode = Cast<ABeatMapEditor>(GetWorld()->GetAuthGameMode());
	if (!EditorMode || !EditorMode->CurrentBeatMap)
		return;

	float StartX = PlaybackCanvas->SizeX * 0.1f;
	float StartY = PlaybackCanvas->SizeY * 0.15f;
	float Width = PlaybackCanvas->SizeX * 0.8f;
	float Height = PlaybackCanvas->SizeY * 0.7f;

	// Draw lane dividers
	FLinearColor LaneColor(0.3f, 0.3f, 0.3f, 1.0f);
	for (int32 i = 0; i <= 4; ++i) {
		float Y = StartY + (Height * (i / 4.0f));
		DrawLine(StartX, Y, StartX + Width, Y, LaneColor);
	}

	// Draw beat lines
	float SecondsPerBeat = 60.0f / EditorMode->CurrentBeatMap->BPM;
	float StartTime = ScreenXToTime(StartX);
	float EndTime = ScreenXToTime(StartX + Width);

	for (float Time = FMath::FloorToFloat(StartTime / SecondsPerBeat) * SecondsPerBeat;
		Time <= EndTime;
		Time += SecondsPerBeat) {
		float X = TimeToScreenX(Time);
		FLinearColor BeatColor = FMath::Fmod(Time / SecondsPerBeat, 4.0f) < 0.01f ?
			FLinearColor(0.5f, 0.5f, 0.5f, 1.0f) :  // Major beat
			FLinearColor(0.2f, 0.2f, 0.2f, 1.0f);   // Minor beat

		DrawLine(X, StartY, X, StartY + Height, BeatColor);
	}
}

void ABeatMapEditorHUD::HandleTimelineLeftClick(FVector2D MousePosition) {
	ABeatMapEditor* EditorMode = Cast<ABeatMapEditor>(GetWorld()->GetAuthGameMode());
	if (!EditorMode)
		return;

	// Check if click is within timeline bounds
	float StartX = PlaybackCanvas->SizeX * 0.1f;
	float StartY = PlaybackCanvas->SizeY * 0.15f;
	float Width = PlaybackCanvas->SizeX * 0.8f;
	float Height = PlaybackCanvas->SizeY * 0.7f;

	if (MousePosition.X >= StartX && MousePosition.X <= StartX + Width &&
		MousePosition.Y >= StartY && MousePosition.Y <= StartY + Height) {
		// Check if we clicked on an existing note
		int32 ClickedNote = GetNoteAtPosition(MousePosition);
		if (ClickedNote != -1) {
			SelectedNoteIndex = ClickedNote;
		} else {
			// Add new note
			float ClickTime = ScreenXToTime(MousePosition.X);
			int32 Lane = ScreenYToLane(MousePosition.Y);
			EditorMode->AddNoteAtTime(ClickTime, Lane, 0.0f, "tap");
		}
	}
}

void ABeatMapEditorHUD::HandleTimelineRightClick(FVector2D MousePosition, float ElapsedSeconds) {
	ABeatMapEditor* EditorMode = Cast<ABeatMapEditor>(GetWorld()->GetAuthGameMode());
	if (!EditorMode)
		return;

	// Check if click is within timeline bounds
	float StartX = PlaybackCanvas->SizeX * 0.1f;
	float StartY = PlaybackCanvas->SizeY * 0.15f;
	float Width = PlaybackCanvas->SizeX * 0.8f;
	float Height = PlaybackCanvas->SizeY * 0.7f;
	float DurationPressed = ElapsedSeconds;

	if (MousePosition.X >= StartX && MousePosition.X <= StartX + Width &&
		MousePosition.Y >= StartY && MousePosition.Y <= StartY + Height) {
		// Check if we clicked on an existing note
		int32 ClickedNote = GetNoteAtPosition(MousePosition);
		if (ClickedNote != -1) {
			SelectedNoteIndex = ClickedNote;
		} else {
			// Add new note
			float ClickTime = ScreenXToTime(MousePosition.X);
			int32 Lane = ScreenYToLane(MousePosition.Y);
			EditorMode->AddNoteAtTime(ClickTime, Lane, DurationPressed, "hold");
		}
	}
}

int32 ABeatMapEditorHUD::GetNoteAtPosition(FVector2D Position) {
	ABeatMapEditor* EditorMode = Cast<ABeatMapEditor>(GetWorld()->GetAuthGameMode());
	if (!EditorMode || !EditorMode->CurrentBeatMap)
		return -1;

	float NoteWidth = 20.0f;
	float NoteHeight = PlaybackCanvas->SizeY * 0.15f;

	for (int32 i = 0; i < EditorMode->CurrentBeatMap->Notes.Num(); ++i) {
		const FNoteData& Note = EditorMode->CurrentBeatMap->Notes[i];
		float NoteX = TimeToScreenX(Note.TimeStamp);
		float NoteY = PlaybackCanvas->SizeY * (0.15f + (0.175f * Note.Lane));

		if (Position.X >= NoteX && Position.X <= NoteX + NoteWidth &&
			Position.Y >= NoteY && Position.Y <= NoteY + NoteHeight) {
			return i;
		}
	}

	return -1;
}

float ABeatMapEditorHUD::TimeToScreenX(float Time) {
	return PlaybackCanvas->SizeX * (0.1f + (Time * TimelineZoom - TimelineScroll) * 0.8f);
}

float ABeatMapEditorHUD::ScreenXToTime(float X) {
	return ((X / PlaybackCanvas->SizeX - 0.1f) / 0.8f + TimelineScroll) / TimelineZoom;
}

int32 ABeatMapEditorHUD::ScreenYToLane(float Y) {
	float RelativeY = (Y - PlaybackCanvas->SizeY * 0.15f) / (PlaybackCanvas->SizeY * 0.7f);
	return FMath::Clamp(FMath::FloorToInt(RelativeY * 4), 0, 3);
}