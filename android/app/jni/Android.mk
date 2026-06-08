MY_ROOT := $(call my-dir)

###########################
# SDL2
###########################
LOCAL_PATH := $(MY_ROOT)
include $(LOCAL_PATH)/SDL/Android.mk

###########################
# SDL2_image
###########################
LOCAL_PATH := $(MY_ROOT)
SUPPORT_PNG        := true
SUPPORT_JPG        := true
SUPPORT_WEBP       := false
SUPPORT_PNG_SHARED := false
SUPPORT_JPG_SHARED := false
include $(LOCAL_PATH)/SDL_image/Android.mk

###########################
# SDL2_mixer
###########################
LOCAL_PATH := $(MY_ROOT)
SUPPORT_WAV          := true
SUPPORT_OGG          := false
SUPPORT_MP3_MPG123   := false
SUPPORT_FLAC         := false
SUPPORT_MOD_MODPLUG  := false
SUPPORT_MID_TIMIDITY := false
include $(LOCAL_PATH)/SDL_mixer/Android.mk

###########################
# SDL2_ttf
###########################
LOCAL_PATH := $(MY_ROOT)
include $(LOCAL_PATH)/SDL_ttf/Android.mk

###########################
# Super Mario Bros (game)
###########################
LOCAL_PATH := $(MY_ROOT)
include $(CLEAR_VARS)

LOCAL_MODULE := main

# $(LOCAL_PATH) добавлен первым — это папка jni/.
# В CI создаётся симлинк jni/SDL2 -> SDL/include,
# поэтому #include <SDL2/SDL.h> резолвится как jni/SDL2/SDL.h ✓
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH) \
    $(LOCAL_PATH)/SDL/include \
    $(LOCAL_PATH)/SDL_image \
    $(LOCAL_PATH)/SDL_mixer \
    $(LOCAL_PATH)/SDL_mixer/include \
    $(LOCAL_PATH)/SDL_ttf \
    $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/include/command \
    $(LOCAL_PATH)/include/scenes \
    $(LOCAL_PATH)/include/systems \
    $(LOCAL_PATH)/include/util \
    $(LOCAL_PATH)/include/ECS

LOCAL_SRC_FILES := \
    src/AABBCollision.cpp \
    src/Camera.cpp \
    src/Core.cpp \
    src/Game.cpp \
    src/Input.cpp \
    src/AndroidJNI.cpp \
    src/Map.cpp \
    src/SoundManager.cpp \
    src/TextureManager.cpp \
    src/command/CommandScheduler.cpp \
    src/main.cpp \
    src/scenes/GameOverScene.cpp \
    src/scenes/GameScene.cpp \
    src/scenes/MenuScene.cpp \
    src/systems/AnimationSystem.cpp \
    src/systems/CallbackSystem.cpp \
    src/systems/CollectibleSystem.cpp \
    src/systems/EnemySystem.cpp \
    src/systems/FlagSystem.cpp \
    src/systems/MapSystem.cpp \
    src/systems/MenuSystem.cpp \
    src/systems/OptionsSystem.cpp \
    src/systems/PhysicsSystem.cpp \
    src/systems/PlayerSystem.cpp \
    src/systems/RenderSystem.cpp \
    src/systems/ScoreSystem.cpp \
    src/systems/SoundSystem.cpp \
    src/systems/WarpSystem.cpp

LOCAL_CPPFLAGS := -std=c++17 -DANDROID -DSDL_MAIN_HANDLED

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_mixer SDL2_ttf

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid -lz

include $(BUILD_SHARED_LIBRARY)
