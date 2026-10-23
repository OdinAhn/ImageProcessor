/**
 * @file CommandLineParser.cpp
 */

#include "CommandLineParser.h"
#include "Exceptions.h"

#include <charconv>
#include <iostream>
#include <string>

namespace ip {

namespace {
    /// argv 의 다음 인자를 안전하게 가져온다.
    std::string nextArg(int argc, char* argv[], int& i, const std::string& flag) {
        if (i + 1 >= argc) {
            throw ArgumentError(flag + ": missing value");
        }
        return argv[++i];
    }

    /// 쓰레드 수 문자열을 검증하여 정수로 변환한다.
    int parseThreadCount(const std::string& text) {
        int value = 0;
        const char* last = text.data() + text.size();
        const auto [ptr, ec] = std::from_chars(text.data(), last, value);
        if (text.empty() || ec != std::errc() || ptr != last ||
            value < 1 || value > ProgramOptions::MAX_THREADS) {
            throw ArgumentError("--threads: must be an integer in [1, " +
                                std::to_string(ProgramOptions::MAX_THREADS) +
                                "] (got \"" + text + "\")");
        }
        return value;
    }
} // anonymous namespace

ProgramOptions CommandLineParser::parse(int argc, char* argv[]) {
    ProgramOptions options;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "--input" || arg == "-i") {
            options.inputPath = nextArg(argc, argv, i, arg);
        }
        else if (arg == "--output" || arg == "-o") {
            options.outputPath = nextArg(argc, argv, i, arg);
        }
        else if (arg == "--filter" || arg == "-f") {
            options.filterName = nextArg(argc, argv, i, arg);
        }
        else if (arg == "--pipeline" || arg == "-p") {
            options.pipeline = nextArg(argc, argv, i, arg);
        }
        else if (arg == "--threads" || arg == "-t") {
            options.threadCount = parseThreadCount(nextArg(argc, argv, i, arg));
        }
        else if (arg == "--log" || arg == "-l") {
            options.logPath = nextArg(argc, argv, i, arg);
        }
        else if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            std::exit(0);
        }
        else {
            throw ArgumentError("Unknown option: " + arg);
        }
    }

    // 필수 인자 검증
    if (options.inputPath.empty()) {
        throw ArgumentError("--input is required");
    }
    if (options.outputPath.empty()) {
        throw ArgumentError("--output is required");
    }
    if (options.filterName.empty() == options.pipeline.empty()) {
        throw ArgumentError("exactly one of --filter or --pipeline is required");
    }

    return options;
}

void CommandLineParser::printUsage(const std::string& exeName) {
    std::cout
        << "Usage:\n"
        << "  " << exeName << " --input <path> --output <path> --filter <name> [options]\n"
        << "  " << exeName << " --input <path> --output <path> --pipeline \"<a>, <b>, ...\" [options]\n\n"
        << "Options:\n"
        << "  -i, --input    <path>   Input BMP file (24-bit, uncompressed)\n"
        << "  -o, --output   <path>   Output BMP file\n"
        << "  -f, --filter   <name>   Filter to apply\n"
        << "  -p, --pipeline <list>   Comma-separated filters applied in order\n"
        << "  -t, --threads  <n>      Worker threads per filter (1-"
        << ProgramOptions::MAX_THREADS << ", default 1)\n"
        << "  -l, --log      <path>   Append processing log to file\n"
        << "  -h, --help              Show this message\n\n"
        << "Filters:\n"
        << "  grayscale               Convert to grayscale (BT.601 luminance)\n"
        << "  threshold:<0-255>       Binarize by luminance\n"
        << "  blur[:<radius 1-20>]    Box blur (default radius 1)\n\n"
        << "Examples:\n"
        << "  " << exeName << " -i input.bmp -o result.bmp -f grayscale\n"
        << "  " << exeName << " -i input.bmp -o result.bmp -f threshold:128\n"
        << "  " << exeName << " -i input.bmp -o result.bmp -p \"grayscale, blur:2, threshold:128\" -t 4 -l run.log\n";
}

} // namespace ip
