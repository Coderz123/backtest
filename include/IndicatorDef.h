#ifndef INDICATOR_DEF
#define INDICATOR_DEF

#include <iostream>
#include <vector>
// #include <cxxtools/serializationinfo.h>
// #include <cxxtools/jsonserializer.h>
// #include <cxxtools/jsondeserializer.h>
// #include "json.hpp" //nlohmann
// #include <json/json.h> //jsoncpp
#include <jsoncons/json.hpp>

class Ind {
public:
  std::string indName;
  std::string name;
  std::string fullName;
  std::string timeFrame;
  int type;
  int inputs;
  int options;
  int outputs;
  std::vector<std::string> inputNames;
  std::vector<std::string> optionNames;
  std::vector<double> optionValues;
  std::vector<std::string> outputNames;
  // nlohmann::NLOHMANN_DEFINE_TYPE_INTRUSIVE(Ind, indName, name, fullName,
  // type, inputs, options, outputs, inputNames, optionNames, optionValues,
  // outputNames)
  void fillInd(jsoncons::json _json) {
    fullName = _json["fullName"].as<std::string>();
    indName = _json["indName"].as<std::string>();
    name = _json["name"].as<std::string>();
    timeFrame = _json["timeFrame"].as<std::string>();
    fullName = _json["fullName"].as<std::string>();
    type = _json["type"].as<int>();
    inputs = _json["inputs"].as<int>();
    options = _json["options"].as<int>();
    outputs = _json["outputs"].as<int>();
    inputNames = _json["inputNames"].as<std::vector<std::string>>();
    optionNames = _json["optionNames"].as<std::vector<std::string>>();
    optionValues = _json["optionValues"].as<std::vector<double>>();
    outputNames = _json["outputNames"].as<std::vector<std::string>>();
  }
};

class IndicatorDef {
public:
  std::vector<Ind> Indicators;
  // nlohmann::NLOHMANN_DEFINE_TYPE_INTRUSIVE(IndicatorDef, Indicators)
};

#endif
