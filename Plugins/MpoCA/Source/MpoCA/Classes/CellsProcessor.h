// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Common.h"

class FCellsMap;
class FCellsWorker;
struct FCellsRange;

DECLARE_DELEGATE(FProcessingDone)

class MPOCA_API FCellsProcessor
{
public:
	FCellsProcessor(const uint8& InCellsX, const uint8& InCellsY, const uint8& InCellsZ, const uint8& InMaxThreads);
	~FCellsProcessor();
	const uint8 MaxThreads;
	const uint8 CellsX;
	const uint8 CellsY;
	const uint8 CellsZ;

	bool ProcessCellsAsync(TSharedPtr<FCellsMap, ESPMode::ThreadSafe> InCellsMap, CELL_PROCESSING_FUNCTION_TYPE SurviveRule);

	const FCellsMap* GetCells() const;

	FProcessingDone ProcessingDoneDelegate;

private:

	uint8 FinishedThreads;

	void ThreadFinished();
	void CleanupThreads();

	TSharedPtr<FCellsMap, ESPMode::ThreadSafe> CellsMap;

	TArray<FRunnableThread*> Threads;

	bool ProcessingNow = false;

};