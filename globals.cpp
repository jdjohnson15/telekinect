#include "stdafx.h"
#include "globals.h"

namespace Globals {
	char* data = ""; //needs to be allocated on the heap in order for values to be valid when accessed by differnt threads
	bool completed = false;
}