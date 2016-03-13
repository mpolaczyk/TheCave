// Fill out your copyright notice in the Description page of Project Settings.

#include "CA.h"
#include "CaveGenerator2D.h"

#include "KismetProceduralMeshLibrary.h"

ACaveGenerator2D::ACaveGenerator2D(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CellsX(10)
	, CellsY(10)
	, CellsLocOffsetX(400)
	, CellsLocOffsetY(400)
	, CellsSizeX(200)
	, CellsSizeY(200)
	, CellsSizeZ(200)
	, DeathLimit(3)
	, BirthLimit(4)
	, InitialPercentAliveChance(40)
{
	PrimaryActorTick.bCanEverTick = true;
	ProceduralMeshComponent = ObjectInitializer.CreateDefaultSubobject<UProceduralMeshComponent>(this, TEXT("ProceduralMeshComponent"));
}



void ACaveGenerator2D::InitCells()
{
	if (CellsProc.IsValid())
	{
		CellsProc.Reset();
	}
	uint8 cores = (uint8)FWindowsPlatformMisc::NumberOfCoresIncludingHyperthreads();
	CellsProc = MakeShareable(new FCellsProcessor(CellsX, CellsY, 1, cores));

	if (CellsMap.IsValid())
	{
		CellsMap.Reset();
	}
	CellsMap = MakeShareable(new FCellsMap(CellsX, CellsY, 1, InitialPercentAliveChance));

}



void ACaveGenerator2D::ShowCells()
{
	// Template cube with 24 vertices
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FColor> Colors;
	TArray<FProcMeshTangent> Tangents;
	UKismetProceduralMeshLibrary::GenerateBoxMesh(FVector(CellsSizeX, CellsSizeY, CellsSizeZ), Vertices, Triangles, Normals, UVs, Tangents);

	// Clear previous data
	ProceduralMeshComponent->ClearAllMeshSections();

	// Cube instance, its vertices
	TArray<FVector> V;

	int CellsIndex = 0;
	for (uint8 x = 0; x < CellsX; x++)
	{
		for (uint8 y = 0; y < CellsY; y++)
		{
			if (CellsMap->IsCellAliveNoIndexCheck(x, y, 0))
			{
				V.Reset();
				V.AddUninitialized(24);

				for (int i = 0; i < 24; i++)
				{
					V[i].X = Vertices[i].X + CellsLocOffsetX * x;
					V[i].Y = Vertices[i].Y + CellsLocOffsetY * y;
					V[i].Z = Vertices[i].Z;
				}

				ProceduralMeshComponent->CreateMeshSection(CellsIndex, V, Triangles, Normals, UVs, Colors, Tangents, true);
				CellsIndex++;
			}
		}
	}
}

void ACaveGenerator2D::ProcessCellsAsync()
{
	CellsProc->ProcessingDoneDelegate.BindUObject(this, &ACaveGenerator2D::ProcessingDoneHandler);

	CellsProc->ProcessCellsAsync(CellsMap, [this](const FCellsMap* CellsMap, const uint8& CurrentCellX, const uint8& CurrentCellY, const uint8& CurrentCellZ)->bool
	{
		uint8 x = CurrentCellX;
		uint8 y = CurrentCellY;

		uint32 n =
			CellsMap->IsCellAlive(x + 1, y + 1, 0) +
			CellsMap->IsCellAlive(x + 1, y, 0) +
			CellsMap->IsCellAlive(x + 1, y - 1, 0) +
			CellsMap->IsCellAlive(x, y + 1, 0) +
			CellsMap->IsCellAlive(x, y - 1, 0) +
			CellsMap->IsCellAlive(x - 1, y + 1, 0) +
			CellsMap->IsCellAlive(x - 1, y, 0) +
			CellsMap->IsCellAlive(x - 1, y - 1, 0);

		if (CellsMap->IsCellAlive(CurrentCellX, CurrentCellY, 0))
		{
			return !(n < DeathLimit);
		}
		else
		{
			return n > BirthLimit;
		}
	});
}

void ACaveGenerator2D::ProcessingDoneHandler()
{
	ProcessingDone();
}


