#pragma once

/**
 * @file Logger.h
 * @brief 처리 과정을 타임스탬프와 함께 파일에 기록한다.
 */

#include <fstream>
#include <string>

namespace ip {

/**
 * @brief 파일 로거.
 *
 * open() 을 호출하기 전이나 로그 경로가 없으면 log() 는 아무 것도 하지 않는다.
 * 예외 처리 경로에서도 호출되므로 log() 는 예외를 던지지 않는다.
 */
class Logger {
public:
    /**
     * @brief 로그 파일을 추가(append) 모드로 연다.
     * @throws std::runtime_error 파일을 열 수 없는 경우.
     */
    void open(const std::string& path);

    /// "[YYYY-MM-DD HH:MM:SS] [LEVEL] message" 형식으로 한 줄 기록한다.
    void log(const std::string& level, const std::string& message) noexcept;

    void info(const std::string& message) noexcept  { log("INFO",  message); }
    void error(const std::string& message) noexcept { log("ERROR", message); }

private:
    std::ofstream m_file;
};

} // namespace ip
