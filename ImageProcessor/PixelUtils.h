#pragma once

/**
 * @file PixelUtils.h
 * @brief 여러 필터가 공유하는 픽셀 계산 함수.
 */

#include <cstdint>

namespace ip {

/**
 * @brief BGR 값을 ITU-R BT.601 가중치로 밝기(0~255)로 변환한다.
 *
 * Y = 0.299 R + 0.587 G + 0.114 B 를 정수 연산(반올림)으로 계산한다.
 */
inline std::uint8_t luminance(std::uint8_t b, std::uint8_t g, std::uint8_t r) noexcept {
    return static_cast<std::uint8_t>((299 * r + 587 * g + 114 * b + 500) / 1000);
}

} // namespace ip
