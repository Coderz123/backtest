#ifndef UTILS_H_
#define UTILS_H_

#include <ctime>
#include <chrono>
#include <string>
#include <sstream>
#include <DataFrame/Utils/DateTime.h>
// #include "date.h"

class DateUtil
{
	public:
		static double epochFromDateStr(std::string _dateStr){
			std::string yearStr = _dateStr.substr(0,4);
			std::string monthStr = _dateStr.substr(4,2);
		    std::string dayStr = _dateStr.substr(6,2);
			std::string hourStr = _dateStr.substr(9,2);
			std::string minStr = _dateStr.substr(11,2);
			std::string secStr = _dateStr.substr(13,2);
			std::string msecStr = _dateStr.substr(15);
			// std::cout << "year: " << yearStr << "\nmonth: " << monthStr << "\nday: " << dayStr
				// << "\nhour: " << hourStr << "\nmin: " << minStr << "\nmsec: " << secStr << "\nmsec: " << msecStr << std::endl;
			std::stringstream ss;
			ss << yearStr << "/" << monthStr << "/" << dayStr << " " << hourStr << ":" << minStr << ":" << secStr << "." << msecStr;


			hmdf::DateTime dt(ss.str().c_str(), (hmdf::DT_DATE_STYLE)3);
			std::string tmpDt = dt.string_format(hmdf::DT_FORMAT::DT_PRECISE);
			std::stringstream dbstr;
			dbstr << tmpDt;
			double retVal;
			dbstr >> retVal;
			return retVal;
		}

		// static std::string DateStrFromEpoch(double _epoch) {
		// 	hmdf::DateTime dt;
		// 	dt = _epoch;
		// 	return dt.string_format(hmdf::DT_FORMAT::ISO_DT_TM);
		// }

		static std::string getTimeStamp(time_t epochTime, const char* format = "%Y%m%d %H%M%S")
		{
			char timestamp[64] = {0};
			strftime(timestamp, sizeof(timestamp), format, localtime(&epochTime));
			return timestamp;
		}

		static time_t convertTimeToEpoch(const char* theTime, const char* format = "%Y-%m-%d %H:%M:%S")
		{
			std::tm tmTime;
			memset(&tmTime, 0, sizeof(tmTime));
			strptime(theTime, format, &tmTime);
			return mktime(&tmTime);
		}
};

#endif // UTILS_H_
