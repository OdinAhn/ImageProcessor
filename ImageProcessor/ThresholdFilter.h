#pragma once

/**
 * @file ThresholdFilter.h
 * @brief 밝기 임계값으로 이미지를 흑/백 두 값으로 이진화한다.
 */

#include "FilterBase.h"

namespace ip {

/// 밝기가 임계값 이상이면 흰색(255), 미만이면 검은색(0)으로 만든다.
class ThresholdFilter : public FilterBase {
public:
    static constexpr int MIN_THRESHOLD = 0;
    static constexpr int MAX_THRESHOLD = 255;

    /**
     * @param threshold 임계값 [0, 255].
     * @throws FilterError 범위를 벗어난 경우.
     */
    explicit ThresholdFilter(int threshold);

    std::string name() const override;

protected:
    void processRows(const ImageBuffer& src, ImageBuffer& dst,
                     int yBegin, int yEnd) const override;

private:
    int m_threshold;
};

} // namespace ip
