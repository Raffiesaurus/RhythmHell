// Raffiesaurus, 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Components/WidgetComponent.h"
#include "VinylRecord.generated.h"

UCLASS()
class RHYTHMHELL_API AVinylRecord : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVinylRecord();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* VinylMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song Details")
	FString SongName;

	virtual void OnInteract_Implementation(AActor* Interactor) override;

	void Highlight_Implementation(bool bEnable);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* InfoWidgetComponent;

	bool bHighlighted = false;
};
