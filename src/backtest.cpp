#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <ctime>
#include <string>
#include <filesystem>
#include "backtest.h"
#include <cxxtools/arg.h>
#include "date.h"
#include "csv.h"
#include "SystemRunner.h"

#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "grid-container.hpp"
#include "indicators.h"

// Define a struct to store bid-related values
struct BidInfo {
    double first_bid;
    double highest_bid;
    double lowest_bid;
    double last_bid;
    double cumulative_volume;
};

using namespace ftxui;

ButtonOption Style() {
    auto option = ButtonOption::Animated();
	option.transform = [](const EntryState& s) {
	    auto element = text(s.label);
		if (s.focused) {
		    element |= bold;
		};
		return element | center | borderEmpty | flex;
	};
	return option;
}

void print_help()
{
	std::cout << "Usage" << std::endl;
	std::cout << "=====" << std::endl << std::endl;
	std::cout << "backtest command [options]" << std::endl << std::endl;
	std::cout << "Commands" << std::endl << "========" << std::endl << std::endl;
	std::cout << "config        Run interactive tui to set up the backtest." << std::endl;
	std::cout << "timeframe     Produce specified timeframe data." << std::endl;
	std::cout << "run           Run the backtest." << std::endl << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "-h            Print this help message." << std::endl;
	std::cout << "-v            Produce verbose output to the console."
		<< std::endl;
	std::cout << "-f filename   Specify the Tick data filename to be processed."
		<< std::endl;
	std::cout << "-o filename   Specify the output filename." << std::endl;
	std::cout << "-t timeframe  Specify the timeframe to be used for "
		"trading in minutes (default=5)"
		<< std::endl;
	std::cout << "-s system     Specify the name of the system to run." << std::endl;
	std::cout << "-p path       Specify the absolute path of the system location." << std::endl;
	std::cout << "-i pair       Specify the instrument/currency pair to use." << std::endl;

    std::cout << std::endl;
}

