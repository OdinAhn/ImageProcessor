/**
 * @file main.cpp
 * @brief ImageProcessor 진입점.
 *
 * 흐름: CLI 파싱 → 필터 생성 → BMP 로드 → 필터 순차 적용 → BMP 저장.
 * 오류 종류별 종료 코드: 인자 4, BMP 2, 필터 3, 기타 1.
 */

#include "BmpParser.h"
#include "CommandLineParser.h"
#include "ImageBuffer.h"
#include "Exceptions.h"
#include "FilterBase.h"
#include "FilterFactory.h"
#include "Logger.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

int main(int argc, char* argv[]) {
    // 예외 처리 블록에서도 기록할 수 있도록 try 밖에 둔다.
    ip::Logger logger;

    try {
        // ── CLI 인자 파싱 (제공된 코드) ─────────────────────────
        const ip::ProgramOptions options = ip::CommandLineParser::parse(argc, argv);

        if (!options.logPath.empty()) {
            logger.open(options.logPath);
        }
        logger.info("Start: input=" + options.inputPath + ", output=" + options.outputPath +
                    ", threads=" + std::to_string(options.threadCount));

        // ── 필터 생성 ───────────────────────────────────────────
        // 잘못된 필터 명세는 이미지를 읽기 전에 빠르게 실패시킨다.
        std::vector<std::unique_ptr<ip::FilterBase>> filters;
        if (!options.filterName.empty()) {
            filters.push_back(ip::FilterFactory::create(options.filterName));
        }
        else {
            filters = ip::FilterFactory::createPipeline(options.pipeline);
        }

        // ── BMP 로드 (제공된 코드) ──────────────────────────────
        ip::ImageBuffer image = ip::BmpParser::loadFromFile(options.inputPath);
        std::cout << "Loaded: " << image.width() << " x " << image.height() << "\n";
        logger.info("Loaded: " + std::to_string(image.width()) + " x " +
                    std::to_string(image.height()));

        // ── 필터 순차 적용 ──────────────────────────────────────
        for (const std::unique_ptr<ip::FilterBase>& filter : filters) {
            const auto begin = std::chrono::steady_clock::now();
            filter->apply(image, options.threadCount);
            const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - begin).count();

            const std::string message =
                "Applied: " + filter->name() + " (" + std::to_string(elapsedMs) + " ms)";
            std::cout << message << "\n";
            logger.info(message);
        }

        // ── BMP 저장 (제공된 코드) ──────────────────────────────
        ip::BmpParser::saveToFile(options.outputPath, image);
        std::cout << "Saved:  " << options.outputPath << "\n";
        logger.info("Saved: " + options.outputPath);
        return 0;
    }
    catch (const ip::ArgumentError& e) {
        std::cerr << e.what() << "\n\n";
        ip::CommandLineParser::printUsage(argc > 0 ? argv[0] : "ImageProcessor");
        logger.error(e.what());
        return 4;
    }
    catch (const ip::BmpParseError& e) {
        std::cerr << e.what() << std::endl;
        logger.error(e.what());
        return 2;
    }
    catch (const ip::FilterError& e) {
        std::cerr << e.what() << std::endl;
        logger.error(e.what());
        return 3;
    }
    catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        logger.error(std::string("Unexpected error: ") + e.what());
        return 1;
    }
}
