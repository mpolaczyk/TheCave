
#pragma once

struct FCellsRange
{
public:
	uint8 StartX;
	uint8 StopX;
	uint8 StartY;
	uint8 StopY;
	uint8 StartZ;
	uint8 StopZ;

	inline uint32 GetTotal() const
	{
		return (StartX - StopX) * (StartY - StopY) * (StartZ - StopZ);
	}

	static FCellsRange GetSliceRange(const uint8& PieceIndex, const uint8& NumPieces, const uint8& MaxX, const uint8& MaxY, const uint8& MaxZ)
	{
		FCellsRange OutCellsRange;

		if (NumPieces == 1)
		{
			OutCellsRange.StartX = 0;
			OutCellsRange.StopX = MaxX;
		}
		else
		{
			uint8 PieceSize = MaxX / NumPieces;

			if (PieceIndex < NumPieces - 1)
			{
				OutCellsRange.StartX = PieceIndex * PieceSize;
				OutCellsRange.StopX = OutCellsRange.StartX + PieceSize - 1;
			}
			else
			{
				OutCellsRange.StartX = PieceSize * (NumPieces - 1);
				OutCellsRange.StopX = MaxX - 1;
			}
		}

		OutCellsRange.StartY = 0;
		OutCellsRange.StartZ = 0;
		OutCellsRange.StopY = MaxY;
		OutCellsRange.StopZ = MaxZ;

		return OutCellsRange;
	}

};

