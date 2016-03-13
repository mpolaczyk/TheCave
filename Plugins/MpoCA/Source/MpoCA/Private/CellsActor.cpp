// Fill out your copyright notice in the Description page of Project Settings.

#include "MpoCAPrivatePCH.h"

#include "CellsActor.h"

#include "KismetProceduralMeshLibrary.h"

ACellsActor::ACellsActor(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
, CellsX(20)
, CellsY(20)
, CellsZ(20)
, CellsLocOffsetX(40)
, CellsLocOffsetY(40)
, CellsLocOffsetZ(40)
, CellsSizeX(10)
, CellsSizeY(10)
, CellsSizeZ(10)
, DeathLimit(11)
, BirthLimit(12)
{
	ProceduralMeshComponent = ObjectInitializer.CreateDefaultSubobject<UProceduralMeshComponent>(this, TEXT("ProceduralMeshComponent"));

	//InitCells();
}

#if WITH_EDITOR
void ACellsActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FString Prop = PropertyChangedEvent.Property->GetNameCPP();
	if (Prop.Equals(FString("CellsX")) || Prop.Equals(FString("CellsY")) || Prop.Equals(FString("CellsZ")) || 
		Prop.Equals(FString("CellsLocOffsetX")) || Prop.Equals(FString("CellsLocOffsetY")) || Prop.Equals(FString("CellsLocOffsetZ")))
	{
		//InitCells();
	}
}
#endif

void ACellsActor::InitCells()
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
	CellsMap = MakeShareable(new FCellsMap(CellsX, CellsY, CellsZ, AliveChance));

}

void ACellsActor::ShowCells()
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

void ACellsActor::FilterWalls()
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

void ACellsActor::ProcessCells()
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


