// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CellsMap.h"
#include "CellsProcessor.h"
#include "Cell.h"

#include "ProceduralMeshComponent.h"

#include "GameFramework/Actor.h"
#include "CaveGenerator2D.generated.h"

UCLASS()
class CA_API ACaveGenerator2D : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TheCave)
	uint8 CellsX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TheCave)
	uint8 CellsY;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TheCave)
	int32 CellsLocOffsetX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TheCave)
	int32 CellsLocOffsetY;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TheCave)
	int32 CellsSizeX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TheCave)
	int32 CellsSizeY;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TheCave)
	int32 CellsSizeZ;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TheCave)
	uint8 BirthLimit;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TheCave)
	uint8 DeathLimit;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TheCave)
	uint8 InitialPercentAliveChance;


	UFUNCTION(BlueprintCallable, Category = TheCave)
	void ProcessCellsAsync();

	UFUNCTION(BlueprintCallable, Category = TheCave)
	void InitCells();

	
	UFUNCTION(BlueprintImplementableEvent,Category = TheCave)
	void ProcessingDone();

	UFUNCTION(BlueprintCallable, Category = TheCave)
	void DrawCellsInProximity(const FVector& WorldLocation);

	FVector WorldToLocal(const FVector& Location);

	FVector LocalToWorld(const FVector& Location);

	/* Begin AActor interface. */
	void BeginPlay() override;
	/* End AActor interface. */

protected:
	UPROPERTY()
	UProceduralMeshComponent* ProceduralMeshComponent = nullptr;

private:

	TSharedPtr<FCellsMap, ESPMode::ThreadSafe> CellsMap;
	TSharedPtr<FCellsProcessor> CellsProc;

	void ProcessingDoneHandler();

	// Name - X and Y coordinates formatted: XXXXYYYY
	TMap<FName, bool> CellDrawn;
	// Can map xy to XXXXYYYY
	FName GetMapName(const uint8& X, const uint8& Y);

	// TODO move this to struct or spawn static mesh
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FColor> Colors;
	TArray<FProcMeshTangent> Tangents;
	int32 CellsIndex;
};
