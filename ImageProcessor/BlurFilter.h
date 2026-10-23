#pragma once

/**
 * @file BlurFilter.h
 * @brief 박스 블러(평균 필터)로 이미지를 흐리게 만든다.
 */

#include "FilterBase.h"

namespace ip {

/**
 * @brief (2r+1) x (2r+1) 영역의 평균으로 각 픽셀을 대체한다.
 *
 * 이미지 경계 밖의 픽셀은 가장 가까운 경계 픽셀로 간주한다 (clamp).
 */
class BlurFilter : public FilterBase {
public:
    static constexpr int DEFAULT_RADIUS = 1;
    static constexpr int MAX_RADIUS     = 20;

    /**
     * @param radius 커널 반경 [1, MAX_RADIUS].
     * @throws FilterError 범위를 벗어난 경우.
     */
    explicit BlurFilter(int radius = DEFAULT_RADIUS);

    std::string name() const override;

protected:
    void processRows(const ImageBuffer& src, ImageBuffer& dst,
                     int yBegin, int yEnd) const override;

private:
    int m_radius;
};

} // namespace ip
