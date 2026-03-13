# SargisLab

Научное приложение для анализа данных и аппроксимации кривых (аналог CurveExpert Professional).

## Возможности

- **Импорт/экспорт**: CSV, JSON, Excel (.xlsx)
- **Регрессия**: полиномы, экспоненциальные, логистические модели, Gaussian CDF
- **Алгоритм**: Levenberg-Marquardt (Ceres Solver)
- **Анализ**: остатки, QQ-plot, симметрия, прогностическая ошибка
- **Монте-Карло**: распределения параметров, 1000+ симуляций
- **Плагины**: добавление моделей без пересборки

## Сборка

См. [docs/BUILD.md](docs/BUILD.md).

## Кратко

```bash
# Windows (vcpkg)
vcpkg install eigen3 ceres:x64-windows qt6-base:x64-windows qt6-charts:x64-windows
cmake -B build -DCMAKE_TOOLCHAIN_FILE=[vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release

# macOS
brew install qt@6 eigen ceres-solver
cmake -B build -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)
cmake --build build
```

## Структура

- `core/` — модели, регрессия, анализ, IO, плагины
- `ui/` — Qt GUI (главное окно, графики, таблица данных)
- `platform/` — OS-специфичные утилиты
- `app/` — точки входа (Windows, macOS)
- `plugins/` — пример плагина Gaussian CDF
