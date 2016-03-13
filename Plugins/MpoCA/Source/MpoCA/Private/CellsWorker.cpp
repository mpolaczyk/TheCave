// Fill out your copyright notice in the Description page of Project Settings.

#include "MpoCAPrivatePCH.h"

#include "CellsWorker.h"

#include "CellsMap.h"
#include "CellsRange.h"

FCellsWorker::FCellsWorker(TSharedPtr<FCellsMap, ESPMode::ThreadSafe> InCellsMap, CELL_PROCESSING_FUNCTION_TYPE InProcessingFunction, function<void(void)> InDoneFunction, FCellsRange InCellsRange)
: CurrentStep(0)
{
	CellsMap = InCellsMap;
	ProcessingFunction = InProcessingFunction;
	DoneFunction = InDoneFunction;
	CellsRange = InCellsRange;
	TotalSteps = CellsRange.GetTotal();

}

FCellsWorker::~FCellsWorker()
{

}


bool FCellsWorker::Init()
{
	if (ProcessingFunction == nullptr || DoneFunction == nullptr) { return false; }

	return true;
}

uint32 FCellsWorker::Run()
{
	if (ProcessingFunction == nullptr) { return -1; }
	StopCall = false;
	CurrentStep = 0;
	FCell* CurrentCell = nullptr;

	// Check each cell if should live - apply rule
	for (uint8 x = CellsRange.StartX; x <= CellsRange.StopX; x++)
	{
		if (StopCall) { break; }
		for (uint8 y = CellsRange.StartY; y <= CellsRange.StopY; y++)
		{
			if (StopCall) { break; }
			for (uint8 z = CellsRange.StartZ; z <= CellsRange.StopZ; z++)
			{
				if (StopCall) { break; }
				CurrentCell = CellsMap->GetCell(x, y, z);
				CurrentCell->ShouldLive = ProcessingFunction(CellsMap.Get(), x, y, z);

				CurrentStep++;
			}
		}
	}
	// todo pass success here !StopCall
	DoneFunction();

	return 0;
}

void FCellsWorker::Stop()
{
	StopCall = true;
}
