// Fill out your copyright notice in the Description page of Project Settings.

#include "CA.h"
#include "CaveGenerator2D.h"

#include "KismetProceduralMeshLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

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

void ACaveGenerator2D::BeginPlay()
{
	Super::BeginPlay();

	UKismetProceduralMeshLibrary::GenerateBoxMesh(FVector(CellsSizeX, CellsSizeY, CellsSizeZ), Vertices, Triangles, Normals, UVs, Tangents);
	CellsIndex = 0;
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

FVector ACaveGenerator2D::WorldToLocal(const FVector& Location)
{
	FVector locdiff = Location - GetActorLocation();
	FRotator rot = GetActorRotation();
	return rot.RotateVector(locdiff);
}

FVector ACaveGenerator2D::LocalToWorld(const FVector& Location)
{
	// TODO rotation
	return Location + GetActorLocation();
	
}

void ACaveGenerator2D::DrawCellsInProximity(const FVector& WorldLocation)
{
	// Convert to local space
	FVector Loc = WorldToLocal(WorldLocation);

	// TODO debug flag
	// Draw debug figure
	//UKismetSystemLibrary::DrawDebugSphere(GWorld->GetWorld(), WorldLocation, 50, 10, FColor(0, 255, 255, 255), 10);

	// Find X and Y coordinates in cells map
	uint8 cx = (uint8)(Loc.X / CellsLocOffsetX);
	uint8 cy = (uint8)(Loc.Y / CellsLocOffsetY);

	// Check all neighbours if were drawn and draw
	for (int x = -3; x <= 3; x++) // TODO parameter
	{
		for (int y = -3; y <= 3; y++)
		{
			FName cell = GetMapName(cx+x, cy+y);
			
			if (CellsMap->CellIndexCheck(cx+x, cy+y, 0))
			{
				if(CellsMap->IsCellAliveNoIndexCheck(cx+x, cy+y, 0))
				{
					UKismetSystemLibrary::DrawDebugSphere(GWorld->GetWorld(), LocalToWorld(FVector((cx+x)*CellsLocOffsetX, (cy+y)*CellsLocOffsetY, 250)), 10, 10, FColor(0, 255, 0, 255), 1);

					if(!CellDrawn.Contains(cell))
					{
						// Draw!
						UKismetSystemLibrary::DrawDebugSphere(GWorld->GetWorld(), LocalToWorld(FVector((cx + x)*CellsLocOffsetX, (cy + y)*CellsLocOffsetY, 250)), 50, 10, FColor(255, 0, 0, 255), 5);

						TArray<FVector> V;
						V.AddUninitialized(24);

						for (int i = 0; i < 24; i++)
						{
							V[i].X = Vertices[i].X + CellsLocOffsetX * (cx+x);
							V[i].Y = Vertices[i].Y + CellsLocOffsetY * (cy+y);
							V[i].Z = Vertices[i].Z;
						}

						ProceduralMeshComponent->CreateMeshSection(CellsIndex, V, Triangles, Normals, UVs, Colors, Tangents, true);
						CellsIndex++; // TODO cellsDrawn length
						CellDrawn.Add(cell);
					}
				}
			}
		}
	}

}



FName ACaveGenerator2D::GetMapName(const uint8& X, const uint8& Y)
{
	FString str = FString::Printf(TEXT("%04d%04d"), X, Y);
	return FName(*str);
}