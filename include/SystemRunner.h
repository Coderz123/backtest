#include <iostream>
#include <map>
#include "IndicatorDef.h"
#include "Triggers.h"
#include "Exits.h"
#include "TradeStats.h"
#include "tickloader.h"
#include "Jinx.hpp"

#include <DataFrame/DataFrame.h>
#include <DataFrame/DataFrameFinancialVisitors.h>  // Financial algorithms
#include <DataFrame/DataFrameMLVisitors.h>         // Machine-learning algorithms
#include <DataFrame/DataFrameStatsVisitors.h>      // Statistical algorithms
#include <DataFrame/Utils/DateTime.h>              // Cool and handy date-time object
using namespace hmdf;
using dblDataFrame = StdDataFrame<double>;

class SystemRunner
{
	public:
		SystemRunner(){};
		~SystemRunner(){};
		void run();
		void setPath(std::string _path);
		void setSystem(std::string _system);
		void setInstrument(std::string _instr);
		void setTimeframe(int _tf);
		void setTickFile(std::string _file);
		void readIndicators();
		void readTriggers();
		void readExits();
		void indInfo(std::string _name);
		void processIndicators();
		void processRules();
		void processTriggers();
		void processOperations();
		void processExits();
		void Execute();
		std::string getScriptText(std::string _name);
		void applyTimeFilter();

	private:
		StdDataFrame<double> data;
		std::string m_path; //Path to the main config file
		std::string m_system;
		std::string m_instrument;
		std::string m_resultFile;
		std::map<std::string, std::string> m_sysfiles;
		std::map<std::string, std::string> m_indicators;
		std::map<std::string, std::string> m_triggers;
		std::map<std::string, std::string> m_exits;
		std::string m_sysName;
		std::string m_sysDesc;
		std::string m_indFile;
		std::string m_triggerFile;
		std::string m_exitFile;
		std::string m_sysPath; //Path to the actual system
		std::string m_scriptPath;
		double m_pipValue;
		double m_accountStartBalance;
		std::string m_accCurrency;
		std::string m_accountMargin;
		double m_accountMarginCall;
		double m_tradeMaxLots;
		double m_tradeMaxPercentage;
		std::string m_tickFile;
		int m_timeFrame;
		int m_startLine = 0;

		IndicatorDef m_indicatorDefs;
		Rules m_rules;
		Exits m_exitRules;

		//Trading and Results
		std::vector<Trade> m_trades;
		TradeStats m_tradeStats;
		Tickloader m_tickLoader;
};
