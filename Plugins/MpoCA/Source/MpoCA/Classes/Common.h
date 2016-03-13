

#include <functional>
#include <vector>

using namespace std;

class FCellsMap;

#ifndef CELL_PROCESSING_FUNCTION_TYPE
#define CELL_PROCESSING_FUNCTION_TYPE function<bool(const FCellsMap* CellsMap, const uint8& CurrentCellX, const uint8& CurrentCellY, const uint8& CurrentCellZ)>
#endif


