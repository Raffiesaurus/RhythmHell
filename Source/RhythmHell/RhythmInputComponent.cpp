// Raffiesaurus, 2025

#include "RhythmInputComponent.h"
#include "EnhancedInputComponent.h"

URhythmInputComponent::URhythmInputComponent(): GameplayController(nullptr), UpAction(nullptr), DownAction(nullptr),
                                                LeftAction(nullptr),
                                                RightAction(nullptr) {
	PrimaryComponentTick.bCanEverTick = true;
	bIsInputEnabled = false;
}

void URhythmInputComponent::BeginPlay() {
	Super::BeginPlay();
}

void URhythmInputComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void URhythmInputComponent::BindInputActions(UEnhancedInputComponent* EnhancedInputComponent) {
	if (EnhancedInputComponent) {
		if (UpAction) {
			EnhancedInputComponent->BindAction(UpAction, ETriggerEvent::Started, this,
			                                   &URhythmInputComponent::HandleUpInput);
		}

		if (DownAction) {
			EnhancedInputComponent->BindAction(DownAction, ETriggerEvent::Started, this,
			                                   &URhythmInputComponent::HandleDownInput);
		}

		if (LeftAction) {
			EnhancedInputComponent->BindAction(LeftAction, ETriggerEvent::Started, this,
			                                   &URhythmInputComponent::HandleLeftInput);
		}

		if (RightAction) {
			EnhancedInputComponent->BindAction(RightAction, ETriggerEvent::Started, this,
			                                   &URhythmInputComponent::HandleRightInput);
		}
	}
}

void URhythmInputComponent::SetInputEnabled(bool bEnabled) {
	bIsInputEnabled = bEnabled;
}

void URhythmInputComponent::SetGameplayController(ARhythmGameplayController* InController) {
	GameplayController = InController;
}

void URhythmInputComponent::HandleUpInput(const FInputActionValue& Value) {
	ProcessRhythmInput("Up");
}

void URhythmInputComponent::HandleDownInput(const FInputActionValue& Value) {
	ProcessRhythmInput("Down");
}

void URhythmInputComponent::HandleLeftInput(const FInputActionValue& Value) {
	ProcessRhythmInput("Left");
}

void URhythmInputComponent::HandleRightInput(const FInputActionValue& Value) {
	ProcessRhythmInput("Right");
}

void URhythmInputComponent::ProcessRhythmInput(const FString& Direction) {
	if (bIsInputEnabled && GameplayController) {		GameplayController->ProcessRhythmInput(Direction);
	}
}
