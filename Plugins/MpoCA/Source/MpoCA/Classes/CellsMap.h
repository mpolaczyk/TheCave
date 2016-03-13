
#pragma once

#include "Common.h"

#include "Cell.h"


class MPOCA_API FCellsMap
{
public:
	FCellsMap(const uint8& InCellsX, const uint8& InCellsY, const uint8& InCellsZ, const uint8& InAliveChance);

	const uint8 CellsX;
	const uint8 CellsY;
	const uint8 CellsZ;
	
	inline FCell* GetCell(const uint8& InX, const uint8& InY, const uint8& InZ)
	{
		return &Cells[InX < 0 ? CellsX - 1 : (InX >= CellsX ? 0 : InX)][InY < 0 ? CellsY - 1 : (InY >= CellsY ? 0 : InY)][InZ < 0 ? CellsZ - 1 : (InZ >= CellsZ ? 0 : InZ)];
	}

	inline FCell* GetCellNoIndexCheck(const uint8& InX, const uint8& InY, const uint8& InZ)
	{
		return &Cells[InX][InY][InZ];
	}

	inline bool IsCellAlive(const uint8& InX, const uint8& InY, const uint8& InZ) const
	{
		return Cells[InX < 0 ? CellsX - 1 : (InX >= CellsX ? 0 : InX)][InY < 0 ? CellsY - 1 : (InY >= CellsY ? 0 : InY)][InZ < 0 ? CellsZ - 1 : (InZ >= CellsZ ? 0 : InZ)].IsAlive;
	}
	
	inline bool IsCellAliveNoIndexCheck(const uint8& InX, const uint8& InY, const uint8& InZ) const
	{
		return Cells[InX][InY][InZ].IsAlive;
	}

private:
	vector<vector<vector<FCell> > > Cells;
};

