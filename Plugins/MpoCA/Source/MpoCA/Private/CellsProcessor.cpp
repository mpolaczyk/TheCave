// Fill out your copyright notice in the Description page of Project Settings.

#include "MpoCAPrivatePCH.h"

#include "CellsProcessor.h"

#include "CellsRange.h"
#include "CellsWorker.h"
#include "CellsMap.h"
#include "Cell.h"

FCellsProcessor::FCellsProcessor(const uint8& InCellsX, const uint8& InCellsY, const uint8& InCellsZ, const uint8& InMaxThreads)
:	MaxThreads(InMaxThreads), CellsX(InCellsX), CellsY(InCellsY), CellsZ(InCellsZ)
{

}

FCellsProcessor::~FCellsProcessor()
{
	CleanupThreads();
}

void FCellsProcessor::CleanupThreads()
{
	if (Threads.Num() > 0)
	{
		for (auto Thread : Threads)
		{
			if (Thread != nullptr)
			{
				Thread->Kill(true);
			}
		}
		Threads.Empty();
	}
}

bool FCellsProcessor::ProcessCellsAsync(TSharedPtr<FCellsMap, ESPMode::ThreadSafe> InCellsMap, CELL_PROCESSING_FUNCTION_TYPE SurviveRule)
{
	if (SurviveRule == nullptr) { return false; }
	if (ProcessingNow) { return false; }
	ProcessingNow = true;

	CellsMap = InCellsMap;

	FinishedThreads = 0;
	for (uint8 i = 0; i < MaxThreads; i++)
	{
		FCellsRange Range = FCellsRange::GetSliceRange(i, MaxThreads, CellsX, CellsY, CellsZ);

		FCellsWorker* Worker = new FCellsWorker(CellsMap, SurviveRule, bind(&FCellsProcessor::ThreadFinished, this), Range);
		FRunnableThread* Thread = FRunnableThread::Create(Worker, TEXT("FCellsWorker"));
		
		Threads.Add(Thread);
	}

	return true;
}


void FCellsProcessor::ThreadFinished()
{
	FinishedThreads++;
	if (FinishedThreads == MaxThreads)
	{
		FCell* CurrentCell = nullptr;

		// Kill and create cells
		for (uint8 x = 0; x < CellsMap->CellsX; x++)
		{
			for (uint8 y = 0; y < CellsMap->CellsY; y++)
			{
				for (uint8 z = 0; z < CellsMap->CellsZ; z++)
				{
					CurrentCell = CellsMap->GetCell(x, y, z);
					CurrentCell->IsAlive = CurrentCell->ShouldLive;
				}
			}
		}

		ProcessingNow = false;
		FinishedThreads = 0;

		CleanupThreads();

		ProcessingDoneDelegate.ExecuteIfBound();
		ProcessingDoneDelegate.Unbind();
	}
}
