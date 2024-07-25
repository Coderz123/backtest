#include <iostream>

class TickData
{
public:
	TickData();
	~TickData();
	TickData(std::string& _dt, double bid, double ask, int vol, bool _verbose);
	int year, month, day, hour, min, sec, msec, vol;
	double bid, ask;
	bool verbose;
};

