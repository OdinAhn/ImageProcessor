/**
 * @file FilterFactory.cpp
 */

#include "FilterFactory.h"
#include "BlurFilter.h"
#include "Exceptions.h"
#include "GrayscaleFilter.h"
#include "ThresholdFilter.h"

#include <cctype>
#include <charconv>
#include <optional>
#include <sstream>

namespace ip {

namespace {
    /// 앞뒤 공백을 제거한다.
    std::string trim(const std::string& text) {
        const auto isSpace = [](unsigned char ch) { return std::isspace(ch) != 0; };
        std::size_t begin = 0;
        std::size_t end   = text.size();
        while (begin < end && isSpace(text[begin]))   { ++begin; }
        while (end > begin && isSpace(text[end - 1])) { --end;   }
        return text.substr(begin, end - begin);
    }

    /**
     * @brief 문자열 전체가 10진 정수일 때만 변환한다.
     *
     * std::stoi 는 "12abc" 를 12 로 받아들이므로 from_chars 로 전체 소비 여부까지 검사한다.
     */
    int parseIntParam(const std::string& filterName, const std::string& text) {
        int value = 0;
        const char* last = text.data() + text.size();
        const auto [ptr, ec] = std::from_chars(text.data(), last, value);
        if (text.empty() || ec != std::errc() || ptr != last) {
            throw FilterError(filterName + ": parameter must be an integer (got \"" + text + "\")");
        }
        return value;
    }
} // anonymous namespace

std::unique_ptr<FilterBase> FilterFactory::create(const std::string& spec) {
    const std::string trimmed = trim(spec);
    if (trimmed.empty()) {
        throw FilterError("Empty filter name");
    }

    // ── "name:param" 분리 ──────────────────────────────
    const std::size_t colon = trimmed.find(':');
    const std::string name  = trim(trimmed.substr(0, colon));
    std::optional<int> param;
    if (colon != std::string::npos) {
        param = parseIntParam(name, trim(trimmed.substr(colon + 1)));
    }

    // ── 이름별 생성 ────────────────────────────────────
    if (name == "grayscale") {
        if (param) {
            throw FilterError("grayscale: takes no parameter");
        }
        return std::make_unique<GrayscaleFilter>();
    }
    if (name == "threshold") {
        if (!param) {
            throw FilterError("threshold: value is required (e.g. threshold:128)");
        }
        return std::make_unique<ThresholdFilter>(*param);
    }
    if (name == "blur") {
        return std::make_unique<BlurFilter>(param.value_or(BlurFilter::DEFAULT_RADIUS));
    }

    throw FilterError("Unknown filter: " + name);
}

std::vector<std::unique_ptr<FilterBase>> FilterFactory::createPipeline(const std::string& spec) {
    std::vector<std::unique_ptr<FilterBase>> filters;
    std::istringstream stream(spec);
    std::string item;

    while (std::getline(stream, item, ',')) {
        if (trim(item).empty()) {
            throw FilterError("Pipeline contains an empty step: \"" + spec + "\"");
        }
        filters.push_back(create(item));
    }
    // 마지막 쉼표 뒤 빈 항목("a,b,")은 getline 이 반환하지 않으므로 따로 검사한다.
    if (filters.empty() || trim(spec).back() == ',') {
        throw FilterError("Pipeline contains an empty step: \"" + spec + "\"");
    }

    return filters;
}

} // namespace ip
