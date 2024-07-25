#include "TickData.h"

TickData::TickData()
{
}

TickData::~TickData()
{
}

TickData::TickData(std::string& _dt, double _bid, double _ask, int _vol, bool _verbose)
{
	std::string dp = _dt.substr(0,8);
	std::cout << dp << std::endl;
	std::string tp = _dt.substr(9,9);
	std::cout << tp << std::endl;

	std::string hr, mi, s, ms;
	hr = tp.substr(0,2);
	mi = tp.substr(2,2);
	s  = tp.substr(4,2);
	ms = tp.substr(6);

    std::string yrstr = dp.substr(0,4);
    year = std::atoi(&yrstr[0]);
    std::string mstr = dp.substr(4,2);
    month = std::atoi(&mstr[0]);
    std::string dstr = dp.substr(6,2);
    day = std::atoi(&dstr[0]);
    hour = std::atoi(&hr[0]);
    min = std::atoi(&mi[0]);
    sec = std::atoi(&s[0]);
    msec = std::atoi(&ms[0]);

    bid = _bid;
    ask = _ask;
    vol = _vol;

    if (_verbose)
    {
        std::cout << hr << ":" << mi << ":" << s << "." << ms << std::endl;
        std::cout << bid << ":" << ask << " - " << std::endl;
    }

}

