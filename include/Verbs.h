#ifndef VERBS_H_
#define VERBS_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "Jinx.hpp"

struct Object{
    virtual ~Object() {}
    virtual std::string getName(){ return "Object";}
    virtual std::vector<double> apply(const std::vector<double>& _a, const std::vector<double>& _b){std::vector<double> res; return res;}
    std::string m_scriptPath;
    std::string m_libName;
    std::string m_verbName;
    Jinx::RuntimePtr m_runtime;
}; // base type for all objects

struct ObjectFactory {
  static Object* create(const std::string& id) { // creates an object from a string
    const Creators_t::const_iterator iter = static_creators().find(id);
    return iter == static_creators().end() ? 0 : (*iter->second)(); // if found, execute the creator function pointer
  }

 private:
  typedef Object* Creator_t(); // function pointer to create Object
  typedef std::map<std::string, Creator_t*> Creators_t; // map from id to creator
  static Creators_t& static_creators() { static Creators_t s_creators; return s_creators; } // static instance of map
  template<class T = int> struct Register {
    static Object* create() { return new T(); };
    static Creator_t* init_creator(const std::string& id) { return static_creators()[id] = create; }
    static Creator_t* creator;
  };
};

#define REGISTER_TYPE(T, STR) template<> ObjectFactory::Creator_t* ObjectFactory::Register<T>::creator = ObjectFactory::Register<T>::init_creator(STR)


class ScriptRunner : public Object{
  public:
    ScriptRunner(){};
    ~ScriptRunner(){};
    std::vector<double> apply(const std::vector<double>& _ind1, const std::vector<double>& _ind2);
    std::string getName(){ if (m_verbName == "") return "Script_Runner"; else return m_libName + m_verbName;}
    std::string getScriptText(std::string _name);
};

class AND : public Object{
  public:
    AND(){};
    ~AND(){};
    std::vector<double> apply(const std::vector<double>& _ind1, const std::vector<double>& _ind2);
    std::string getName(){ return "AND";}
};

class OR : public Object{
  public:
    OR(){};
    ~OR(){};
    std::vector<double> apply(const std::vector<double>& _ind1, const std::vector<double>& _ind2);
    std::string getName(){ return "OR";}
};

class crossup : public Object{
  public:
    crossup(){};
    ~crossup(){};
    std::vector<double> apply(const std::vector<double>& _ind1, const std::vector<double>& _ind2);
    std::string getName(){ return "crossup";}
};

class crossdown : public Object{
  public:
    crossdown(){};
    ~crossdown(){};
    std::vector<double> apply(const std::vector<double>& _ind1, const std::vector<double>& _ind2);
    std::string getName(){ return "crossdown";}
};

class above : public Object{
  public:
    above(){};
    ~above(){};
    std::vector<double> apply(const std::vector<double>& _ind1, const std::vector<double>& _ind2);
    std::string getName(){ return "above";}
};

class below : public Object{
  public:
    below(){};
    ~below(){};
    std::vector<double> apply(const std::vector<double>& _ind1, const std::vector<double>& _ind2);
    std::string getName(){ return "below";}
};

#endif // VERBS_H_
