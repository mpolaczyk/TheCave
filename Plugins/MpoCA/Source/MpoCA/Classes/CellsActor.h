// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CellsMap.h"
#include "CellsProcessor.h"
#include "Cell.h"

#include "ProceduralMeshComponent.h"

#include "CellsActor.generated.h"

/**
 * 
 */
UCLASS()
class MPOCA_API ACellsActor : public AActor
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TheCave)
	uint8 CellsX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TheCave)
	uint8 CellsY;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TheCave)
	uint8 CellsZ;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TheCave)
	int32 CellsLocOffsetX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TheCave)
	int32 CellsLocOffsetY;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TheCave)
	int32 CellsLocOffsetZ;

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
	uint8 AliveChance;
	

	UFUNCTION(BlueprintCallable, Category = TheCave)
	void ShowCells();

	UFUNCTION(BlueprintCallable, Category = TheCave)
	void ProcessCells();

	UFUNCTION(BlueprintCallable, Category = TheCave)
	void FilterWalls();

	UFUNCTION(BlueprintCallable, Category = TheCave)
	void InitCells();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	UPROPERTY()
	UProceduralMeshComponent* ProceduralMeshComponent = nullptr;

private:
	
	
	TSharedPtr<FCellsMap, ESPMode::ThreadSafe> CellsMap;
	TSharedPtr<FCellsProcessor> CellsProc;
};
