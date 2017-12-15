#ifndef __j1TIMER_H__
#define __j1TIMER_H__

#include "p2Defs.h"

class j1Timer
{
public:

	// Constructor
	j1Timer();

	void Start();
	void SetStartTime(int time);
	uint32 Read() const;
	float ReadSec() const;

private:
	uint32	started_at = 0;
};

#endif //__j1TIMER_H__