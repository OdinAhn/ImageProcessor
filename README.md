# ImageProcessor 과제 제출

## 구현 항목

- **필터 3종**
  - `grayscale`: BT.601 가중치(0.299R + 0.587G + 0.114B)로 흑백 변환
  - `threshold:<0~255>`: 밝기 기준 이진화 (임계값 이상 흰색, 미만 검은색)
  - `blur[:<1~20>]`: 박스 블러(평균 필터). 반경 생략 시 1, 경계는 가장 가까운 픽셀로 처리(clamp)
- **추상 클래스 기반 다형성 설계**: `FilterBase` (Template Method 패턴)
  - 공개 `apply()` 가 원본 복사, 행 분할, 쓰레드 관리, 예외 전달을 담당
  - 파생 클래스는 `processRows(src, dst, yBegin, yEnd)` 만 구현
  - `FilterFactory` 가 `"name:param"` 문자열 명세를 해석해 `std::unique_ptr<FilterBase>` 로 생성
- **필터 파이프라인 체인**: `--pipeline "grayscale, blur:2, threshold:128"` 처럼 쉼표로 구분한 필터를 순서대로 적용
- **멀티쓰레드 처리**: `--threads <1~64>`
  - 이미지 행을 쓰레드 수만큼 균등 분할해 `std::thread` 로 병렬 처리
  - 각 쓰레드는 겹치지 않는 행만 쓰고 원본 복사본만 읽으므로 동기화가 필요 없음
  - 쓰레드 안에서 발생한 예외는 `std::exception_ptr` 로 모아 메인 쓰레드에서 다시 던짐
  - 단일 쓰레드 결과와 바이트 단위로 동일함을 확인
- **로그 파일 출력**: `--log <path>`
  - 시작 인자, 이미지 크기, 필터별 소요 시간, 저장 경로, 오류를 타임스탬프와 함께 추가(append) 기록
- **입력 검증과 예외 처리**
  - 알 수 없는 필터, 범위를 벗어난 값, `"2a"` 같은 잘못된 숫자, 파이프라인의 빈 항목을 거부
  - `--filter` 와 `--pipeline` 은 둘 중 정확히 하나만 지정해야 함
  - 필터 명세는 이미지를 읽기 전에 검증하여 빠르게 실패
  - 종료 코드: 성공 0, 인자 오류 4, BMP 오류 2, 필터 오류 3, 기타 1

## 빌드

Visual Studio 2022 (v143, C++17) 에서 `ImageProcessor.sln` 을 열고 `Release | x64` 로 빌드합니다.

```powershell
msbuild ImageProcessor.sln /p:Configuration=Release /p:Platform=x64
```

## 실행 명령어

```powershell
# Grayscale 변환
.\x64\Release\ImageProcessor.exe --input .\Resource\1_astronaut.bmp --output .\Resource\1_astronaut_grayscale.bmp --filter grayscale
```

```powershell
# 임계값 이진화
.\x64\Release\ImageProcessor.exe --input .\Resource\4_text_page.bmp --output .\Resource\4_text_page_threshold.bmp --filter threshold:128
```

```powershell
# Blur (반경 3)
.\x64\Release\ImageProcessor.exe --input .\Resource\2_coffee.bmp --output .\Resource\2_coffee_blur.bmp --filter blur:3
```

```powershell
# 필터 파이프라인 체인 + 멀티쓰레드 + 로그 파일
.\x64\Release\ImageProcessor.exe --input .\Resource\3_chelsea_cat.bmp --output .\Resource\3_chelsea_cat_pipeline.bmp --pipeline "grayscale, blur:2, threshold:128" --threads 8 --log .\run.log
```

```powershell
# 도움말
.\x64\Release\ImageProcessor.exe --help
```

## 파일 구성

| 파일 | 설명 |
| --- | --- |
| `FilterBase.h/.cpp` | 필터 추상 클래스, 행 분할 멀티쓰레드 실행 |
| `GrayscaleFilter.h/.cpp` | 흑백 변환 |
| `ThresholdFilter.h/.cpp` | 임계값 이진화 |
| `BlurFilter.h/.cpp` | 박스 블러 |
| `PixelUtils.h` | 밝기 계산 공용 함수 |
| `FilterFactory.h/.cpp` | 필터/파이프라인 명세 해석 및 생성 |
| `Logger.h/.cpp` | 파일 로거 |
| `CommandLineParser.h/.cpp` | `--pipeline`, `--threads`, `--log` 옵션 추가 |
| `main.cpp` | 필터 생성 → 로드 → 적용 → 저장 흐름 |
| `BmpParser`, `ImageBuffer`, `Exceptions.h` | 제공된 코드 (변경 없음) |
