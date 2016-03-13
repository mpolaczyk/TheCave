
#pragma once

struct FCell
{
	FCell(bool InIsAlive, bool InShouldLive)
		:	IsAlive(InIsAlive),
			ShouldLive(InShouldLive) 
	{ };

	bool IsAlive : 1;
	bool ShouldLive : 1;
};