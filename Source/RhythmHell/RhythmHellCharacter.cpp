// Copyright Epic Games, Inc. All Rights Reserved.

#include "RhythmHellCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Interactable.h"
#include "RhythmGameInstance.h"
#include "RhythmInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ARhythmHellCharacter

void ARhythmHellCharacter::EnableRhythmMode(ARhythmGameplayController* GameplayController, const bool bEnable) const {
	if (RhythmInputComponent) {
		RhythmInputComponent->SetGameplayController(GameplayController);
		RhythmInputComponent->SetInputEnabled(bEnable);

		// Optionally disable/enable normal movement while in rhythm mode
		if (bEnable) {
			GetCharacterMovement()->DisableMovement();
		} else {
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
	}
}

ARhythmHellCharacter::ARhythmHellCharacter() {
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	InteractionRange = 200.0f;

	RhythmInputComponent = CreateDefaultSubobject<URhythmInputComponent>(TEXT("RhythmInputComponent"));
}

//////////////////////////////////////////////////////////////////////////
// Input

void ARhythmHellCharacter::NotifyControllerChanged() {
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}	

void ARhythmHellCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARhythmHellCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARhythmHellCharacter::Look);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this,
		                                   &ARhythmHellCharacter::Interact);

		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &ARhythmHellCharacter::Pause);

		if (RhythmInputComponent) {
			RhythmInputComponent->UpAction = RhythmUpAction;
			RhythmInputComponent->DownAction = RhythmDownAction;
			RhythmInputComponent->LeftAction = RhythmLeftAction;
			RhythmInputComponent->RightAction = RhythmRightAction;
			RhythmInputComponent->BindInputActions(EnhancedInputComponent);
		}
	} else {
		UE_LOG(LogTemplateCharacter, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

void ARhythmHellCharacter::BeginPlay() {
	Super::BeginPlay();

	if (VinylHUDClass && GetWorld()->GetName() == "DefaultMap") {
		VinylHUDWidget = CreateWidget<UUserWidget>(GetWorld(), VinylHUDClass);

		if (VinylHUDWidget) {
			VinylHUDWidget->AddToViewport();
		}
	}
}

void ARhythmHellCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	SearchForNearbyInteractables(false);
}

void ARhythmHellCharacter::Interact() {
	SearchForNearbyInteractables(true);
}

void ARhythmHellCharacter::Pause() {
	if (bHasPausedGame) {
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		RemovePauseMenu();
		bHasPausedGame = false;
	} else {
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		ShowPauseMenu();
		bHasPausedGame = true;
	}
}

void ARhythmHellCharacter::AttachVinylToCharacter(AVinylRecord* Vinyl) {
	if (PickedUpVinyl) {
		DetachVinylFromCharacter();
	}

	if (Vinyl && GetMesh()) {
		PickedUpVinyl = Vinyl;
		bIsCarryingVinyl = true;

		if (UPrimitiveComponent* VinylRoot = Cast<UPrimitiveComponent>(PickedUpVinyl->GetRootComponent())) {
			VinylRoot->SetSimulatePhysics(false);
		}

		PickedUpVinyl->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		                                 VinylSocketName);
		PickedUpVinyl->SetActorEnableCollision(false);

		FText VinylName = FText::FromString(*PickedUpVinyl->SongName);
		UE_LOG(LogTemp, Log, TEXT("Picked up a vinyl"));

		if (UFunction* UpdateFunction = VinylHUDWidget->FindFunction(TEXT("SetPickedUpVinyl"))) {
			VinylHUDWidget->ProcessEvent(UpdateFunction, &VinylName);
		}
	}
}

void ARhythmHellCharacter::DetachVinylFromCharacter() {
	if (PickedUpVinyl) {
		bIsCarryingVinyl = false;

		PickedUpVinyl->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		PickedUpVinyl->SetActorEnableCollision(true);

		if (UPrimitiveComponent* VinylRoot = Cast<UPrimitiveComponent>(PickedUpVinyl->GetRootComponent())) {
			VinylRoot->SetSimulatePhysics(true);
		}

		PickedUpVinyl = nullptr;
	}
}

void ARhythmHellCharacter::ShowPauseMenu() {
	if (!PauseMenuWidget) {
		PauseMenuWidget = CreateWidget<UUserWidget>(GetWorld(), PauseMenuWidgetClass);
	}
	PauseMenuWidget->AddToViewport();

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController()) {
		PlayerController->bShowMouseCursor = true;

		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(PauseMenuWidget->TakeWidget());
		PlayerController->SetInputMode(InputMode);
	}
}

void ARhythmHellCharacter::RemovePauseMenu() {
	if (PauseMenuWidget) {
		PauseMenuWidget->RemoveFromParent();
	}

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController()) {
		PlayerController->bShowMouseCursor = false;

		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
	}
}

void ARhythmHellCharacter::StoreAndLoad() const {
	URhythmGameInstance* GameInst = Cast<URhythmGameInstance>(GetGameInstance());
	const FString JSONPath = FPaths::ProjectContentDir() + TEXT("JSONs/") + PickedUpVinyl->SongName.ToLower() +
		TEXT(".json");
	GameInst->SetVinylJSON(JSONPath);
	VinylHUDWidget->RemoveFromParent();
}

void ARhythmHellCharacter::SearchForNearbyInteractables(bool bInteract) {
	const FVector StartPoint = GetActorLocation();
	const FVector EndPoint = StartPoint + (GetActorForwardVector() * InteractionRange);

	const FCollisionShape Sphere = FCollisionShape::MakeSphere(InteractionRange);

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);

	if (TArray<FHitResult> HitResults; GetWorld()->SweepMultiByChannel(HitResults, StartPoint, EndPoint,
	                                                                   FQuat::Identity, ECC_GameTraceChannel1,
	                                                                   Sphere, CollisionQueryParams)) {
		bool bPickedUpVinylThisAction = false;
		AActor* CurrentHighlightedActor = nullptr;
		for (const FHitResult& Hit : HitResults) {
			if (AActor* HitActor = Hit.GetActor()) {
				if (HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass())) {
					IInteractable::Execute_Highlight(HitActor, true);
					CurrentHighlightedActor = HitActor;

					if (bInteract) {
						IInteractable::Execute_OnInteract(HitActor, this);

						// Check if the interacted object is a vinyl record
						if (HitActor->IsA(AVinylRecord::StaticClass()) && !bPickedUpVinylThisAction) {
							bPickedUpVinylThisAction = true;
							AttachVinylToCharacter(Cast<AVinylRecord>(HitActor));
						}
					}
					break;
				}
			}
		}

		if (LastHighlightedActor && LastHighlightedActor != CurrentHighlightedActor) {
			if (LastHighlightedActor->GetClass()->ImplementsInterface(UInteractable::StaticClass())) {
				IInteractable::Execute_Highlight(LastHighlightedActor, false);
			}
		}
		LastHighlightedActor = CurrentHighlightedActor;
	} else {
		if (LastHighlightedActor) {
			if (LastHighlightedActor->GetClass()->ImplementsInterface(UInteractable::StaticClass())) {
				IInteractable::Execute_Highlight(LastHighlightedActor, false);
			}
			LastHighlightedActor = nullptr;
		}
	}


	DrawDebugSphere(GetWorld(), StartPoint, Sphere.GetSphereRadius(), 12, FColor::Red, false, 0.0f);
}

void ARhythmHellCharacter::Move(const FInputActionValue& Value) {
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr) {
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ARhythmHellCharacter::Look(const FInputActionValue& Value) {
	// input is a Vector2D
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr) {
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
