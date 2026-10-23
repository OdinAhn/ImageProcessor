/**
 * @file GrayscaleFilter.cpp
 */

#include "GrayscaleFilter.h"
#include "PixelUtils.h"

namespace ip {

void GrayscaleFilter::processRows(const ImageBuffer& src, ImageBuffer& dst,
                                  int yBegin, int yEnd) const {
    const int width = src.width();

    for (int y = yBegin; y < yEnd; ++y) {
        const std::uint8_t* in  = src.rowPtr(y);
        std::uint8_t*       out = dst.rowPtr(y);

        for (int x = 0; x < width; ++x) {
            const int i = x * ImageBuffer::CHANNELS;
            const std::uint8_t gray = luminance(in[i], in[i + 1], in[i + 2]);
            out[i] = out[i + 1] = out[i + 2] = gray;
        }
    }
}

} // namespace ip
