#pragma once

#include <SDL2/SDL.h>

#include <unordered_map>
#include <unordered_set>
#include <vector>

enum class Key : int
{
   NONE,
   RIGHT,
   LEFT,
   JUMP,
   DUCK,
   SPRINT,
   FIREBALL,
   MENU_UP,
   MENU_DOWN,
   MENU_LEFT,
   MENU_RIGHT,
   MENU_ACCEPT,
   MENU_ESCAPE,
   PAUSE,
};

class Input {
  public:
   static Input& Get() {
      return instance;
   }

   void initDefault();

   void set(const Key action, SDL_Scancode keyCode);

   // Called each frame with keyboard state
   void update(const Uint8* keystates);

#ifdef ANDROID
   // Called from Java via JNI when a virtual button is pressed/released
   void setVirtualKey(Key key, bool pressed);

   // Called from Java to inject a virtual key press for menu navigation
   void pressVirtualKey(Key key);
#endif

   SDL_Scancode getBoundKey(Key action);

   bool getRawKey(Key action);

   bool getKeyPressed(Key action);

   bool getKeyHeld(Key action);

   std::vector<SDL_Scancode>& getCurrentRawKeys();

  private:
   static Input instance;

   Input() {
      initDefault();
   }

   Input(const Input&) = delete;

   std::vector<SDL_Scancode> currentRawKeys;

   std::unordered_map<Key, SDL_Scancode> keyBindings;
   std::unordered_map<Key, bool> keysPressed;
   std::unordered_map<Key, bool> keysHeld;

#ifdef ANDROID
   // Virtual keys set by touch overlay (independent of keyboard)
   std::unordered_map<Key, bool> virtualKeys;
   // Keys that were virtually pressed this frame (one-shot)
   std::unordered_set<Key> virtualPressedThisFrame;
#endif
};