int main(int argc, char **argv) {
	cxxtools::Arg<bool> help(argc, argv, "--help");
	cxxtools::Arg<bool> helpS(argc, argv, "-h");

	if (help || helpS) {
		print_help();
		return 0;
	}

	cxxtools::Arg<bool> verbose(argc, argv, "-v");
	cxxtools::Arg<bool> cmd(argc, argv, "config");
	cxxtools::Arg<bool> cmdRun(argc, argv, "run");
	cxxtools::Arg<bool> cmdTf(argc, argv, "timeframe");
	cxxtools::Arg<std::string> filename(
			argc, argv, "-f",
			"/home/cv/Downloads/Data/Forex/EURUSD/DAT_ASCII_EURUSD_T_202301.csv");
	// 20230101 170401067,1.069700,1.070920,0

	//std::cout << "Config Command: " << cmd << std::endl;

	if (cmd)
	{
		std::cout << "Running configuration..." << std::endl << std::endl;
		std::vector<std::string> indList;
		//Configure the system.
		const ti_indicator_info *info = ti_indicators;
		while (info->name != 0) {
		   indList.push_back(info->full_name);
		   ++info;
		}
		int selected_0 = 0;
		auto indDd = Dropdown(&indList, &selected_0) | flex;
		auto Btn = Button("Exit", [&] {Style();});
		auto layout = GridContainer({{indDd}, {Btn}});


		auto screen = ScreenInteractive::FitComponent();
		screen.Loop(layout);

	}
	else if (cmdRun)
	{
		cxxtools::Arg<std::string> system(argc, argv, "-s");
		cxxtools::Arg<std::string> syspath(argc, argv, "-p");
		cxxtools::Arg<std::string> instr(argc, argv, "-i");
		cxxtools::Arg<int> tf(argc, argv, "-t");
		//Run the configured backtest
		/* Set info to first indicators in array. */
		const ti_indicator_info *info = ti_indicators;

		/* The last item is all zeros, so loop until then. */
		 printf("%s,%s\n", "Name", "Description");
		while (info->name != 0) {
			/* The last item is all zeros, so we'll stop when we get there. */
			 int i;
			 printf("%s,%s\n", info->name, info->full_name);
			 for (int i = 0; i < info->inputs ; i++)
			 	printf("    %s\n", info->input_names[i]);
			 for (int i = 0; i < info->options ; i++)
			 	printf("        %s\n", info->option_names[i]);
			++info;
/* Next indicator. */
		}

		if (!syspath.isSet())
		{
			std::cout << "Error: Path is not set." << std::endl;
			return 0;
		}
		if (!system.isSet())
		{
			std::cout << "Error: System name not set." << std::endl;
			return 0;
		}
		if (!instr.isSet())
		{
			std::cout << "Note: Instrument is not set." << std::endl;
			return 0;
		}
		if (!tf.isSet())
		{
			std::cout << "Note: Timeframe is not set." << std::endl;
			return 0;
		}

		SystemRunner runner;
		runner.setPath(syspath.getValue());
		runner.setSystem(system.getValue());
		runner.setInstrument(instr.getValue());
		runner.setTimeframe(tf.getValue());
		runner.setTickFile(filename.getValue());
		runner.run();
	}
	else if (cmdTf)
	{
		cxxtools::Arg<int> timeframe(argc, argv, "-t", 5);
		cxxtools::Arg<std::string> outfilename(argc, argv, "-o", "./output.csv");
		std::cout << "Backtest..." << std::endl;
		std::cout << "Running on " << timeframe
			<< " minute bars and processing data file " << std::endl
			<< filename.getValue() << std::endl;
		std::cout << "Converting data..." << std::endl;
		std::stringstream ss;
		ss << timeframe << "_" << outfilename;
		std::string outName = ss.str();
		std::string outtmp = outName + ".tmp";
		int cnt = 0;

		{
			io::CSVReader<4> in(filename.getValue());
			// in.read_header(io::ignore_extra_column, "dt", "bid", "ask",
			// "vol");

			std::string line;
			std::map<time_t, BidInfo>
				interval_data; // Map to store data for each interval
			std::string datetime_str;
			double bid, ask, volume;

			// Write the result to a CSV file without column headers
			std::ofstream outfile(outtmp /*"output_intervals.csv"*/);
			if (!outfile) {
				std::cerr << "Error opening output file." << std::endl;
				return 1;
			}
			std::cout << "Saving to " << outName << std::endl;
			while (in.read_row(datetime_str, bid, ask, volume)) {
				// Convert datetime string to a time_t value
				struct tm tm;
				strptime(datetime_str.c_str(), "%Y%m%d %H%M%S%f", &tm);
				time_t timestamp = mktime(&tm);

				// Calculate the interval start time (rounded down to 5
				// minutes)
				int intfilter = timeframe * 60;
				time_t interval_start = (timestamp / intfilter) * intfilter;
				// std::cout << tm.tm_year+1900 << "/" << tm.tm_mon + 1 <<
				// "/"
				//<< tm.tm_mday << " " << tm.tm_hour << ":" << tm.tm_min <<
				//":" << tm.tm_sec << std::endl; 		struct tm* ptm;
				// ptm = gmtime(&interval_start); 	    char buffer [80];
				//       strftime (buffer,80,"Now it's %I:%M%p.",ptm);
				//       puts (buffer);

				// Update or create entry in the map
				if (interval_data.find(interval_start) ==
					interval_data.end()) {
					for (const auto &entry : interval_data) {
					const BidInfo &info = entry.second;
					cnt++;
					outfile << cnt << "," << entry.first << "," << info.first_bid << ","
							<< info.highest_bid << "," << info.lowest_bid
							<< "," << info.last_bid << ","
							<< info.cumulative_volume << "\n";
					}

					interval_data.clear();
					interval_data[interval_start] = {bid, bid, bid, bid, volume};
				} else {
					BidInfo &info = interval_data[interval_start];
					info.highest_bid = std::max(info.highest_bid, bid);
					info.lowest_bid = std::min(info.lowest_bid, bid);
					info.last_bid = bid;
					info.cumulative_volume += volume;
				}
			}
		}
		std::ofstream outputFile(outName);
		std::ifstream inputFile(outtmp);

		outputFile << std::string("INDEX:") << cnt << ":<double>,datetime:" << cnt << ":<double>,open:" << cnt << ":<double>,high:" << cnt << ":<double>,low:" << cnt << ":<double>,close:" << cnt << ":<double>,volume:" << cnt << ":<double>" << std::endl;
		outputFile << inputFile.rdbuf();

		inputFile.close();
		outputFile.close();

		std::remove(outtmp.c_str());

	}
	else
	{
		print_help();
	}
	return 0;
}
