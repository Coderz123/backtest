#include "SystemRunner.h"
#include "IndicatorDef.h"
#include "Triggers.h"
#include "indicators.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
// #include "json.hpp"
// #include <json/json.h>
#include <jsoncons/json.hpp>
// #include <chaiscript/chaiscript.hpp>
#include "Verbs.h"
#include "utils.h"
#include "date.h"
#include <uberswitch/uberswitch.hpp>

void SystemRunner::run() {
  std::string line;
  namespace fs = std::filesystem;
  std::string path = m_path + "/" + m_system;
  // for (const auto& entry : fs::directory_iterator(path))
  //			std::cout << entry.path() << std::endl;
  std::ifstream inMain(path + "/" + "main.cfg");
  while (getline(inMain, line)) {
    auto pos = line.find("=");
    if (pos == std::string::npos)
      continue;
    if (line.substr(0, pos) == "system.name")
      m_sysName = line.substr(pos + 1);
    if (line.substr(0, pos) == "system.Description")
      m_sysDesc = line.substr(pos + 1);
    if (line.substr(0, pos) == "script.path")
      m_scriptPath = line.substr(pos + 1);
    if (line.substr(0, pos) == "system.path")
      m_sysPath = line.substr(pos + 1);
    if (line.substr(0, pos) == "indicators")
      m_indFile = line.substr(pos + 1);
    if (line.substr(0, pos) == "triggers")
      m_triggerFile = line.substr(pos + 1);
    if (line.substr(0, pos) == "exits")
      m_exitFile = line.substr(pos + 1);
    if (line.substr(0, pos) == "results")
      m_resultFile = line.substr(pos + 1);
    if (line.substr(0, pos) == "pipvalue")
      m_pipValue = std::stod(line.substr(pos + 1));
    if (line.substr(0, pos) == "account.startbalance")
      m_accountStartBalance = std::stod(line.substr(pos + 1));
    if (line.substr(0, pos) == "account.currency")
      m_accCurrency = line.substr(pos + 1);
    if (line.substr(0, pos) == "account.margin")
      m_accountMargin = line.substr(pos + 1);
    if (line.substr(0, pos) == "account.margin.call")
      m_accountMarginCall = std::stod(line.substr(pos + 1));
    if (line.substr(0, pos) == "trade.max.lots")
      m_tradeMaxLots = std::stod(line.substr(pos + 1));
    if (line.substr(0, pos) == "trade.max.percentage")
      m_tradeMaxPercentage = std::stod(line.substr(pos + 1));
    // std::cout << line << std::endl;
  }
  std::cout << "Name        : " << m_sysName << std::endl;
  std::cout << "Description : " << m_sysDesc << std::endl;
  std::cout << "Indicators  : " << m_indFile << std::endl;
  std::cout << "Triggers    : " << m_triggerFile << std::endl;
  std::cout << "Exits       : " << m_exitFile << std::endl;
  std::cout << "Scripts     : " << m_scriptPath << std::endl;
  std::cout << "Instrument  : " << m_instrument << std::endl;
  std::cout << "Results     : " << m_resultFile << std::endl;
  std::cout << "PipValue    : " << m_pipValue << std::endl;
  std::cout << "Start Bal.  : " << m_accountStartBalance << std::endl;
  std::cout << "Currency    : " << m_accCurrency << std::endl;
  std::cout << "Margin      : " << m_accountMargin << std::endl;
  std::cout << "Margin Call : " << m_accountMarginCall << std::endl;
  std::cout << "Max Lots    : " << m_tradeMaxLots << std::endl;
  std::cout << "Max Perc.   : " << m_tradeMaxPercentage << std::endl;

  readIndicators();
  readTriggers();
  readExits();
  if (m_rules.m_Enabled == "false") {
    std::cout << "This system is disabled." << std::endl;
    return;
  }
  processIndicators();
  processRules();
  processTriggers();
  processOperations();
  std::cout << "All calculations completed." << std::endl;
  data.write<double, long>("indicators.csv", io_format::csv2);
  applyTimeFilter();
  Execute();
}

void SystemRunner::indInfo(std::string _name) {
  const ti_indicator_info *info = ti_find_indicator(_name.c_str());
  if (info) {
    printf("Found indicator %s.\n It takes %d inputs, %d options, and provides "
           "%d outputs\n",
           info->full_name, info->inputs, info->options, info->outputs);

    /* TODO use the indicator (e.g. info->start(...), info->indicator(...)) */
    std::cout << "Indicator Inputs:" << std::endl;
    for (int i = 0; i < info->inputs; i++)
      printf("%s\n", info->input_names[i]);

    std::cout << "Indicator Options:" << std::endl;
    for (int i = 0; i < info->options; i++)
      printf("%s\n", info->option_names[i]);

    std::cout << "Indicator Outputs:" << std::endl;
    for (int i = 0; i < info->outputs; i++)
      printf("%s\n", info->output_names[i]);

    std::cout << "Start Function: " << info->start << std::endl;
    std::cout << "Indicator Function: " << info->indicator << std::endl;

  } else {
    printf("Erorr, not found\n");
  }
}

