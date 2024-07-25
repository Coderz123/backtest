#include "Verbs.h"
#include <LuaCpp.hpp>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>

int v_above(int size, double const *const *inputs, double const *options,
            double *const *outputs) {

  const double *a = inputs[0];
  const double *b = inputs[1];

  (void)options;

  double *output = outputs[0];

  int i;
  for (i = 1; i < size; ++i) {
    *output++ = (a[i] > b[i]);
  }

  return 0;
}

int v_below(int size, double const *const *inputs, double const *options,
            double *const *outputs) {
  const double *a = inputs[0];
  const double *b = inputs[1];

  (void)options;

  double *output = outputs[0];

  int i;
  for (i = 1; i < size; ++i) {
    *output++ = (a[i] < b[i]);
  }

  return 0;
}

std::vector<double> AND::apply(const std::vector<double> &_ind1,
                               const std::vector<double> &_ind2) {
  std::vector<double> res(_ind1.size(), 0.0);
  for (int i = 0; i < _ind1.size(); i++) {
    if (_ind1[i] && _ind2[i])
      res[i] = 1.0;
  }
  return res;
}

std::vector<double> OR::apply(const std::vector<double> &_ind1,
                              const std::vector<double> &_ind2) {
  std::vector<double> res(_ind1.size(), 0.0);
  for (int i = 0; i < _ind1.size(); i++) {
    if (_ind1[i] || _ind2[i])
      res[i] = 1.0;
  }
  return res;
}

std::vector<double> crossup::apply(const std::vector<double> &_ind1,
                                   const std::vector<double> &_ind2) {
  std::vector<double> res(_ind1.size(), 0.0);
  for (int i = 0; i < _ind1.size(); i++) {
    if (_ind1[i] && _ind2[i])
      res[i] = 1.0;
  }
  return res;
}

std::vector<double> crossdown::apply(const std::vector<double> &_ind1,
                                     const std::vector<double> &_ind2) {
  std::vector<double> res(_ind1.size(), 0.0);
  for (int i = 0; i < _ind1.size(); i++) {
    if (_ind1[i] && _ind2[i])
      res[i] = 1.0;
  }
  return res;
}

std::vector<double> above::apply(const std::vector<double> &_ind1,
                                 const std::vector<double> &_ind2) {
  std::vector<double> res(_ind1.size(), 0.0);
  for (int i = 0; i < _ind1.size(); i++) {
    if (_ind1[i] > _ind2[i])
      res[i] = 1.0;
  }
  return res;
}

std::vector<double> below::apply(const std::vector<double> &_ind1,
                                 const std::vector<double> &_ind2) {
  std::vector<double> res(_ind1.size(), 0.0);
  for (int i = 0; i < _ind1.size(); i++) {
    if (_ind1[i] < _ind2[i])
      res[i] = 1.0;
  }
  return res;
}

std::string ScriptRunner::getScriptText(std::string _name) {
  std::string fileName = m_scriptPath + "/" + _name + ".jinx";
  std::ifstream in(fileName);
  std::stringstream ss;
  ss << in.rdbuf();
  return ss.str();
}

std::vector<double> ScriptRunner::apply(const std::vector<double> &_ind1,
                                        const std::vector<double> &_ind2) {
  using namespace LuaCpp;
  using namespace LuaCpp::Registry;
  using namespace LuaCpp::Engine;
  LuaContext ctx;
  // Compile the file before use
  ctx.CompileFile(m_verbName, m_scriptPath + "/" + m_verbName + ".lua");

  // Read Script and compile libraries
  LuaTTable In1, In2, Res1;

  for (int i = 0; i < _ind1.size(); i++) {
    In1.setValue(Table::Key(i + 1), std::make_shared<LuaTNumber>(_ind1[i]));
    In2.setValue(Table::Key(i + 1), std::make_shared<LuaTNumber>(_ind2[i]));
    Res1.setValue(Table::Key(i + 1), std::make_shared<LuaTNumber>(0.0));
  }
  ctx.AddGlobalVariable("ind1", std::make_shared<LuaTTable>(In1));
  ctx.AddGlobalVariable("ind2", std::make_shared<LuaTTable>(In2));
  ctx.AddGlobalVariable("res", std::make_shared<LuaTTable>(Res1));
  std::unique_ptr<LuaState> L = ctx.newStateFor(m_verbName);
  // Try Catch Block
  try {
    ctx.Run(m_verbName);
  } catch (std::runtime_error &e) {
    std::cout << e.what() << '\n';
  }

  std::vector<double> resj;

  LuaTTable *restbl =
      reinterpret_cast<LuaTTable *>(ctx.getGlobalVariable("res").get());
  // std::cout << "result table = " << restbl->ToString();
  auto cnt = restbl->getValues().size();
  for (auto g = 0; g < cnt; g++) {
    LuaTNumber &val =
        reinterpret_cast<LuaTNumber &>(restbl->getValue(Table::Key(g + 1)));
    resj.push_back(val.getValue());
  }
  In1.PopGlobal(*L);
  In2.PopGlobal(*L);
  Res1.PopGlobal(*L);
  return resj;
}

REGISTER_TYPE(AND, "AND");
REGISTER_TYPE(OR, "OR");
REGISTER_TYPE(crossup, "crossup");
REGISTER_TYPE(crossdown, "crossdown");
REGISTER_TYPE(above, "above");
REGISTER_TYPE(below, "below");
REGISTER_TYPE(ScriptRunner, "ScriptRunner");
