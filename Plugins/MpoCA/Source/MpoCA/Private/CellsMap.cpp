// Fill out your copyright notice in the Description page of Project Settings.

#include "MpoCAPrivatePCH.h"

#include "CellsMap.h"

FCellsMap::FCellsMap(const uint8& InCellsX, const uint8& InCellsY, const uint8& InCellsZ, const uint8& InAliveChance)
		:	CellsX(InCellsX),
			CellsY(InCellsY),
			CellsZ(InCellsZ)
	{
		Cells.resize(CellsX, vector<vector<FCell> >(CellsY, vector<FCell>(CellsZ, FCell(true, true))));

		for (uint8 x = 0; x < CellsX; x++)
		{
			for (uint8 y = 0; y < CellsY; y++)
			{
				for (uint8 z = 0; z < CellsZ; z++)
				{
					Cells[x][y][z].IsAlive = (FMath::RandRange(0, 100) < InAliveChance);
				}
			}
		}
	}