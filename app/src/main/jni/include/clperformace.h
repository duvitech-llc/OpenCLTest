/*
                                _                       
                       __ _  __| |_   _____ _ __   __ _ 
                      / _` |/ _` \ \ / / _ \ '_ \ / _` |
                     | (_| | (_| |\ V /  __/ | | | (_| |
                      \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                  |___/ 
*/
#ifndef _CLPERFORMACE_H
#define _CLPERFORMACE_H
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <ratio>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <iomanip>

class pref_rec {
private:
	std::fstream m_log;
	unsigned int index;
protected:
public:
	void open(const char *filename) {
		m_log.open(filename, std::fstream::in | std::fstream::out | std::fstream::trunc);
		m_log << "catch delay samples" << std::endl;
		m_log << "-------------------------------------------------" << std::endl;
		m_log << "index,source,function,text,ms_start,ms_end,ms_diff" << std::endl;
	};
	void close() {
		m_log.flush();
		m_log.close();
	};
	void log(const char* file, const char* func, const char* txt, unsigned long start, unsigned long end) {
		unsigned long diffdelay = end - start;
		m_log << index++;
		m_log << "," << file;
		m_log << "," << func;
		m_log << "," << txt;
		m_log << "," << (unsigned long)start;
		m_log << "," << (unsigned long)end;
		m_log << "," << diffdelay;
		m_log << std::endl;
	}
};
#define PERF_LOCATION(file, func, txt) "perf[" + std::string(txt) + "](" + std::string(file) + ")"
class perf
{
private:
#ifdef _WITH_PERF_LOGDUMP
	std::fstream dump;
#endif //_WITH_PERF_LOGDUMP

#ifdef _WITH_PREF_REC
	pref_rec m_rec;
#endif //_WITH_PREF_REC
	unsigned int index;
protected:
	std::map<std::string, std::chrono::microseconds> mapDelay;
	std::map<std::string, std::chrono::system_clock::time_point> mapDelayStart;
public:
	perf(const char *filename) {
		index = 0;
#ifdef _WITH_PERF_LOGDUMP
		dump.open("dump.pref", std::fstream::in | std::fstream::out | std::fstream::trunc);
#endif //_WITH_PERF_LOGDUMP
#ifdef _WITH_PREF_REC
		m_rec.open(filename);
#endif //_WITH_PREF_REC
	};
	~perf() {
		show();
#ifdef _WITH_PREF_REC
		m_rec.close();
#endif //_WITH_PREF_REC
#ifdef _WITH_PERF_LOGDUMP
		dump.close();
#endif //_WITH_PERF_LOGDUMP
	};
	inline void start(const char* file, const char* func, const char* txt) {
		std::string str = PERF_LOCATION(file, func, txt);
		mapDelayStart[str] = std::chrono::system_clock::now();
	};
	inline unsigned long end(const char* file, const char* func, const char* txt) {
		std::string str = PERF_LOCATION(file, func, txt);
		std::chrono::system_clock::time_point start = mapDelayStart[str];
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

		std::chrono::microseconds delay = std::chrono::duration_cast<std::chrono::microseconds>(now - start);
		div_t res;
		if(mapDelay[str].count() > 0) {
			long a = delay.count() + mapDelay[str].count();
			res = div(a, 2);
			delay = std::chrono::microseconds(res.quot);
		}
		mapDelay[str] = delay;

		typedef std::chrono::duration<int, std::milli> milliseconds_type;
		std::chrono::time_point<std::chrono::system_clock, milliseconds_type> Start = std::chrono::time_point_cast<milliseconds_type>(start);
		std::chrono::time_point<std::chrono::system_clock, milliseconds_type> End = std::chrono::time_point_cast<milliseconds_type>(now);

		unsigned long start_x = (unsigned long)Start.time_since_epoch().count();
		unsigned long end_x = (unsigned long)End.time_since_epoch().count();
#ifdef _WITH_PREF_REC
		m_rec.log(file, func, txt, start_x, end_x);
#endif //_WITH_PREF_REC
		return end_x - start_x;
	};
	void show() {
		std::chrono::time_point<std::chrono::system_clock> now;
		now = std::chrono::system_clock::now();
		std::time_t now_time = std::chrono::system_clock::to_time_t(now);
		std::cout << std::endl;
		std::cout << "report time: " << std::ctime(&now_time) << std::endl;
		std::cout << "===performance averages===" << std::endl;
		std::cout << std::endl;
		for (std::map<std::string, std::chrono::microseconds>::iterator it = mapDelay.begin(); it != mapDelay.end(); ++it) {
			time_t tt;
			auto uavg = std::chrono::duration_cast<std::chrono::microseconds>(it->second).count();
			auto mavg = std::chrono::duration_cast<std::chrono::milliseconds>(it->second).count();
			auto savg = std::chrono::duration_cast<std::chrono::seconds>(it->second).count();
			std::cout << it->first;
			std::cout << " => ";
			std::cout << uavg;
			std::cout << "us ";
			std::cout << mavg;
			std::cout << "ms ";
			std::cout << savg;
			std::cout << "sec ";
			std::cout << std::endl;

#ifdef _WITH_PERF_LOGDUMP
			dump << it->first;
			dump << " => ";
			dump << uavg;
			dump << "us ";
			dump << mavg;
			dump << "ms ";
			dump << savg;
			dump << "sec ";
			dump << std::endl;
#endif //_WITH_PERF_LOGDUMP
		}
		std::cout << std::endl;
#ifdef _WITH_PERF_LOGDUMP
		dump.flush();
#endif //_WITH_PERF_LOGDUMP
	};
	void report() {
		//TODO:
	};
};


#endif //_CLPERFORMACE_H
