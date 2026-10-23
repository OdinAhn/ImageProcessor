#pragma once

/**
 * @file GrayscaleFilter.h
 * @brief 컬러 이미지를 흑백(그레이스케일)으로 변환한다.
 */

#include "FilterBase.h"

namespace ip {

/// 각 픽셀의 B, G, R 을 같은 밝기 값으로 바꾼다.
class GrayscaleFilter : public FilterBase {
public:
    std::string name() const override { return "grayscale"; }

protected:
    void processRows(const ImageBuffer& src, ImageBuffer& dst,
                     int yBegin, int yEnd) const override;
};

} // namespace ip
