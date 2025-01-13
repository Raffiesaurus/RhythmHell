// Raffiesaurus, 2025


#include "RhythmMapperBFL.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

bool URhythmMapperBFL::FileSaveString(FString SaveTextB, FString FileNameB) {
	FString filesavepath = *(FPaths::ProjectDir() + FileNameB);
	UE_LOG(LogTemp, Warning, TEXT("Saved at : %s"), *filesavepath);
	return FFileHelper::SaveStringToFile(SaveTextB, *(FPaths::ProjectDir() + FileNameB));
}

bool URhythmMapperBFL::FileLoadString(FString FileNameA, FString& SaveTextA) {
	return FFileHelper::LoadFileToString(SaveTextA, *(FPaths::ProjectDir() + FileNameA));
}
