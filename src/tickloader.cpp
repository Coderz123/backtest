#include "tickloader.h"
#include <iostream>
#include <fstream>

Tickloader::Tickloader()
{

}

Tickloader::~Tickloader()
{

}

void Tickloader::init(std::string& _filename)
{
    m_filename = _filename;
    m_tickCounter = 0;
    m_oldTickCounter = 0;
}

void Tickloader::setStartDate(int _y, int _m, int _d, int _h, int _min, int _s)
{
    s_year = _y;
    s_month = _m;
    s_day = _d;
    s_hour = _h;
    s_min = _min;
    s_sec = _s;
}

void Tickloader::setEndDate(int _y, int _m, int _d, int _h, int _min, int _s)
{
    e_year = _y;
    e_month = _m;
    e_day = _d;
    e_hour = _h;
    e_min = _min;
    e_sec = _s;
}

void Tickloader::setDays(std::vector<int> _days)
{
    m_days = _days;
}

std::string Tickloader::getFirstTick()
{
    std::ifstream in("" + m_filename);
    getline(in, m_tickLine);
    m_oldTickCounter = 0;
    m_tickCounter = in.tellg();
    return m_tickLine;
}

std::string Tickloader::getNextTick()
{
    std::ifstream in("" + m_filename);
    in.seekg(m_tickCounter);
    if(getline(in, m_tickLine))
    {
        m_oldTickCounter = m_tickCounter;
        m_tickCounter = in.tellg();
        return m_tickLine;
    }
    return std::string();
}

void Tickloader::goBack()
{
    m_tickCounter = m_oldTickCounter;
}
