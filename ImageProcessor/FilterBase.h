#pragma once

/**
 * @file FilterBase.h
 * @brief 모든 이미지 필터의 추상 기반 클래스.
 */

#include "ImageBuffer.h"

#include <string>

namespace ip {

/**
 * @brief 이미지 필터 인터페이스 (Template Method 패턴).
 *
 * apply() 는 원본을 복사해 두고 행 단위로 작업을 나누어
 * 파생 클래스의 processRows() 를 호출한다.
 * 파생 클래스는 "원본(src)의 행 [yBegin, yEnd) 를 읽어 결과(dst)의 같은 행에 쓴다"
 * 만 구현하면 되므로, 단일/멀티쓰레드 여부를 신경 쓰지 않아도 된다.
 */
class FilterBase {
public:
    virtual ~FilterBase() = default;

    /// 로그/출력용 필터 이름 (예: "threshold:128").
    virtual std::string name() const = 0;

    /**
     * @brief 이미지에 필터를 적용한다.
     * @param image       입력이자 출력 이미지.
     * @param threadCount 사용할 쓰레드 수 (1 이상). 행 수보다 크면 행 수로 줄인다.
     * @throws FilterError 이미지가 비어 있거나 쓰레드 수가 잘못된 경우.
     */
    void apply(ImageBuffer& image, int threadCount = 1) const;

protected:
    /**
     * @brief src 의 [yBegin, yEnd) 행을 처리하여 dst 의 같은 행에 기록한다.
     *
     * 서로 다른 쓰레드가 겹치지 않는 행 범위로 동시에 호출하므로,
     * dst 의 해당 범위 밖을 쓰거나 멤버 상태를 변경해서는 안 된다.
     */
    virtual void processRows(const ImageBuffer& src, ImageBuffer& dst,
                             int yBegin, int yEnd) const = 0;
};

} // namespace ip
