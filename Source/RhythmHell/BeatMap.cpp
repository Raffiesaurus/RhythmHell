// Raffiesaurus, 2025


#include "BeatMap.h"
#include "JsonObjectConverter.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"

ABeatMap::ABeatMap() {
	PrimaryActorTick.bCanEverTick = false;
	BPM = 120.0f;
	Offset = 0.0f;
}

void ABeatMap::AddNote(float TimeStamp, int32 Lane, float Duration, FString NoteType) {
	FNoteData NewNote;
	NewNote.TimeStamp = TimeStamp;
	NewNote.Lane = Lane;
	NewNote.Duration = Duration;
	NewNote.NoteType = NoteType;

	Notes.Add(NewNote);
	SortNotesByTime();
}

void ABeatMap::SortNotesByTime() {
	Notes.Sort([](const FNoteData& A, const FNoteData& B) {
		return A.TimeStamp < B.TimeStamp;
		});
}

bool ABeatMap::SaveToJSON(const FString& FilePath) {
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetNumberField("bpm", BPM);
	JsonObject->SetNumberField("offset", Offset);

	TArray<TSharedPtr<FJsonValue>> NotesArray;
	for (const FNoteData& Note : Notes) {
		TSharedPtr<FJsonObject> NoteObject = MakeShared<FJsonObject>();
		NoteObject->SetNumberField("timestamp", Note.TimeStamp);
		NoteObject->SetNumberField("lane", Note.Lane);
		NoteObject->SetNumberField("duration", Note.Duration);
		NoteObject->SetStringField("type", Note.NoteType);
		NotesArray.Add(MakeShared<FJsonValueObject>(NoteObject));
	}
	JsonObject->SetArrayField("notes", NotesArray);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	return FFileHelper::SaveStringToFile(OutputString, *FilePath);
}

bool ABeatMap::LoadFromJSON(const FString& FilePath) {
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *FilePath)) {
		return false;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject)) {
		return false;
	}

	BPM = JsonObject->GetNumberField(FString("bpm"));
	Offset = JsonObject->GetNumberField(FString("offset"));

	Notes.Empty();
	TArray<TSharedPtr<FJsonValue>> NotesArray = JsonObject->GetArrayField(FString("notes"));
	for (const TSharedPtr<FJsonValue>& Value : NotesArray) {
		TSharedPtr<FJsonObject> NoteObject = Value->AsObject();
		FNoteData Note;
		Note.TimeStamp = NoteObject->GetNumberField(FString("timestamp"));
		Note.Lane = NoteObject->GetNumberField(FString("lane"));
		Note.Duration = NoteObject->GetNumberField(FString("duration"));
		Note.NoteType = NoteObject->GetStringField(FString("type"));
		Notes.Add(Note);
	}

	SortNotesByTime();
	return true;
}

// Called when the game starts or when spawned
void ABeatMap::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void ABeatMap::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

