# Упаковка SargisLab для macOS ARM64

## Быстрый старт

```bash
./build_mac.sh
./package_mac.sh install/SargisLabScientificApp.app
```

Результат:
- `install/SargisLabScientificApp.app` — бандл приложения
- `dist/SargisLabScientificApp.dmg` — образ для распространения

## build_mac.sh

Скрипт выполняет:
1. `brew install cmake ninja eigen ceres-solver qt@6`
2. CMake configure с `-DCMAKE_OSX_ARCHITECTURES=arm64`
3. `cmake --build . --parallel`
4. `cmake --install .` (результат в `install/`)

## package_mac.sh

Использование:
```bash
./package_mac.sh <путь-к-.app> [--sign] [--notarize]
```

Опции:
- без флагов — только DMG (без подписи)
- `--sign` — подписать .app перед созданием DMG
- `--notarize` — подписать, создать DMG, отправить на notarization, staple; результат в `dist/notarized/`

Скрипт:
1. Запускает `macdeployqt` для копирования Qt-библиотек в бандл
2. При `--sign`: вызывает `sign_mac.sh`
3. Создаёт DMG через `hdiutil create`
4. При `--notarize`: вызывает `notarize_mac.sh`

## Подпись (sign_mac.sh)

Подписывает .app для распространения вне App Store:

```bash
./sign_mac.sh install/SargisLabScientificApp.app
```

Требуется **Developer ID Application** сертификат. Идентификатор задаётся через:

```bash
export DEVELOPER_ID_CERT="Developer ID Application: Your Name (TEAM_ID)"
./sign_mac.sh install/SargisLabScientificApp.app
```

Или вторым аргументом:
```bash
./sign_mac.sh install/SargisLabScientificApp.app "Developer ID Application: Your Name (TEAM_ID)"
```

Параметры `codesign`:
- `--deep` — подпись вложенных компонентов
- `--options runtime` — hardened runtime (нужно для notarization)
- `--timestamp` — штамп времени от Apple

Проверка подписи:
```bash
codesign --verify --deep --strict install/SargisLabScientificApp.app
```

## Notarization (notarize_mac.sh)

Отправляет DMG в Apple на проверку и прикрепляет штамп:

```bash
./notarize_mac.sh dist/SargisLabScientificApp.dmg
```

Переменные окружения:
- `APPLE_ID` — Apple ID (email)
- `TEAM_ID` — Team ID (10 символов)
- `APP_SPECIFIC_PASSWORD` — пароль приложения с [appleid.apple.com](https://appleid.apple.com)

Пример:
```bash
export APPLE_ID="developer@example.com"
export TEAM_ID="ABCD123456"
export APP_SPECIFIC_PASSWORD="xxxx-xxxx-xxxx-xxxx"
./notarize_mac.sh dist/SargisLabScientificApp.dmg
```

После успешной проверки Apple ticket прикрепляется к DMG (`xcrun stapler staple`).

## Локальная подпись и notarization

Полный цикл:

```bash
# 1. Сборка
./build_mac.sh

# 2. Подпись .app
export DEVELOPER_ID_CERT="Developer ID Application: Your Name (TEAM_ID)"
./sign_mac.sh install/SargisLabScientificApp.app

# 3. Упаковка DMG (с подписью)
./package_mac.sh install/SargisLabScientificApp.app --sign

# 4. Notarization
export APPLE_ID="your@apple.id"
export TEAM_ID="TEAMID1234"
export APP_SPECIFIC_PASSWORD="app-specific-password"
./notarize_mac.sh dist/SargisLabScientificApp.dmg
```

Или одной командой (если заданы переменные окружения):
```bash
./package_mac.sh install/SargisLabScientificApp.app --sign --notarize
```

## Unified GitHub Release

**Основной workflow** `.github/workflows/release.yml`:
- Триггер: push тега `v*` или ручной запуск
- **build-windows**: сборка Windows x64, артефакт `SargisLabScientificApp-win64.zip`
- **build-macos**: сборка macOS ARM64, подпись, notarization; артефакты `.dmg`, `.app.zip`
- **publish-release**: создаёт GitHub Release и загружает все три файла

```bash
git tag v1.0.0
git push origin v1.0.0
```

## GitHub Actions (macOS standalone)

Workflow `.github/workflows/mac_notarize.yml`:
- Триггер: ручной запуск (workflow_dispatch)
- Runner: `macos-14` (ARM64)
- Сборка, подпись, notarization
- Артефакты в `dist/notarized/`

### Секреты репозитория

В **Settings → Secrets and variables → Actions** создайте:

| Секрет | Описание |
|--------|----------|
| `DEVELOPER_ID_CERT` | Base64-кодированный .p12 (Developer ID Application) |
| `CERTIFICATE_PASSWORD` | Пароль от .p12 |
| `CODESIGN_IDENTITY` | Имя сертификата для codesign, напр. `Developer ID Application: Name (TEAM_ID)` |
| `APPLE_ID` | Apple ID (email) |
| `TEAM_ID` | Team ID (10 символов) |
| `APP_SPECIFIC_PASSWORD` | App-specific password |

### Экспорт сертификата в base64

```bash
base64 -i Certificate.p12 | pbcopy
# Вставьте в GitHub Secrets как DEVELOPER_ID_CERT
```

### Создание App-specific password

1. [appleid.apple.com](https://appleid.apple.com) → Sign-In and Security → App-Specific Passwords
2. Создать пароль для «SargisLab Notarization»
3. Сохранить в секрет `APP_SPECIFIC_PASSWORD`

## GitHub Actions (без notarization)

Workflow `.github/workflows/mac_arm64_release.yml` — сборка macOS без подписи и notarization (для тестов).

## package_mac.sh — артефакты

При вызове с `--sign` или `--notarize` создаётся:
- `SargisLabScientificApp.dmg`
- `SargisLabScientificApp.app.zip`

## Иконка

Добавьте `resources/icons/app.icns` для отображения иконки в Finder и Dock. Инструкции — `resources/icons/README.md`.
