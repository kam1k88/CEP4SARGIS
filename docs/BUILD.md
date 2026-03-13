# SargisLab — Build Instructions

Кроссплатформенное научное приложение для регрессии и аппроксимации кривых.

## Требования

- **CMake** 3.16+
- **C++20** компилятор (MSVC 2019+, GCC 10+, Clang 12+)
- **Qt 6** (Core, Gui, Widgets, Charts)
- **Eigen 3.4+**
- **Ceres Solver 2.2+**

## Windows 11 (MSVC)

### Установка зависимостей (vcpkg)

```powershell
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg install eigen3:x64-windows ceres[suitesparse]:x64-windows qt6-base:x64-windows qt6-charts:x64-windows
.\vcpkg integrate install
```

### Сборка

```powershell
cd SargisLab
mkdir build
cd build

cmake -G "Visual Studio 17 2022" -A x64 `
  -DCMAKE_TOOLCHAIN_FILE="C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake" `
  -DCMAKE_PREFIX_PATH="C:/Qt/6.6/msvc2019_64" `
  ..

cmake --build . --config Release
```

Исполняемый файл: `build\Release\SargisLab.exe`

### Без vcpkg (ручная установка Qt и Ceres)

```powershell
cmake -G "Visual Studio 17 2022" -A x64 `
  -DCMAKE_PREFIX_PATH="C:/Qt/6.6/msvc2019_64" `
  -DCeres_DIR="C:/path/to/ceres/build" `
  -DEigen3_DIR="C:/path/to/eigen/share/eigen3/cmake" `
  ..
```

## macOS ARM64 (M1–M3)

### Установка зависимостей (Homebrew)

```bash
brew install qt@6 eigen ceres-solver
```

### Сборка

```bash
cd SargisLab
mkdir build
cd build

cmake -G Ninja \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_PREFIX_PATH="$(brew --prefix qt@6)" \
  -DCMAKE_BUILD_TYPE=Release \
  ..

ninja
```

Или с vcpkg:

```bash
cmake -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_TARGET_TRIPLET=arm64-osx \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  ..
```

Бандл приложения: `build/SargisLabScientificApp.app`

### Сборка и упаковка (скрипты)

```bash
# Полная сборка и установка
./build_mac.sh

# Создание DMG (после build_mac.sh)
./package_mac.sh install/SargisLabScientificApp.app
```

После `build_mac.sh`:
- `.app`: `install/SargisLabScientificApp.app`
- DMG: `dist/SargisLabScientificApp.dmg` (после `package_mac.sh`)

### Install target

```bash
cmake --build build --target install
# Результат: ${CMAKE_INSTALL_PREFIX}/SargisLabScientificApp.app
```

### Иконка приложения

Поместите `app.icns` в `resources/icons/` для включения в бандл. Инструкции — в `resources/icons/README.md`.

## GitHub Actions — Unified Release

При пуше тега `v*` workflow `release.yml` автоматически:
1. Собирает Windows x64 (vcpkg + MSVC)
2. Собирает macOS ARM64 (подпись и notarization)
3. Создаёт GitHub Release с артефактами:
   - `SargisLabScientificApp-win64.zip`
   - `SargisLabScientificApp.dmg`
   - `SargisLabScientificApp.app.zip`

```bash
git tag v1.0.0
git push origin v1.0.0
```

### Windows Release Workflow

`.github/workflows/windows_release.yml` (также входит в `release.yml`):
- **Runner**: `windows-latest`
- **Зависимости**: vcpkg (ceres, eigen3, qt6-base, qt6-charts, qt6-tools)
- **Сборка**: CMake + Visual Studio 2022, Release
- **Упаковка**: windeployqt, папка `SargisLabScientificApp-win64/`, zip

### macOS Release Workflow

`.github/workflows/mac_notarize.yml` (входит в `release.yml`):
- **Runner**: `macos-14` (ARM64)
- Сборка, подпись, notarization
- Артефакты: `.dmg`, `.app.zip`

## Отдельные workflows

- `windows_release.yml` — только Windows (workflow_dispatch)
- `mac_notarize.yml` — только macOS с notarization

## Опции CMake

| Опция | По умолчанию | Описание |
|-------|--------------|----------|
| `SargisLab_USE_QCUSTOMPLOT` | ON | Использовать QCustomPlot для графиков |
| `SargisLab_BUILD_PLUGINS` | ON | Собирать пример плагина |

## QCustomPlot (опционально)

Для научной визуализации с высокой производительностью:

1. Скачайте [QCustomPlot](https://www.qcustomplot.com/) (qcustomplot.h и qcustomplot.cpp)
2. Поместите файлы в `external/qcustomplot/`
3. Пересоберите проект

При отсутствии QCustomPlot используется Qt Charts.

## Excel (.xlsx) поддержка

Опционально. Поместите заголовки OpenXLSX в `external/OpenXLSX` или установите через систему.

## Плагины

Пример плагина `gaussian_cdf_plugin` собирается в `plugins/gaussian_cdf_plugin/`. Скопируйте `.dll` (Windows) или `.dylib` (macOS) в каталог `plugins/` рядом с исполняемым файлом. Приложение автоматически загрузит плагины при запуске.

## Структура проекта

```
SargisLab/
├── core/          # Модели, регрессия, анализ, IO, плагины
├── ui/            # Qt GUI
├── platform/      # OS-утилиты (Windows, macOS)
├── app/           # Точки входа
├── plugins/       # Пример плагина
└── docs/          # Документация
```

## Отладка

- **Windows**: Visual Studio — откройте `build/SargisLab.sln`, выберите конфигурацию Debug.
- **macOS**: Xcode — сгенерируйте проект через CMake с `-G Xcode` или используйте `lldb` с Ninja-build.
