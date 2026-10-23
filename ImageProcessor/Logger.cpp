/**
 * @file Logger.cpp
 */

#include "Logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <stdexcept>

namespace ip {

void Logger::open(const std::string& path) {
    m_file.open(path, std::ios::app);
    if (!m_file) {
        throw std::runtime_error("Cannot open log file: " + path);
    }
}

void Logger::log(const std::string& level, const std::string& message) noexcept {
    if (!m_file.is_open()) {
        return;
    }
    try {
        const std::time_t now =
            std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm local{};
        localtime_s(&local, &now);

        m_file << '[' << std::put_time(&local, "%Y-%m-%d %H:%M:%S") << "] "
               << '[' << level << "] " << message << '\n';
        m_file.flush();
    }
    catch (...) {
        // 로그 실패가 본 작업을 중단시키지 않도록 무시한다.
    }
}

} // namespace ip
