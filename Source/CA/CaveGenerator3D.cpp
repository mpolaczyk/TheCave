// Fill out your copyright notice in the Description page of Project Settings.

#include "CA.h"
#include "CaveGenerator3D.h"

#include "KismetProceduralMeshLibrary.h"

ACaveGenerator3D::ACaveGenerator3D(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
, CellsX(10)
, CellsY(10)
, CellsZ(10)
, CellsLocOffsetX(40)
, CellsLocOffsetY(40)
, CellsLocOffsetZ(40)
, CellsSizeX(20)
, CellsSizeY(20)
, CellsSizeZ(20)
, DeathLimit(13)
, BirthLimit(15)
, InitialPercentAliveChance(40)
{
	PrimaryActorTick.bCanEverTick = true;
	ProceduralMeshComponent = ObjectInitializer.CreateDefaultSubobject<UProceduralMeshComponent>(this, TEXT("ProceduralMeshComponent"));

}


void ACaveGenerator3D::InitCells()
{
	if (CellsProc.IsValid())
	{
		CellsProc.Reset();
	}
	uint8 cores = (uint8)FWindowsPlatformMisc::NumberOfCoresIncludingHyperthreads();
	CellsProc = MakeShareable(new FCellsProcessor(CellsX, CellsY, CellsZ, cores));

	if (CellsMap.IsValid())
	{
		CellsMap.Reset();
	}
	CellsMap = MakeShareable(new FCellsMap(CellsX, CellsY, CellsZ, InitialPercentAliveChance));

}

void ACaveGenerator3D::ShowCells()
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
			for (uint8 z = 0; z < CellsZ; z++)
			{
				if (CellsMap->IsCellAliveNoIndexCheck(x, y, z))
				{
					V.Reset();
					V.AddUninitialized(24);

					for (int i = 0; i < 24; i++)
					{
						V[i].X = Vertices[i].X + CellsLocOffsetX * x;
						V[i].Y = Vertices[i].Y + CellsLocOffsetY * y;
						V[i].Z = Vertices[i].Z + CellsLocOffsetZ * z;
					}

					ProceduralMeshComponent->CreateMeshSection(CellsIndex, V, Triangles, Normals, UVs, Colors, Tangents, true);
					CellsIndex++;
				}
			}
		}
	}
}

void ACaveGenerator3D::FilterWalls()
{
	CellsProc->ProcessCellsAsync(CellsMap, [this](const FCellsMap* CellsMap, const uint8& CurrentCellX, const uint8& CurrentCellY, const uint8& CurrentCellZ)->bool
	{
		uint8 x = CurrentCellX;
		uint8 y = CurrentCellY;
		uint8 z = CurrentCellZ;

		uint32 n =
			CellsMap->IsCellAlive(x + 1, y + 1, z) +
			CellsMap->IsCellAlive(x + 1, y, z) +
			CellsMap->IsCellAlive(x + 1, y - 1, z) +
			CellsMap->IsCellAlive(x, y + 1, z) +
			CellsMap->IsCellAlive(x, y - 1, z) +
			CellsMap->IsCellAlive(x - 1, y + 1, z) +
			CellsMap->IsCellAlive(x - 1, y, z) +
			CellsMap->IsCellAlive(x - 1, y - 1, z) +

			CellsMap->IsCellAlive(x + 1, y + 1, z + 1) +
			CellsMap->IsCellAlive(x + 1, y, z + 1) +
			CellsMap->IsCellAlive(x + 1, y - 1, z + 1) +
			CellsMap->IsCellAlive(x, y + 1, z + 1) +
			CellsMap->IsCellAlive(x, y, z + 1) +
			CellsMap->IsCellAlive(x, y - 1, z + 1) +
			CellsMap->IsCellAlive(x - 1, y + 1, z + 1) +
			CellsMap->IsCellAlive(x - 1, y, z + 1) +
			CellsMap->IsCellAlive(x - 1, y - 1, z + 1) +

			CellsMap->IsCellAlive(x + 1, y + 1, z - 1) +
			CellsMap->IsCellAlive(x + 1, y, z - 1) +
			CellsMap->IsCellAlive(x + 1, y - 1, z - 1) +
			CellsMap->IsCellAlive(x, y + 1, z - 1) +
			CellsMap->IsCellAlive(x, y, z - 1) +
			CellsMap->IsCellAlive(x, y - 1, z - 1) +
			CellsMap->IsCellAlive(x - 1, y + 1, z - 1) +
			CellsMap->IsCellAlive(x - 1, y, z - 1) +
			CellsMap->IsCellAlive(x - 1, y - 1, z - 1) +
			0;

		if (CellsMap->IsCellAlive(CurrentCellX, CurrentCellY, CurrentCellZ))
		{
			return n < 26;
		}
		else
		{
			return false;
		}
	});
}

void ACaveGenerator3D::ProcessCells()
{
	CellsProc->ProcessCellsAsync(CellsMap, [this](const FCellsMap* CellsMap, const uint8& CurrentCellX, const uint8& CurrentCellY, const uint8& CurrentCellZ)->bool
	{
		uint8 x = CurrentCellX;
		uint8 y = CurrentCellY;
		uint8 z = CurrentCellZ;

		uint32 n =
			CellsMap->IsCellAlive(x + 1, y + 1, z) +
			CellsMap->IsCellAlive(x + 1, y, z) +
			CellsMap->IsCellAlive(x + 1, y - 1, z) +
			CellsMap->IsCellAlive(x, y + 1, z) +
			CellsMap->IsCellAlive(x, y - 1, z) +
			CellsMap->IsCellAlive(x - 1, y + 1, z) +
			CellsMap->IsCellAlive(x - 1, y, z) +
			CellsMap->IsCellAlive(x - 1, y - 1, z) +

			CellsMap->IsCellAlive(x + 1, y + 1, z + 1) +
			CellsMap->IsCellAlive(x + 1, y, z + 1) +
			CellsMap->IsCellAlive(x + 1, y - 1, z + 1) +
			CellsMap->IsCellAlive(x, y + 1, z + 1) +
			CellsMap->IsCellAlive(x, y, z + 1) +
			CellsMap->IsCellAlive(x, y - 1, z + 1) +
			CellsMap->IsCellAlive(x - 1, y + 1, z + 1) +
			CellsMap->IsCellAlive(x - 1, y, z + 1) +
			CellsMap->IsCellAlive(x - 1, y - 1, z + 1) +

			CellsMap->IsCellAlive(x + 1, y + 1, z - 1) +
			CellsMap->IsCellAlive(x + 1, y, z - 1) +
			CellsMap->IsCellAlive(x + 1, y - 1, z - 1) +
			CellsMap->IsCellAlive(x, y + 1, z - 1) +
			CellsMap->IsCellAlive(x, y, z - 1) +
			CellsMap->IsCellAlive(x, y - 1, z - 1) +
			CellsMap->IsCellAlive(x - 1, y + 1, z - 1) +
			CellsMap->IsCellAlive(x - 1, y, z - 1) +
			CellsMap->IsCellAlive(x - 1, y - 1, z - 1) +
			0;

		if (CellsMap->IsCellAlive(CurrentCellX, CurrentCellY, CurrentCellZ))
		{
			return !(n < DeathLimit);
		}
		else
		{
			return n > BirthLimit;
		}
	});
}


