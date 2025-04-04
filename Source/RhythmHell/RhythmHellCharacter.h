// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RhythmGameplayController.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "VinylRecord.h"
#include "RhythmHellCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ARhythmHellCharacter : public ACharacter {
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PauseAction;

	UPROPERTY(VisibleAnywhere, Category = "Rhythm")
	class URhythmInputComponent* RhythmInputComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RhythmUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RhythmDownAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RhythmLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RhythmRightAction;

public:
	ARhythmHellCharacter();

	UFUNCTION(BlueprintCallable, Category = "Rhythm")
	void EnableRhythmMode(ARhythmGameplayController* GameplayController, bool bEnable) const;

protected:
	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void Interact();

	UFUNCTION(BlueprintCallable)
	void Pause();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> VinylHUDClass;

	UPROPERTY()
	UUserWidget* VinylHUDWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Attachments")
	FName VinylSocketName = "VinylSocket";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attachments")
	AVinylRecord* PickedUpVinyl;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	UPROPERTY()
	UUserWidget* PauseMenuWidget;

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	float InteractionRange = 50.0f;

	void AttachVinylToCharacter(AVinylRecord* Vinyl);

	void DetachVinylFromCharacter();

	bool bIsCarryingVinyl = false;

	bool bHasPausedGame = false;

	void ShowPauseMenu();

	void RemovePauseMenu();

	void StoreAndLoad() const;

private:
	void SearchForNearbyInteractables(bool bInteract);

	UPROPERTY()
	AActor* LastHighlightedActor = nullptr;
};
