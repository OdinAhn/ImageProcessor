/**
 * @file FilterBase.cpp
 */

#include "FilterBase.h"
#include "Exceptions.h"

#include <algorithm>
#include <exception>
#include <thread>
#include <vector>

namespace ip {

void FilterBase::apply(ImageBuffer& image, int threadCount) const {
    if (image.empty()) {
        throw FilterError(name() + ": cannot apply to an empty image");
    }
    if (threadCount < 1) {
        throw FilterError(name() + ": thread count must be >= 1 (got " +
                          std::to_string(threadCount) + ")");
    }

    // 주변 픽셀을 참조하는 필터(blur 등)가 이미 바뀐 값을 읽지 않도록 원본을 보존한다.
    const ImageBuffer source = image;
    const int height = image.height();
    const int workers = std::min(threadCount, height);

    if (workers == 1) {
        processRows(source, image, 0, height);
        return;
    }

    // ── 행을 workers 개 구간으로 균등 분할 ─────────────────
    std::vector<std::thread> threads;
    std::vector<std::exception_ptr> errors(workers);
    threads.reserve(workers);

    const int rowsPerWorker = height / workers;
    const int remainder     = height % workers;
    int yBegin = 0;

    for (int i = 0; i < workers; ++i) {
        // 나머지 행은 앞쪽 쓰레드에 하나씩 더 배분한다.
        const int yEnd = yBegin + rowsPerWorker + (i < remainder ? 1 : 0);
        threads.emplace_back([this, &source, &image, &errors, i, yBegin, yEnd]() {
            // 쓰레드 밖으로 예외가 빠져나가면 std::terminate 되므로 잡아서 전달한다.
            try {
                processRows(source, image, yBegin, yEnd);
            }
            catch (...) {
                errors[i] = std::current_exception();
            }
        });
        yBegin = yEnd;
    }

    for (std::thread& t : threads) {
        t.join();
    }

    for (const std::exception_ptr& error : errors) {
        if (error) {
            std::rethrow_exception(error);
        }
    }
}

} // namespace ip
