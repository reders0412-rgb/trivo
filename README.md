# 🧊 Trivo — 3D File Viewer

> **오픈소스 3D 파일 뷰어** | Open-source 3D File Viewer  
> GLB · FBX · OBJ · GLTF · STL · DAE · 3DS · 30+ formats

[![Build](https://github.com/your-org/trivo/actions/workflows/build.yml/badge.svg)](https://github.com/your-org/trivo/actions/workflows/build.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey)](#)

---

## ✨ 주요 기능 / Features

| 기능 | Feature |
|------|---------|
| 30+ 3D 파일 형식 지원 | 30+ 3D file format support |
| PBR 렌더링 (OpenGL 4.1) | PBR Rendering (OpenGL 4.1) |
| 드래그 앤 드롭 | Drag & Drop |
| 여러 모델을 하나의 씬에 배치 | Multi-model scene |
| 애니메이션 재생 (속도 조절) | Animation playback with speed control |
| 5가지 라이트 프리셋 | 5 Light presets |
| 다크 / 라이트 테마 | Dark & Light theme |
| 한국어 / 영어 UI | Korean & English UI |
| 스크린샷 저장 | Screenshot export |
| 파일별 전용 아이콘 | Per-format file icons |
| 그리드 배경 | Grid background |

---

## 📦 지원 형식 / Supported Formats

| 카테고리 | 형식 |
|---------|-----|
| 모던 / Modern | GLB, GLTF |
| 게임 엔진 / Game | FBX, X (DirectX) |
| 범용 / General | OBJ, DAE (COLLADA), 3DS, PLY, STL |
| DCC | BLEND (Blender), LWO (LightWave), MS3D |
| CAD | STEP, IGES, IFC, DXF |
| 애니메이션 | BVH, MD2, MD3, MD5 |
| 포인트 클라우드 | PCD, XYZ, PTS |
| 기타 | AC, OFF, NFF, SMD, VTA, COB, ... |

---

## 🛠 빌드 방법 / Build Instructions

### 요구사항 / Requirements

- C++20 컴파일러 / C++20 compiler (MSVC 2019+, GCC 10+, Clang 12+)
- CMake 3.20+
- Qt 6.4+
- Assimp 5.x

### Linux / macOS

```bash
# 의존성 설치 / Install dependencies
# Ubuntu
sudo apt-get install libassimp-dev qt6-base-dev qt6-opengl-dev

# macOS
brew install assimp qt6

# 빌드 / Build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/Trivo
```

### Windows

```powershell
# vcpkg로 Assimp 설치
vcpkg install assimp:x64-windows

# Qt6 설치 후 (Qt 공식 인스톨러)
cmake -B build -S . `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build --config Release
```

### GitHub Actions (자동 빌드)

`main` 브랜치에 push 하거나 `v*` 태그를 달면 자동으로 Windows / macOS / Linux 빌드가 실행됩니다.

```bash
git tag v1.0.0
git push origin v1.0.0
# → GitHub Release에 자동 업로드
```

---

## 🎮 조작법 / Controls

| 조작 | 기능 |
|------|------|
| 왼쪽 드래그 | 카메라 회전 |
| 가운데 드래그 | 카메라 이동 (Pan) |
| 스크롤 | 줌 인/아웃 |
| `R` | 카메라 초기화 |
| `F` | 씬에 맞춤 (Fit) |
| `Space` | 애니메이션 재생/정지 |
| `Ctrl+O` | 파일 열기 |
| `Ctrl+Shift+O` | 씬에 추가 |
| `Ctrl+P` | 스크린샷 |

---

## 📚 라이브러리 / Libraries

| 라이브러리 | 라이선스 |
|-----------|---------|
| [Qt6](https://www.qt.io) | LGPL v3 |
| [Assimp](https://github.com/assimp/assimp) | BSD 3-Clause |
| OpenGL 4.1 | Vendor |
| CMake | BSD 3-Clause |

---

## 📄 라이선스 / License

MIT License — Copyright (c) 2024 Trivo Contributors
