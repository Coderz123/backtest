#ifndef TRIGGERS_H_
#define TRIGGERS_H_

#include <jsoncons/json.hpp>
#include <iostream>
#include <fstream>
#include <map>
#include <functional>

class Trigger {
    public:
    Trigger(){};
    ~Trigger(){};
    std::string Name;
    std::string Ind1;
    std::string Verb;
    std::string Ind2;
};

class Rules {
    public:
    Rules(){};
    ~Rules(){};

    int m_MaxBuy;
    int m_MaxSell;
    std::string m_BuyAndSell;
    std::string m_Enabled;
    std::string m_StartTime;
    std::string m_EndTime;
    std::string m_BuyColumn;
    std::string m_SellColumn;
    std::vector<int> m_Days;
    std::map<std::string, std::vector<Trigger>> m_triggers;
    std::map<std::string, std::vector<Trigger>> m_operations;
};

#endif // TRIGGERS_H_
