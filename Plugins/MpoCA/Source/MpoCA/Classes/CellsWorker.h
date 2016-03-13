
#pragma once

#include "Common.h"
#include "CellsRange.h"

class FCellsMap;


class FCellsWorker : public FRunnable
{
public:
	FCellsWorker(TSharedPtr<FCellsMap, ESPMode::ThreadSafe> InCellsMap, CELL_PROCESSING_FUNCTION_TYPE InProcessingFunction, function<void(void)> InDoneFunction, FCellsRange InCellsRange);
	~FCellsWorker();


	inline bool IsFinished() const
	{
		return !(CurrentStep < TotalSteps);
	}

	// Begin FRunnable interface.
	bool Init() override;
	uint32 Run() override;
	void Stop() override;
	// End FRunnable interface

private:
	int32 CurrentStep;
	int32 TotalSteps;
	TSharedPtr<FCellsMap, ESPMode::ThreadSafe> CellsMap;
	FCellsRange CellsRange;
	CELL_PROCESSING_FUNCTION_TYPE ProcessingFunction;
	function<void(void)> DoneFunction;
	bool StopCall;
};