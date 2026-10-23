/**
 * @file ThresholdFilter.cpp
 */

#include "ThresholdFilter.h"
#include "Exceptions.h"
#include "PixelUtils.h"

namespace ip {

ThresholdFilter::ThresholdFilter(int threshold)
    : m_threshold(threshold)
{
    if (threshold < MIN_THRESHOLD || threshold > MAX_THRESHOLD) {
        throw FilterError("threshold must be in [" + std::to_string(MIN_THRESHOLD) +
                          ", " + std::to_string(MAX_THRESHOLD) + "] (got " +
                          std::to_string(threshold) + ")");
    }
}

std::string ThresholdFilter::name() const {
    return "threshold:" + std::to_string(m_threshold);
}

void ThresholdFilter::processRows(const ImageBuffer& src, ImageBuffer& dst,
                                  int yBegin, int yEnd) const {
    const int width = src.width();

    for (int y = yBegin; y < yEnd; ++y) {
        const std::uint8_t* in  = src.rowPtr(y);
        std::uint8_t*       out = dst.rowPtr(y);

        for (int x = 0; x < width; ++x) {
            const int i = x * ImageBuffer::CHANNELS;
            const bool isBright = luminance(in[i], in[i + 1], in[i + 2]) >= m_threshold;
            const std::uint8_t value = isBright ? 255 : 0;
            out[i] = out[i + 1] = out[i + 2] = value;
        }
    }
}

} // namespace ip
