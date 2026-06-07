LOCAL_PATH := $(call my-dir)

###########################
# SDL2
###########################
include $(LOCAL_PATH)/SDL/Android.mk

###########################
# SDL2_image
###########################
SUPPORT_PNG := true
SUPPORT_JPG := true
include $(LOCAL_PATH)/SDL_image/Android.mk

###########################
# SDL2_mixer
###########################
SUPPORT_WAV := true
SUPPORT_OGG := false
SUPPORT_MP3_MPG123 := false
include $(LOCAL_PATH)/SDL_mixer/Android.mk

###########################
# SDL2_ttf
###########################
include $(LOCAL_PATH)/SDL_ttf/Android.mk

###########################
# Super Mario Bros (game)
###########################
include $(CLEAR_VARS)

LOCAL_MODULE := main

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/SDL/include \
    $(LOCAL_PATH)/SDL_image \
    $(LOCAL_PATH)/SDL_mixer \
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

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid

include $(BUILD_SHARED_LIBRARY)
