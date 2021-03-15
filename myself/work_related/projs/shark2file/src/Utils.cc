// Created by skymelody on 2019/12/24.
//

#include <chrono>
#include <string>
#include <iomanip>
#include <sstream>

std::string CurrentTimeToStr() {
	auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::gmtime(&tt);
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y_%m_%d_%H_%M_%S");
    return ss.str();
}

