#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define SDL_MAIN_HANDLED
#include "Core.h"

#ifdef ANDROID
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <SDL2/SDL.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <cstdio>
#include <string>

// Рекурсивно копирует папку из assets/ во внутреннее хранилище.
// SDL_AndroidGetInternalStoragePath() → /data/data/<pkg>/files
static void copyAssetDir(AAssetManager* mgr, const std::string& assetPath,
                         const std::string& destPath) {
    // Создаём целевую директорию
    mkdir(destPath.c_str(), 0755);

    AAssetDir* dir = AAssetManager_openDir(mgr, assetPath.c_str());
    if (!dir) return;

    const char* filename;
    while ((filename = AAssetDir_getNextFileName(dir)) != nullptr) {
        std::string srcFile  = assetPath + "/" + filename;
        std::string destFile = destPath  + "/" + filename;

        // Пробуем открыть как файл
        AAsset* asset = AAssetManager_open(mgr, srcFile.c_str(), AASSET_MODE_STREAMING);
        if (asset) {
            FILE* out = fopen(destFile.c_str(), "wb");
            if (out) {
                char buf[4096];
                int  read;
                while ((read = AAsset_read(asset, buf, sizeof(buf))) > 0) {
                    fwrite(buf, 1, read, out);
                }
                fclose(out);
            }
            AAsset_close(asset);
        } else {
            // Это подпапка — рекурсируем
            copyAssetDir(mgr, srcFile, destFile);
        }
    }
    AAssetDir_close(dir);
}

static void setupAndroidWorkingDir() {
    // Внутреннее хранилище: /data/data/<pkg>/files
    const char* internalPath = SDL_AndroidGetInternalStoragePath();
    if (!internalPath) return;

    std::string destRes = std::string(internalPath) + "/res";

    // Копируем только если res/ ещё не существует (не делаем это при каждом запуске)
    struct stat st;
    if (stat(destRes.c_str(), &st) != 0) {
        JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
        jobject activity = (jobject)SDL_AndroidGetActivity();

        // Получаем AssetManager через Java
        jclass    activityClass = env->GetObjectClass(activity);
        jmethodID getAssets     = env->GetMethodID(activityClass,
                                      "getAssets", "()Landroid/content/res/AssetManager;");
        jobject   assetManager  = env->CallObjectMethod(activity, getAssets);

        AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
        if (mgr) {
            copyAssetDir(mgr, "res", destRes);
        }

        env->DeleteLocalRef(assetManager);
        env->DeleteLocalRef(activityClass);
        env->DeleteLocalRef(activity);
    }

    // Меняем рабочую директорию — теперь все относительные пути ("res/...") работают
    chdir(internalPath);
}
#endif // ANDROID

void runLoop(void* data) {
    Core* core = (Core*)data;
    core->mainLoop();
}

int main(int argc, char** argv) {
#ifdef ANDROID
    // Шаг 1: скопировать ресурсы из assets/ во внутреннее хранилище и chdir туда.
    // После этого весь код с fopen("res/...") работает без изменений.
    setupAndroidWorkingDir();
#endif

    Core core;

    if (core.init() != 0) {
        return -1;
    }

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(runLoop, &core, 0, 1);
#else
    core.run();
#endif

    return 0;
}
