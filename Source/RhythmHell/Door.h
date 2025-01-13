// Raffiesaurus, 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Components/BoxComponent.h"
#include "Door.generated.h"

UCLASS()
class RHYTHMHELL_API ADoor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	FName LevelToLoad;

	UPROPERTY(VisibleAnywhere, Category = "Trigger")
	UBoxComponent* TriggerBox;
	
	virtual void OnInteract_Implementation(AActor* Interactor) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:
	void DisplyPickupPrompt();
};
