#pragma once

/**
 * @file FilterFactory.h
 * @brief 문자열 명세로부터 필터 객체를 생성한다.
 *
 * 필터 명세 형식: "<name>" 또는 "<name>:<int>"
 *   - grayscale
 *   - threshold:<0~255>   (값 생략 불가)
 *   - blur[:<radius>]     (기본 반경 1)
 *
 * 파이프라인 명세 형식: 쉼표로 구분한 필터 명세 목록
 *   예) "grayscale, blur:2, threshold:128"
 */

#include "FilterBase.h"

#include <memory>
#include <string>
#include <vector>

namespace ip {

class FilterFactory {
public:
    FilterFactory() = delete;  // 인스턴스화 금지 (정적 메서드만 제공)

    /**
     * @brief 필터 명세 하나로 필터를 생성한다.
     * @throws FilterError 알 수 없는 이름이거나 파라미터가 잘못된 경우.
     */
    static std::unique_ptr<FilterBase> create(const std::string& spec);

    /**
     * @brief 쉼표로 구분된 명세 목록으로 필터 체인을 생성한다.
     * @throws FilterError 비어 있는 항목이 있거나 개별 명세가 잘못된 경우.
     */
    static std::vector<std::unique_ptr<FilterBase>> createPipeline(const std::string& spec);
};

} // namespace ip