void SystemRunner::readIndicators() {
  try {

    std::cout << "Reading indicator file "
              << m_sysPath + "/" + m_system + "/" + m_indFile << std::endl;
    std::ifstream indStr(m_sysPath + "/" + m_system + "/" + m_indFile);

    // nlohmann::json j = nlohmann::json::parse(indStr);
    jsoncons::json _json = jsoncons::json::parse(indStr);
    jsoncons::json j = _json["Indicators"];
    for (auto &item : j.array_range()) {
      Ind a;
      a.fillInd(item);
      m_indicatorDefs.Indicators.push_back(a);
    }
    // std::cout << j << std::endl;
    // auto iDef = j.template get<IndicatorDef>();
    // std::cout << "Full Name: " << iDef.Indicators[0].fullName << std::endl;
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}

void SystemRunner::readTriggers() {
  std::cout << "Reading trading rules "
            << m_sysPath + "/" + m_system + "/" + m_triggerFile << std::endl;

  std::ifstream trigStr(m_sysPath + "/" + m_system + "/" + m_triggerFile);
  jsoncons::json _json = jsoncons::json::parse(trigStr);
  m_rules.m_Enabled = _json["ENABLED"].as<std::string>();
  m_rules.m_MaxBuy = _json["MAX_BUY"].as<int>();
  m_rules.m_MaxSell = _json["MAX_SELL"].as<int>();
  m_rules.m_BuyAndSell = _json["BUY_AND_SELL"].as<std::string>();
  m_rules.m_BuyColumn = _json["BUY_COLUMN"].as<std::string>();
  m_rules.m_SellColumn = _json["SELL_COLUMN"].as<std::string>();

  jsoncons::json _timing = _json["TIMING"];
  for (auto &item : _timing.array_range()) {
    m_rules.m_StartTime = item["START"].as<std::string>();
    m_rules.m_EndTime = item["END"].as<std::string>();
    m_rules.m_Days = item["DAYS"].as<std::vector<int>>();
  }

  jsoncons::json b = _json["BUY"];
  std::vector<Trigger> _buyTriggers;
  for (auto &item : b.array_range()) {
    Trigger trig;
    trig.Name = item["Name"].as<std::string>();
    trig.Ind1 = item["Ind1"].as<std::string>();
    trig.Ind2 = item["Ind2"].as<std::string>();
    trig.Verb = item["Verb"].as<std::string>();
    _buyTriggers.push_back(trig);
  }
  m_rules.m_triggers.insert(
      std::pair<std::string, std::vector<Trigger>>("BUY", _buyTriggers));

  jsoncons::json s = _json["SELL"];
  std::vector<Trigger> _sellTriggers;
  for (auto &item : s.array_range()) {
    Trigger trig;
    trig.Name = item["Name"].as<std::string>();
    trig.Ind1 = item["Ind1"].as<std::string>();
    trig.Ind2 = item["Ind2"].as<std::string>();
    trig.Verb = item["Verb"].as<std::string>();
    _sellTriggers.push_back(trig);
  }
  m_rules.m_triggers.insert(
      std::pair<std::string, std::vector<Trigger>>("SELL", _sellTriggers));

  jsoncons::json bo = _json["BUY_OPERATIONS"];
  std::vector<Trigger> _buyOperations;
  for (auto &item : bo.array_range()) {
    Trigger trig;
    trig.Name = item["Name"].as<std::string>();
    trig.Ind1 = item["Ind1"].as<std::string>();
    trig.Ind2 = item["Ind2"].as<std::string>();
    trig.Verb = item["Verb"].as<std::string>();
    _buyOperations.push_back(trig);
  }
  m_rules.m_operations.insert(std::pair<std::string, std::vector<Trigger>>(
      "BUY_OPERATIONS", _buyOperations));

  jsoncons::json so = _json["SELL_OPERATIONS"];
  std::vector<Trigger> _sellOperations;
  for (auto &item : so.array_range()) {
    Trigger trig;
    trig.Name = item["Name"].as<std::string>();
    trig.Ind1 = item["Ind1"].as<std::string>();
    trig.Ind2 = item["Ind2"].as<std::string>();
    trig.Verb = item["Verb"].as<std::string>();
    _sellOperations.push_back(trig);
  }
  m_rules.m_operations.insert(std::pair<std::string, std::vector<Trigger>>(
      "SELL_OPERATIONS", _sellOperations));
}

void SystemRunner::readExits() {
  std::cout << "Reading trade exit rules "
            << m_sysPath + "/" + m_system + "/" + m_exitFile << std::endl;

  std::ifstream exitStr(m_sysPath + "/" + m_system + "/" + m_exitFile);
  jsoncons::json _json = jsoncons::json::parse(exitStr);

  jsoncons::json _exits = _json["BUY_EXIT"];
  std::vector<Exit> buyExits;
  for (auto &item : _exits.array_range()) {
    Exit ex;
    ex.Name = item["Name"].as<std::string>();
    ex.Ind1 = item["Ind1"].as<std::string>();
    ex.Ind2 = item["Ind2"].as<std::string>();
    ex.Verb = item["Verb"].as<std::string>();
    buyExits.push_back(ex);
  }
  m_exitRules.m_exitRules.insert(
      std::pair<std::string, std::vector<Exit>>("BUY_EXIT", buyExits));

  jsoncons::json s_exits = _json["SELL_EXIT"];
  std::vector<Exit> sellExits;
  for (auto &item : s_exits.array_range()) {
    Exit ex;
    ex.Name = item["Name"].as<std::string>();
    ex.Ind1 = item["Ind1"].as<std::string>();
    ex.Ind2 = item["Ind2"].as<std::string>();
    ex.Verb = item["Verb"].as<std::string>();
    sellExits.push_back(ex);
  }
  m_exitRules.m_exitRules.insert(
      std::pair<std::string, std::vector<Exit>>("SELL_EXIT", sellExits));
}

void SystemRunner::setPath(std::string _path) { m_path = _path; }

void SystemRunner::setSystem(std::string _system) { m_system = _system; }

void SystemRunner::setInstrument(std::string _instr) { m_instrument = _instr; }
void SystemRunner::setTimeframe(int _tf) { m_timeFrame = _tf; }

void SystemRunner::processIndicators() {
  std::cout << "Calculating Indicators..." << std::endl;
  DateTime lt;
  // lt.set_time(1675182600);
  // std::cout << lt.string_format(DT_FORMAT::ISO_DT_TM) << std::endl;
  // lt.set_time(1672585200);
  // std::cout << lt.string_format(DT_FORMAT::ISO_DT_TM) << std::endl;
  // std::cout << m_rules.m_Enabled << std::endl;

  // dblDataFrame data;
  std::stringstream ss;
  ss << m_timeFrame << "_" << m_instrument << ".csv";
  std::string fname = ss.str();
  data.read(fname.c_str(), io_format::csv2);

  for (auto ind : m_indicatorDefs.Indicators) {
    /*
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
   */
    std::cout << ind.indName << ", " << ind.name << std::endl;
    if (ind.indName.substr(0,1) == ":") {

    }
    else {
    //indInfo(ind.name);
    const ti_indicator_info *pInd = ti_find_indicator(ind.name.c_str());

    const auto &openCol = data.get_column<double>("open");
    const auto &highCol = data.get_column<double>("high");
    const auto &lowCol = data.get_column<double>("low");
    const auto &closeCol = data.get_column<double>("close");
    const auto &volCol = data.get_column<double>("volume");
    const int input_length =
        closeCol.size(); // sizeof(data_in) / sizeof(double);
    double *all_inputs[ind.inputs];
    double options[ind.options];
    double *all_outputs[ind.outputs];

    // Get the inut data
    for (int i = 0; i < ind.inputs; i++) {
      if (ind.inputNames[i] == "open")
        all_inputs[i] = (double *)&openCol[0];
      else if (ind.inputNames[i] == "high")
        all_inputs[i] = (double *)&highCol[0];
      else if (ind.inputNames[i] == "low")
        all_inputs[i] = (double *)&lowCol[0];
      else if (ind.inputNames[i] == "close")
        all_inputs[i] = (double *)&closeCol[0];
      else if (ind.inputNames[i] == "vol")
        all_inputs[i] = (double *)&volCol[0];
      else {
        const auto &tmpCol = data.get_column<double>(ind.inputNames[i].c_str());
        all_inputs[i] = (double *)&tmpCol[0];
      }
    }

    // Get the options
    for (int i = 0; i < ind.options; i++) {
      options[i] = ind.optionValues[i];
    }

    if (ind.name == "ema") m_startLine = std::max((int)ind.optionValues[0], m_startLine);
    const int start = pInd->start(options);
    m_startLine = std::max(m_startLine, start);
    const int output_length = input_length - start;

    double *pDbl = 0;
    for (int i = 0; i < ind.outputs; i++) {
      pDbl = (double *)malloc(output_length * sizeof(double));
      assert(pDbl != 0);
      all_outputs[i] = pDbl;
    }

    int error = pInd->indicator(input_length, all_inputs, options, all_outputs);
    assert(error == TI_OKAY);
    // std::vector<double> sma_res(data_out, data_out + output_length);
    for (int i = 0; i < ind.outputs; i++) {
      std::vector<double> tmpRes(input_length, 0.0);
      std::copy(all_outputs[i], all_outputs[i] + output_length,
                tmpRes.begin() + start);
      data.load_column(ind.outputNames[i].c_str(), std::move(tmpRes));
    }
    }
    // std::cout << "There are " << sma_res.size() << " ma records." <<
    // std::endl;
  }

  // for (const auto& dbl: dtCol) {
  //   std::cout << dbl << ", ";
  // }
  // std::cout << std::endl;
  // std::cout << "There are " << dtCol.size() << " records." << std::endl;

  // data.write<std::ostream, std::string, double>(std::cout, io_format::csv2);
  // data.write<double, long>("test.json", io_format::json);
  data.write<double, long>("indicators.csv", io_format::csv2);
}

void SystemRunner::processRules() {
  std::cout << "Processing trading rules..." << std::endl;
  for (auto item : m_rules.m_triggers) {
    std::string op = item.first;
    auto trigList = item.second;
    for (auto t : trigList) {
      std::cout << op << ": " << t.Ind1 << "," << t.Verb << "," << t.Ind2
                << std::endl;
    }
  }
}

void SystemRunner::processTriggers() {
  std::cout << "Processing rule triggers..." << std::endl;
  for (auto item : m_rules.m_triggers) {
    std::string op = item.first;
    auto trigList = item.second;
    for (auto t : trigList) {
      // std::cout << "Processing Triggers: " << op << ": " << t.Ind1 << "," <<
      // t.Verb << "," << t.Ind2 << std::endl;
      Object *pVerb = 0;
      auto pos = t.Verb.find(":");
      if (pos == std::string::npos) {
        pVerb = ObjectFactory::create(t.Verb);
      } else {
        pVerb = ObjectFactory::create("ScriptRunner");
        pVerb->m_libName = t.Verb.substr(0, pos);
        pVerb->m_verbName = t.Verb.substr(pos + 1);
        pVerb->m_scriptPath = m_scriptPath;
        std::cout << "PT: Verb=" << pVerb->m_libName
                  << ", Verb=" << pVerb->m_verbName << std::endl;
      }
      // std::cout << "Type Name: " << pVerb->getName() << std::endl;
      std::vector<double> tmpRes(data.shape().first, 0.0);
      std::vector<double>indNumVal{data.shape().first, 0.0};
      std::vector<double>indNumVal2{data.shape().first, 0.0};
      bool oneNum = false;
      bool twoNum = false;
      if (t.Ind1.substr(0,1) == "#" && t.Ind2.substr(0,1) == "#"){
        double inVal = std::stod(t.Ind1.substr(1));
        std::fill(indNumVal.begin(), indNumVal.end(), inVal);
        double inVal2 = std::stod(t.Ind2.substr(1));
        std::fill(indNumVal2.begin(), indNumVal2.end(), inVal2);
        tmpRes = pVerb->apply(indNumVal, indNumVal2);
      }
      if (t.Ind1.substr(0,1) != "#" && t.Ind2.substr(0,1) == "#"){
        const auto &IndCol1 = data.get_column<double>(t.Ind1.c_str());
        double inVal2 = std::stod(t.Ind2.substr(1));
        std::fill(indNumVal2.begin(), indNumVal2.end(), inVal2);
        tmpRes = pVerb->apply(IndCol1, indNumVal2);
      }
      if (t.Ind1.substr(0,1) == "#" && t.Ind2.substr(0,1) != "#"){
        const auto &IndCol2 = data.get_column<double>(t.Ind2.c_str());
        double inVal = std::stod(t.Ind1.substr(1));
        std::fill(indNumVal.begin(), indNumVal.end(), inVal);
        tmpRes = pVerb->apply(indNumVal, IndCol2);
      }
      if (t.Ind1.substr(0,1) != "#" && t.Ind2.substr(0,1) != "#"){
        const auto &IndCol1 = data.get_column<double>(t.Ind1.c_str());
        const auto &IndCol2 = data.get_column<double>(t.Ind2.c_str());
        tmpRes = pVerb->apply(IndCol1, IndCol2);
      }

      data.load_column(t.Name.c_str(), std::move(tmpRes));
      data.write<double, long>("indicators.csv", io_format::csv2);
      delete pVerb;
    }
  }
}

void SystemRunner::processOperations() {
  std::cout << "Processing trade operations..." << std::endl;

  // std::string tblStr = data.to_string<double, long>();

  for (auto item : m_rules.m_operations) {
    std::string op = item.first;
    auto trigList = item.second;
    for (auto t : trigList) {
      Object *pVerb = 0;
      std::cout << "Verb is " << t.Verb << std::endl;
      auto pos = t.Verb.find(":");
      if (pos == std::string::npos) {
        pVerb = ObjectFactory::create(t.Verb);
      } else {
        pVerb = ObjectFactory::create("ScriptRunner");
        pVerb->m_libName = t.Verb.substr(0, pos);
        pVerb->m_verbName = t.Verb.substr(pos + 1);
        pVerb->m_scriptPath = m_scriptPath;
        // std::cout << "PT: Library=" << pVerb->m_libName << ", Verb=" <<
        // pVerb->m_verbName << std::endl; pVerb->m_runtime = runtime;
      }
      const auto &IndCol1 = data.get_column<double>(t.Ind1.c_str());
      const auto &IndCol2 = data.get_column<double>(t.Ind2.c_str());
      std::vector<double> tmpRes(IndCol1.size(), 0.0);
      tmpRes = pVerb->apply(IndCol1, IndCol2);
      data.load_column(t.Name.c_str(), std::move(tmpRes));
      delete pVerb;
    }
    data.write<double, long>("indicators.csv", io_format::csv2);
  }
}

void SystemRunner::processExits() {
  std::cout << "Processing Exits ..." << std::endl;

  // std::string tblStr = data.to_string<double, long>();

  for (auto item : m_exitRules.m_exitRules) {
    std::string op = item.first;
    auto exitList = item.second;
    for (auto t : exitList) {
      Object *pVerb = 0;
      std::cout << "Verb is " << t.Verb << std::endl;
      auto pos = t.Verb.find(":");
      if (pos == std::string::npos) {
        pVerb = ObjectFactory::create(t.Verb);
      } else {
        pVerb = ObjectFactory::create("ScriptRunner");
        pVerb->m_libName = t.Verb.substr(0, pos);
        pVerb->m_verbName = t.Verb.substr(pos + 1);
        pVerb->m_scriptPath = m_scriptPath;
        // std::cout << "PT: Library=" << pVerb->m_libName << ", Verb=" <<
        // pVerb->m_verbName << std::endl; pVerb->m_runtime = runtime;
      }
      const auto &IndCol1 = data.get_column<double>(t.Ind1.c_str());
      const auto &IndCol2 = data.get_column<double>(t.Ind2.c_str());
      std::vector<double> tmpRes(IndCol1.size(), 0.0);
      tmpRes = pVerb->apply(IndCol1, IndCol2);
      data.load_column(t.Name.c_str(), std::move(tmpRes));
      delete pVerb;
    }
    data.write<double, long>("indicators.csv", io_format::csv2);
  }
}

std::string SystemRunner::getScriptText(std::string _name) {
  std::string fileName = m_scriptPath + "/" + _name + ".jinx";
  std::ifstream in(fileName);
  std::stringstream ss;
  ss << in.rdbuf();
  return ss.str();
}

void SystemRunner::setTickFile(std::string _file) { m_tickFile = _file; }

void SystemRunner::applyTimeFilter() {
  // std::cout << m_rules.m_StartTime << std::endl;
  // std::cout << m_rules.m_EndTime << std::endl;
  // for (auto i : m_rules.m_Days)
  //   std::cout << i << std::endl;

  auto index_size = data.shape().first;
  std::vector<double> remList;
  for (auto idx = 1; idx < index_size - 1; idx++) {
    if (idx < m_startLine) {
        remList.push_back(idx);
        continue;
    }
    continue;
    auto row = data.get_row<double>(idx);
    // std::cout << idx << " - " << std::setw(15) << row.at<double>(1)
              // << std::endl;
    using namespace std::chrono;

    double time_ms = row.at<double>(1);
    using ds = duration<double>;
    sys_time<ds> time{ds{time_ms}};
    auto tp = round<milliseconds>(time);
    auto td = floor<days>(tp);
    auto tod = tp - td;
    // std::cout << "tod = " << tod << '\n';
    int startHr = std::atoi(m_rules.m_StartTime.substr(0,2).c_str());
    int startMin = std::atoi(m_rules.m_StartTime.substr(2).c_str());
    int endHr = std::atoi(m_rules.m_EndTime.substr(0,2).c_str());
    int endMin = std::atoi(m_rules.m_EndTime.substr(2).c_str());
    double startTm = startHr * 60 * 60 + startMin * 60;
    double endTm = endHr * 60 * 60 + endMin * 60;
    // std::cout << m_rules.m_StartTime << ", " << m_rules.m_EndTime << ", " << startHr << startMin << " " << endHr << endMin << ":" << startTm << "-" << endTm << std::endl;


    sys_time<ds> start_time{ds{startTm}};
    sys_time<ds> end_time{ds{endTm}};
    // auto start_time = date::make_time(startTm);
    // auto end_time = date::make_time(endTm);
    sys_time<ds> rec_time{ds{tod}};

    // std::cout << std::format("Rec Time {0:%R} ", rec_time) << std::format(" Start Time: {0:%R} ", start_time) << std::format("End Time: {0:%R}.", end_time) << std::endl;
    bool remove = false;
    year_month_weekday ymd{std::chrono::floor<std::chrono::days>(tp)};
    // std::cout << "Day: " << ymd.weekday() << std::endl;
    auto dayNo = ymd.weekday();
    if (std::find(m_rules.m_Days.begin(), m_rules.m_Days.end(),
                  dayNo.c_encoding()) == m_rules.m_Days.end())
      remove = true;

    if (rec_time < start_time) remove = true;
    if (rec_time >= end_time) remove = true;

    if (remove == true)
    {
      remList.push_back(idx);
    }
  }
  std::cout << "Removing records..." << std::endl;
  for (auto it = remList.begin();it != remList.end();++it){
    // std::cout << "Removing record with index " << *it << std::endl;
    data.remove_data_by_idx<double>({*it,*it});
  }
  int newSize = data.shape().first;
  std::vector<double> newIdx(newSize,0);
  for (int i = 1; i <= newSize; i++)
    newIdx[i-1] = i;
  data.load_column("Idx", std::move(newIdx), nan_policy::dont_pad_with_nans);
  dblDataFrame newdata = data.get_reindexed<double, double>("Idx");
  data.swap(newdata);
  // data.write<std::ostream, double>(std::cout, io_format::csv2);
}

void SystemRunner::Execute() {
  // Read and compile Jinx libraries
  // Populate Bid and Ask
  // Find first data line with all indicators calculated.
  // Apply trade rules and times.
  // Get first trade line
  // Play ticks until next data line, executing exit rules
  // Get next data line
  // Rinse, repeat
  // Generate Report
  //
  // int cols = data.shape().first;
  // std::vector<std::string> cols{"datetime"};
  // cols.push_back(m_rules.m_BuyColumn);
  // cols.push_back(m_rules.m_SellColumn);

  using namespace std::chrono;
  // std::vector<double> bids(data.shape().first, 0);
  // std::vector<double> asks(data.shape().first, 0);
  // std::vector<double> vols(data.shape().first, 0);
  // data.load_column<double>("bid", std::move(bids), nan_policy::dont_pad_with_nans);
  // data.load_column<double>("ask", std::move(asks), nan_policy::dont_pad_with_nans);
  // data.load_column<double>("vol", std::move(vols), nan_policy::dont_pad_with_nans);

  int buytradeCnt = 0;
  int selltradeCnt = 0;
  bool newBar = false;

  using ds = duration<double>;
  double tfRows = data.shape().first;
  double currentRow = 0;

  //trade variables
  double currentBalance = m_accountStartBalance;
  double currentAccountMargin = 0.0; //Total acc margin for margin calls
  double currentMargin = 0.0; //Total used margin
  double currentMarginPerc = 0.0; //Acc margin perc for margin calls
  double tradeId = 0.0;

  auto lastRow = data.get_row<double>(tfRows - 1);
  double rec_epoch = lastRow.at<double>(1);
  sys_time<ds> lastDte{ds{rec_epoch}};
  year_month_day lastDate{std::chrono::floor<std::chrono::days>(lastDte)};
  auto tp = round<seconds>(lastDte);
  auto td = floor<days>(tp);
  auto tod = tp - td;
  sys_time<ds> rec_time{ds{tod}};

  std::cout << "Shape : " << tfRows << std::endl;
  m_tickLoader.init(m_tickFile);
  double dtDbl = 0.0;

  int startHr = std::atoi(m_rules.m_StartTime.substr(0,2).c_str());
    int startMin = std::atoi(m_rules.m_StartTime.substr(2).c_str());
    int endHr = std::atoi(m_rules.m_EndTime.substr(0,2).c_str());
    int endMin = std::atoi(m_rules.m_EndTime.substr(2).c_str());
    double startTm = startHr * 60 * 60 + startMin * 60;
    double endTm = endHr * 60 * 60 + endMin * 60;
    // std::cout << m_rules.m_StartTime << ", " << m_rules.m_EndTime << ", " << startHr << startMin << " " << endHr << endMin << ":" << startTm << "-" << endTm << std::endl;
    sys_time<ds> start_time{ds{startTm}};
  sys_time<ds> end_time{ds{endTm}};

  while (currentRow < tfRows - 2) {
        auto prev_row = data.get_row<double>(currentRow);
    auto from_row = data.get_row<double>(currentRow + 1);
    auto to_row = data.get_row<double>(currentRow + 2);
    //for (auto t = prev_row.begin<double>(); t != prev_row.end<double>(); ++t) {
    //    std::cout << *t << ", " ;
    //}
    //std::cout << std::endl;
    newBar = true;
    // std::cout << "Row " << currentRow << " and " << currentRow + 1 << std::endl;
    double prev_rec_time_s = prev_row.at<double>(1);
    double from_rec_time_s = from_row.at<double>(1);
    double to_rec_time_s = to_row.at<double>(1);
    sys_time<ds> prev_time{ds{prev_rec_time_s}};
    sys_time<ds> from_time{ds{from_rec_time_s}};
    sys_time<ds> to_time{ds{to_rec_time_s}};

    auto tp = round<milliseconds>(from_time);
    auto td = floor<days>(tp);
    auto tod = tp - td;
    // std::cout << "tod = " << tod << '\n';
    // auto start_time = date::make_time(startTm);
    // auto end_time = date::make_time(endTm);
    sys_time<ds> rec_time{ds{tod}};

    // std::cout << std::format("Rec Time {0:%R} ", rec_time) << std::format(" Start Time: {0:%R} ", start_time) << std::format("End Time: {0:%R}.", end_time) << std::endl;
    bool remove = false;
    year_month_weekday ymd{std::chrono::floor<std::chrono::days>(tp)};
    // std::cout << "Day: " << ymd.weekday() << std::endl;
    auto dayNo = ymd.weekday();
    if (std::find(m_rules.m_Days.begin(), m_rules.m_Days.end(),
                  dayNo.c_encoding()) == m_rules.m_Days.end())
      remove = true;

    if (rec_time < start_time) remove = true;
    if (rec_time >= end_time) remove = true;

    if (remove == true)
    {
      currentRow = currentRow + 1;
      continue;
    }


    dtDbl = 0.0;
    std::string dtPart;
    //Read ticks intil the first data record
    std::string nextLine;
    while (dtDbl < from_rec_time_s) {
      nextLine = m_tickLoader.getNextTick();
      // std::cout << ".";
      std::string dtPart = nextLine.substr(0, 18);
      dtDbl = DateUtil::epochFromDateStr(dtPart);
    }
    //Process ticks until the next data record
    while (dtDbl < to_rec_time_s) {
      nextLine = m_tickLoader.getNextTick();
      dtPart = nextLine.substr(0, 18);
      dtDbl = DateUtil::epochFromDateStr(dtPart);
      // std::cout << "|";
      auto firstComma = nextLine.find(",");
      auto secondComma = nextLine.find(",", firstComma + 1);
      auto thirdComma = nextLine.find(",", secondComma + 1);
      std::string bidPart = nextLine.substr(firstComma + 1, secondComma - firstComma - 1);
      std::string askPart = nextLine.substr(secondComma + 1, thirdComma - secondComma - 1);
      std::string volPart = nextLine.substr(thirdComma + 1);
      double bid = std::stod(bidPart);
      double ask = std::stod(askPart);
      double vol = std::stod(volPart);
      //Check for trade
      if (prev_row.at<double>(data.col_name_to_idx(m_rules.m_BuyColumn.c_str())) == 1 &&
          buytradeCnt < m_rules.m_MaxBuy &&
          (m_rules.m_BuyAndSell == "BOTH" || m_rules.m_BuyAndSell == "BUY")
          && newBar == true){
        std::cout << "\nAdd a buy trade" << std::endl;
        Trade trade;
        trade.m_tradeTime = dtDbl;
        trade.m_openPrice = ask;
        trade.m_isBuyTrade = true;
        trade.m_tradeId = tradeId++;
        trade.m_isOpen = true;
        m_trades.push_back(trade);
        buytradeCnt++;
        newBar = false;
        continue;
      }
      if (prev_row.at<double>(data.col_name_to_idx(m_rules.m_SellColumn.c_str())) == 1 &&
          selltradeCnt < m_rules.m_MaxSell &&
          (m_rules.m_BuyAndSell == "BOTH" || m_rules.m_BuyAndSell == "SELL")
          && newBar == true){
        std::cout << "\nAdd a sell trade" << std::endl;
        Trade trade;
        trade.m_tradeTime = dtDbl;
        trade.m_openPrice = bid;
        trade.m_isBuyTrade = false;
        trade.m_tradeId = tradeId++;
        trade.m_isOpen = true;
        m_trades.push_back(trade);
        selltradeCnt++;
        newBar = false;
        continue;
      }

      //Check for exit
      for (auto it = m_trades.begin(); it != m_trades.end(); ++it) {
        Trade& item = *it;
        if (item.m_isOpen == false) continue;
        if (item.m_isBuyTrade == true)
        {
          std::vector<Exit> vec = m_exitRules.m_exitRules["BUY_EXIT"];
          for (auto exitItem: vec) {
            double leftVal = 0.0;
            bool hasVal = false;
            if (data.has_column(exitItem.Ind1.c_str())){
              leftVal = prev_row.at<double>(data.col_name_to_idx(exitItem.Ind1.c_str()));
              hasVal = true;
            }
            else {
              uswitch(exitItem.Ind1){
                ucase("BID"):
                  leftVal = bid;
                  hasVal = true;
                  break;
                ucase("ASK"):
                  leftVal = ask;
                  hasVal = true;
                  break;
                ucase("VOL"):
                  leftVal = vol;
                  hasVal = true;
                  break;
              }
            }
            if (hasVal == false) continue;
            double rightVal = 0.0;
            hasVal = false;
            if (data.has_column(exitItem.Ind2.c_str())){
              rightVal = prev_row.at<double>(data.col_name_to_idx(exitItem.Ind2.c_str()));
              hasVal = true;
            }
            else {
              uswitch(exitItem.Ind2){
                ucase("BID"):
                  rightVal = bid;
                  hasVal = true;
                  break;
                ucase("ASK"):
                  rightVal = ask;
                  hasVal = true;
                  break;
                ucase("VOL"):
                  rightVal = vol;
                  hasVal = true;
                  break;
              }
            }
            if (hasVal == false) continue;
            bool doExit = false;
            uswitch(exitItem.Verb){
              ucase("above"):
              ucase(">"):
                doExit = leftVal > rightVal;
                break;
              ucase("below"):
              ucase("<"):
                doExit = leftVal < rightVal;
                break;
              ucase("equal"):
              ucase("="):
                doExit = leftVal == rightVal;
            }
            if (doExit == true)
            {
              item.m_closeTime = dtDbl;
              item.m_closePrice = bid;
              item.m_isOpen = false;
              buytradeCnt--;
              std::cout << "closing buy trade." << std::endl;
            }
          }
        }
        //Sell trades
        if (item.m_isBuyTrade == false)
        {
          std::vector<Exit> vec = m_exitRules.m_exitRules["SELL_EXIT"];
          for (auto exitItem: vec) {
            double leftVal = 0.0;
            bool hasVal = false;
            if (data.has_column(exitItem.Ind1.c_str())){
              leftVal = prev_row.at<double>(data.col_name_to_idx(exitItem.Ind1.c_str()));
              hasVal = true;
            }
            else {
              uswitch(exitItem.Ind1){
                ucase("BID"):
                  leftVal = bid;
                  hasVal = true;
                  break;
                ucase("ASK"):
                  leftVal = ask;
                  hasVal = true;
                  break;
                ucase("VOL"):
                  leftVal = vol;
                  hasVal = true;
                  break;
              }
            }
            if (hasVal == false) continue;
            double rightVal = 0.0;
            hasVal = false;
            if (data.has_column(exitItem.Ind2.c_str())){
              rightVal = prev_row.at<double>(data.col_name_to_idx(exitItem.Ind2.c_str()));
              hasVal = true;
            }
            else {
              uswitch(exitItem.Ind2){
                ucase("BID"):
                  rightVal = bid;
                  hasVal = true;
                  break;
                ucase("ASK"):
                  rightVal = ask;
                  hasVal = true;
                  break;
                ucase("VOL"):
                  rightVal = vol;
                  hasVal = true;
                  break;
              }
            }
            if (hasVal == false) continue;
            bool doExit = false;
            uswitch(exitItem.Verb){
              ucase("above"):
              ucase(">"):
                doExit = leftVal > rightVal;
                break;
              ucase("below"):
              ucase("<"):
                doExit = leftVal < rightVal;
                break;
              ucase("equal"):
              ucase("="):
                doExit = leftVal == rightVal;
            }
            if (doExit == true)
            {
              item.m_closeTime = dtDbl;
              item.m_closePrice = ask;
              item.m_isOpen = false;
              selltradeCnt--;
              std::cout << "closing sell trade." << std::endl;
            }
          }
        }
      }
      //monitorTrades
      std::cout << '\r' <<  std::setprecision(16) << rec_epoch << "/" << std::setprecision(16) << dtDbl << std::flush;
    }
    currentRow = currentRow + 1;
    m_tickLoader.goBack();
  }
  std::cout << "Trades done" << std::endl << "+++++++++++" << std::endl << std::endl;
      double profits = 0.0;
      double totalprofit = 0.0;
      int losecnt = 0;
      double loseAmt = 0.0;
      int wincnt = 0;
      double winAmt = 0.0;
      int eqcnt = 0;
      double acval = m_accountStartBalance;
      std::vector<double> accmove{m_accountStartBalance};
      for (auto it = m_trades.begin(); it != m_trades.end(); ++it) {
        Trade& item = *it;
        profits = (item.m_isBuyTrade ? (item.m_closePrice - item.m_openPrice):(item.m_openPrice - item.m_closePrice));
        if (profits > 0) {wincnt++; winAmt += profits;}
        if (profits < 0) {losecnt++; loseAmt += profits;}
        if (profits == 0.0) eqcnt++;
        acval += profits * 100000;
        totalprofit += profits;
        accmove.push_back(acval);
        std::cout << "Trade: " << item.m_tradeId << std::endl;
        std::cout << "\tBuy/Sell:   " << (item.m_isBuyTrade ? "Buy":"Sell") << std::endl;
        std::cout << "\tOpen Price: " << item.m_openPrice << std::endl;
        std::cout << "\tClose Price:" << item.m_closePrice << std::endl;
        std::cout << "\tProfit:     " << (item.m_isBuyTrade ? (item.m_closePrice - item.m_openPrice):(item.m_openPrice - item.m_closePrice)) << std::endl;
       }
       std::ofstream resout("results.csv");
       int reccnt = 0;
       for (auto item: accmove) {
        resout << reccnt++ << "," << item << std::endl;
       }


  std::cout << "Total profits:  " << totalprofit << " pips" << std::endl;
  std::cout << "Total profits: $" << totalprofit * 1000000 << std::endl;
  std::cout << "Win/Loss:       " << wincnt << "/" << losecnt << "  ratio=" << wincnt/losecnt*100.0 << " %" << std::endl;
  std::cout << "Total: " << data.get_memory_usage<double>("datetime").column_used_memory * data.shape().second << std::endl;
  std::cout << "Memory usage of data1: " << data.get_memory_usage<double>("datetime")  << std::endl;

}
