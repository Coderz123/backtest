#ifndef TICKLOADER_H_
#define TICKLOADER_H_

#include <chrono>
#include <string>

//Class to play ticks for backtesting
class Tickloader
{
public:
    Tickloader();
    ~Tickloader();
    void init(std::string& _filename);
    void setStartDate(int _y, int _m, int _d, int _h, int _min, int _s);
    void setEndDate(int _y, int _m, int _d, int _h, int _min, int _s);
    void setDays(std::vector<int> _days);
    std::string getFirstTick();
    std::string getNextTick();
    void goBack();

    std::string m_filename;
    int s_year, s_month, s_day, s_hour, s_min, s_sec;
    int e_year, e_month, e_day, e_hour, e_min, e_sec;
    std::vector<int> m_days;
    std::string m_tickLine;
    double m_oldTickCounter = 0;
    double m_tickCounter = 0;
};

#endif // TICKLOADER_H_
