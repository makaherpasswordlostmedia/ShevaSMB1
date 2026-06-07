# ShevaMario — Android Port

Порт проекта [Gold872/Super-Mario-Bros](https://github.com/Gold872/Super-Mario-Bros) на Android через SDL2 + Android NDK.

## Как использовать

### 1. Скопируй файлы из этого репозитория в корень проекта

Структура после копирования:
```
Super-Mario-Bros/
├── .github/
│   └── workflows/
│       └── build-android.yml   ← GitHub Actions
├── android/
│   ├── gradlew                 ← нужно сгенерировать (см. ниже)
│   ├── gradlew.bat
│   ├── build.gradle
│   ├── settings.gradle
│   ├── gradle.properties
│   └── app/
│       ├── build.gradle
│       ├── jni/
│       │   ├── Android.mk      ← NDK build script
│       │   ├── Application.mk
│       │   ├── SDL/            ← скачивается автоматически в CI
│       │   ├── SDL_image/
│       │   ├── SDL_mixer/
│       │   ├── SDL_ttf/
│       │   ├── src/            ← копия src/ из проекта
│       │   └── include/        ← копия include/ из проекта
│       └── src/main/
│           ├── AndroidManifest.xml
│           ├── assets/res/     ← копия bin/res/ из проекта
│           └── java/com/supermariobros/game/
│               └── MainActivity.java
├── src/          ← оригинальные исходники
├── include/
└── bin/res/
```

### 2. Сгенерируй gradlew

В папке `android/` выполни:
```bash
gradle wrapper --gradle-version 8.4
```
Или скачай вручную: https://gradle.org/releases/

### 3. Push на GitHub

GitHub Actions автоматически запустит сборку при push.

APK будет доступен во вкладке **Actions → Artifacts** → `SuperMarioBros-debug`.

## Управление на Android

Игра использует SDL2, который автоматически поддерживает:
- **Геймпад** (Bluetooth/USB) — рекомендуется
- **Экранный джойстик** — SDL2 не добавляет автоматически, нужна доработка

## Требования

- Android 5.0+ (API 21)
- ARM64, ARMv7 или x86_64
- ~100MB свободного места

## Известные ограничения

- Сенсорного управления нет из коробки (только геймпад)
- Разрешение фиксировано 800×480 (как в оригинале)
