#ifdef ANDROID

#include <jni.h>
#include "Input.h"

// Maps integer button IDs from Java to Key enum values
static Key keyFromId(int id) {
    switch (id) {
        case 0: return Key::LEFT;
        case 1: return Key::RIGHT;
        case 2: return Key::DUCK;
        case 3: return Key::JUMP;
        case 4: return Key::SPRINT;
        case 5: return Key::FIREBALL;
        case 6: return Key::PAUSE;
        case 7: return Key::MENU_ACCEPT;
        case 8: return Key::MENU_ESCAPE;
        default: return Key::NONE;
    }
}

extern "C" {

// Called when a virtual button is held down or released
JNIEXPORT void JNICALL
Java_com_supermariobros_game_TouchOverlay_nativeSetKey(JNIEnv* env, jclass cls,
                                                       jint keyId, jboolean pressed) {
    Key key = keyFromId(keyId);
    if (key != Key::NONE) {
        Input::Get().setVirtualKey(key, pressed == JNI_TRUE);
    }
}

// Called for a tap (one-shot press, for menu buttons)
JNIEXPORT void JNICALL
Java_com_supermariobros_game_TouchOverlay_nativePressKey(JNIEnv* env, jclass cls,
                                                          jint keyId) {
    Key key = keyFromId(keyId);
    if (key != Key::NONE) {
        Input::Get().pressVirtualKey(key);
    }
}

} // extern "C"

#endif // ANDROID
