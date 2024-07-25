#ifndef EXITS_H_
#define EXITS_H_

#include <vector>
#include <map>

class Exit{
    public:
    std::string Name;
    std::string Ind1;
    std::string Verb;
    std::string Ind2;
};

class Exits{
  public:
    Exits(){};
    ~Exits(){}
    std::map<std::string, std::vector<Exit>> m_exitRules;
};

#endif // EXITS_H_
