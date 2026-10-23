/**
 * @file BlurFilter.cpp
 */

#include "BlurFilter.h"
#include "Exceptions.h"

#include <algorithm>

namespace ip {

BlurFilter::BlurFilter(int radius)
    : m_radius(radius)
{
    if (radius < 1 || radius > MAX_RADIUS) {
        throw FilterError("blur radius must be in [1, " + std::to_string(MAX_RADIUS) +
                          "] (got " + std::to_string(radius) + ")");
    }
}

std::string BlurFilter::name() const {
    return "blur:" + std::to_string(m_radius);
}

void BlurFilter::processRows(const ImageBuffer& src, ImageBuffer& dst,
                             int yBegin, int yEnd) const {
    const int width  = src.width();
    const int height = src.height();
    const int kernelArea = (2 * m_radius + 1) * (2 * m_radius + 1);

    for (int y = yBegin; y < yEnd; ++y) {
        std::uint8_t* out = dst.rowPtr(y);

        for (int x = 0; x < width; ++x) {
            int sum[ImageBuffer::CHANNELS] = {0, 0, 0};

            for (int dy = -m_radius; dy <= m_radius; ++dy) {
                const int sy = std::clamp(y + dy, 0, height - 1);
                const std::uint8_t* in = src.rowPtr(sy);

                for (int dx = -m_radius; dx <= m_radius; ++dx) {
                    const int sx = std::clamp(x + dx, 0, width - 1);
                    const int i = sx * ImageBuffer::CHANNELS;
                    for (int c = 0; c < ImageBuffer::CHANNELS; ++c) {
                        sum[c] += in[i + c];
                    }
                }
            }

            const int o = x * ImageBuffer::CHANNELS;
            for (int c = 0; c < ImageBuffer::CHANNELS; ++c) {
                // 반올림하여 평균을 구한다.
                out[o + c] = static_cast<std::uint8_t>((sum[c] + kernelArea / 2) / kernelArea);
            }
        }
    }
}

} // namespace ip
