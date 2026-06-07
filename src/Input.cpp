#include "Input.h"

Input Input::instance;

void Input::initDefault() {
   keyBindings = std::unordered_map<Key, SDL_Scancode>{
       {Key::NONE, SDL_SCANCODE_UNKNOWN},
       {Key::RIGHT, SDL_SCANCODE_D},
       {Key::LEFT, SDL_SCANCODE_A},
       {Key::JUMP, SDL_SCANCODE_SPACE},
       {Key::DUCK, SDL_SCANCODE_S},
       {Key::SPRINT, SDL_SCANCODE_LSHIFT},
       {Key::FIREBALL, SDL_SCANCODE_Q},
       {Key::MENU_UP, SDL_SCANCODE_UP},
       {Key::MENU_DOWN, SDL_SCANCODE_DOWN},
       {Key::MENU_LEFT, SDL_SCANCODE_LEFT},
       {Key::MENU_RIGHT, SDL_SCANCODE_RIGHT},
       {Key::MENU_ACCEPT, SDL_SCANCODE_RETURN},
       {Key::MENU_ESCAPE, SDL_SCANCODE_ESCAPE},
       {Key::PAUSE, SDL_SCANCODE_ESCAPE},
   };

   for (auto key : keyBindings) {
      keysPressed.insert({key.first, false});
      keysHeld.insert({key.first, false});
#ifdef ANDROID
      virtualKeys.insert({key.first, false});
#endif
   }
}

void Input::set(Key action, SDL_Scancode keyCode) {
   keyBindings.insert_or_assign(action, keyCode);
   keysPressed.insert_or_assign(action, false);
   keysHeld.insert_or_assign(action, false);
#ifdef ANDROID
   virtualKeys.insert_or_assign(action, false);
#endif
}

void Input::update(const Uint8* keystates) {
   currentRawKeys.clear();

   for (auto key : keyBindings) {
#ifdef ANDROID
      // Combine keyboard state with virtual (touch) state
      bool physicalDown = keystates[key.second];
      bool virtualDown  = virtualKeys.count(key.first) ? virtualKeys.at(key.first) : false;
      bool isDown = physicalDown || virtualDown;

      // One-shot virtual press (menu tap)
      if (virtualPressedThisFrame.count(key.first)) {
         isDown = true;
      }
#else
      bool isDown = keystates[key.second];
#endif

      if (isDown) {
         keysHeld.insert_or_assign(key.first, keysPressed.at(key.first));
         keysPressed.insert_or_assign(key.first, true);
      } else {
         keysPressed.insert_or_assign(key.first, false);
         keysHeld.insert_or_assign(key.first, false);
      }
   }

#ifdef ANDROID
   // Clear one-shot presses after processing
   virtualPressedThisFrame.clear();
#endif
}

#ifdef ANDROID
void Input::setVirtualKey(Key key, bool pressed) {
   virtualKeys.insert_or_assign(key, pressed);
}

void Input::pressVirtualKey(Key key) {
   virtualPressedThisFrame.insert(key);
}
#endif

SDL_Scancode Input::getBoundKey(Key action) {
   return keyBindings.at(action);
}

bool Input::getRawKey(Key action) {
   return keysPressed.at(action);
}

bool Input::getKeyPressed(Key action) {
   return keysPressed.at(action) && !keysHeld.at(action);
}

bool Input::getKeyHeld(Key action) {
   return keysHeld.at(action);
}

std::vector<SDL_Scancode>& Input::getCurrentRawKeys() {
   return currentRawKeys;
}
