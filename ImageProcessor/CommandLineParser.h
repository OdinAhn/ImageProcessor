#pragma once

/**
 * @file CommandLineParser.h
 * @brief 커맨드라인 인자 파싱.
 *
 * 기본 옵션(--input, --output, --filter)과
 * 고급 옵션(--pipeline, --threads, --log)을 파싱한다.
 * --filter 와 --pipeline 은 둘 중 정확히 하나만 지정해야 한다.
 */

#include <string>

namespace ip {

/// 파싱 결과를 담는 단순 구조체.
struct ProgramOptions {
    static constexpr int MAX_THREADS = 64;

    std::string inputPath;        ///< --input    / -i
    std::string outputPath;       ///< --output   / -o
    std::string filterName;       ///< --filter   / -f
    std::string pipeline;         ///< --pipeline / -p  (쉼표로 구분한 필터 목록)
    int         threadCount = 1;  ///< --threads  / -t  [1, MAX_THREADS]
    std::string logPath;          ///< --log      / -l  (비어 있으면 로그 미사용)
};

class CommandLineParser {
public:
    CommandLineParser() = delete;  // 인스턴스화 금지 (정적 메서드만 제공)

    /**
     * @brief argv 를 파싱하여 ProgramOptions 를 반환한다.
     * @throws ArgumentError 필수 인자 누락, 알 수 없는 옵션, 형식 오류 등.
     */
    static ProgramOptions parse(int argc, char* argv[]);

    /// 사용법을 표준 출력에 출력한다.
    static void printUsage(const std::string& exeName);
};

} // namespace ip
