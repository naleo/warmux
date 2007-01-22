/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************
 * Keyboard managment.
 *****************************************************************************/

#include "keyboard.h"
#include <sstream>
#include <iostream>
#include "cursor.h"
#include "game_msg.h"
#include "interface.h"
#include "../include/action.h"
#include "../include/app.h"
#include "../game/config.h"
#include "../game/game.h"
#include "../game/game_loop.h"
#include "../game/game_mode.h"
#include "../game/time.h"
#include "../graphic/video.h"
#include "../include/action_handler.h"
#include "../include/constant.h"
#include "../map/camera.h"
#include "../team/macro.h"
#include "../character/move.h"
#include "../tool/i18n.h"
#include "../tool/math_tools.h"
#include "../sound/jukebox.h"
#include "../map/camera.h"
#include "../weapon/weapon.h"
#include "../weapon/weapons_list.h"
#include "../network/network.h"

// Vitesse du definalement au clavier
#define SCROLL_CLAVIER 20 // ms

Keyboard * Keyboard::singleton = NULL;

Keyboard * Keyboard::GetInstance() {
  if (singleton == NULL) {
    singleton = new Keyboard();
  }
  return singleton;
}

Keyboard::Keyboard()
{
  //Disable repeated events when a key is kept down
  SDL_EnableKeyRepeat(0,0);

  SetKeyAction(SDLK_LEFT,      Keyboard::KEY_MOVE_LEFT);
  SetKeyAction(SDLK_RIGHT,     Keyboard::KEY_MOVE_RIGHT);
  SetKeyAction(SDLK_UP,        Keyboard::KEY_UP);
  SetKeyAction(SDLK_DOWN,      Keyboard::KEY_DOWN);
  SetKeyAction(SDLK_RETURN,    Keyboard::KEY_JUMP);
  SetKeyAction(SDLK_BACKSPACE, Keyboard::KEY_HIGH_JUMP);
  SetKeyAction(SDLK_b,         Keyboard::KEY_BACK_JUMP);
  SetKeyAction(SDLK_SPACE,     Keyboard::KEY_SHOOT);
  SetKeyAction(SDLK_TAB,       Keyboard::KEY_NEXT_CHARACTER);
  SetKeyAction(SDLK_ESCAPE,    Keyboard::KEY_QUIT);
  SetKeyAction(SDLK_p,         Keyboard::KEY_PAUSE);
  SetKeyAction(SDLK_F10,       Keyboard::KEY_FULLSCREEN);
  SetKeyAction(SDLK_F9,        Keyboard::KEY_TOGGLE_INTERFACE);
  SetKeyAction(SDLK_F1,        Keyboard::KEY_WEAPONS1);
  SetKeyAction(SDLK_F2,        Keyboard::KEY_WEAPONS2);
  SetKeyAction(SDLK_F3,        Keyboard::KEY_WEAPONS3);
  SetKeyAction(SDLK_F4,        Keyboard::KEY_WEAPONS4);
  SetKeyAction(SDLK_F5,        Keyboard::KEY_WEAPONS5);
  SetKeyAction(SDLK_F6,        Keyboard::KEY_WEAPONS6);
  SetKeyAction(SDLK_F7,        Keyboard::KEY_WEAPONS7);
  SetKeyAction(SDLK_F8,        Keyboard::KEY_WEAPONS8);
  SetKeyAction(SDLK_c,         Keyboard::KEY_CENTER);
  SetKeyAction(SDLK_1,         Keyboard::KEY_WEAPON_1);
  SetKeyAction(SDLK_2,         Keyboard::KEY_WEAPON_2);
  SetKeyAction(SDLK_3,         Keyboard::KEY_WEAPON_3);
  SetKeyAction(SDLK_4,         Keyboard::KEY_WEAPON_4);
  SetKeyAction(SDLK_5,         Keyboard::KEY_WEAPON_5);
  SetKeyAction(SDLK_6,         Keyboard::KEY_WEAPON_6);
  SetKeyAction(SDLK_7,         Keyboard::KEY_WEAPON_7);
  SetKeyAction(SDLK_8,         Keyboard::KEY_WEAPON_8);
  SetKeyAction(SDLK_9,         Keyboard::KEY_WEAPON_9);
  SetKeyAction(SDLK_PAGEUP,    Keyboard::KEY_WEAPON_MORE);
  SetKeyAction(SDLK_PAGEDOWN,  Keyboard::KEY_WEAPON_LESS);
  SetKeyAction(SDLK_s,         Keyboard::KEY_CHAT);
}

void Keyboard::Reset()
{
  for (int i = 0; i != 256; i++)
    PressedKeys[i] = false ;
}

void Keyboard::SetKeyAction(int key, Key_t at)
{
  layout[key] = at;
}

// Get the key associated to an action.
int Keyboard::GetKeyAssociatedToAction(Key_t at)
{
  std::map<int, Key_t>::iterator it;
  for (it= layout.begin(); it != layout.end(); it++) {
    if (it->second == at) {
      return it->first;
    }
  }
  return 0;
}


void Keyboard::HandleKeyEvent(const SDL_Event& event)
{
  // Not a keyboard event
  if ( event.type != SDL_KEYDOWN && event.type != SDL_KEYUP) {
    return;
  }

  //Handle input text for Chat session in Network game
  //While player writes, it cannot control the game.
  if(GameLoop::GetInstance()->chatsession.CheckInput()){
    GameLoop::GetInstance()->chatsession.HandleKey(event);
    return;
  }

  Key_Event_t event_type;
  switch( event.type)
    {
    case SDL_KEYDOWN:
      event_type = KEY_PRESSED;
      break;
    case SDL_KEYUP:
      event_type = KEY_RELEASED;
      break;
    default:
      return;
    }

  std::map<int, Key_t>::iterator it = layout.find(event.key.keysym.sym);

  if ( it == layout.end() )
    return;

  Key_t key = it->second;

  if(event_type == KEY_PRESSED) {
    HandleKeyPressed(key);
    return;
  }
  
  if(event_type == KEY_RELEASED) {
    HandleKeyReleased(key);
    return;
  }
}

// Handle a pressed key
void Keyboard::HandleKeyPressed (const Key_t &key)
{
  // Managing keys related to character moves
  // Available only when local
  if (!ActiveTeam().IsLocal()) return;
  if (GameLoop::GetInstance()->ReadState() == GameLoop::END_TURN) return;

  switch (key) {

  case KEY_MOVE_RIGHT:
    ActiveCharacter().HandleKeyPressed_MoveRight();
    goto pressed_accepted;
  case KEY_MOVE_LEFT:
    ActiveCharacter().HandleKeyPressed_MoveLeft();
    goto pressed_accepted;	
  case KEY_UP:
    ActiveCharacter().HandleKeyPressed_Up();
    goto pressed_accepted;
  case KEY_DOWN:
    ActiveCharacter().HandleKeyPressed_Down();
    goto pressed_accepted;
  case KEY_JUMP:
    ActiveCharacter().HandleKeyPressed_Jump();
    goto pressed_accepted;
  case KEY_HIGH_JUMP:
    ActiveCharacter().HandleKeyPressed_HighJump();
    goto pressed_accepted;
  case KEY_BACK_JUMP:
    ActiveCharacter().HandleKeyPressed_BackJump();
    goto pressed_accepted;
  default:
    break;
  }

  if (key == KEY_SHOOT) {
    if (GameLoop::GetInstance()->ReadState() == GameLoop::PLAYING)
      ActiveCharacter().HandleKeyPressed_Shoot();
    goto pressed_accepted;
  }

  return;

 pressed_accepted:
  PressedKeys[key] = true ;
}

// Handle a released key
void Keyboard::HandleKeyReleased (const Key_t &key)
{
  PressedKeys[key] = false ;

  // We manage here only actions which are active on KEY_RELEASED event.
  Interface * interface = Interface::GetInstance();
  
  { // Managing keys related to interface (no game interaction)
    // Always available
    switch(key){
      // Managing interface
    case KEY_QUIT:
      Game::GetInstance()->SetEndOfGameStatus( true );
      return;
    case KEY_PAUSE:
      ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_PAUSE));
      return;
    case KEY_FULLSCREEN:
      AppWormux::GetInstance()->video.ToggleFullscreen();
      return;
    case KEY_CHAT:
      if(network.IsConnected())
	GameLoop::GetInstance()->chatsession.ShowInput();
      return;
    case KEY_CENTER:
      CharacterCursor::GetInstance()->FollowActiveCharacter();
      camera.FollowObject (&ActiveCharacter(), true, true, true);
      return;
    case KEY_TOGGLE_INTERFACE:
      interface->EnableDisplay (!interface->IsDisplayed());
      return;
    default:
      break;
    }
  }

  // Managing shoot key
  // Drop bonus box or medkit when outside a turn
  // Shoot when in turn
  if (key == KEY_SHOOT) {
    
    if (GameLoop::GetInstance()->ReadState() == GameLoop::END_TURN) {
      ObjBox* current_box = GameLoop::GetInstance()->GetCurrentBox();
      if (current_box != NULL) {
	current_box->DropBox();
      }
    } else if (GameLoop::GetInstance()->ReadState() == GameLoop::PLAYING &&
	       ActiveTeam().IsLocal()) {
      ActiveCharacter().HandleKeyReleased_Shoot();
    }
    return;
  }

  { // Managing keys related to character moves
    // Available only when local
    if (!ActiveTeam().IsLocal()) return;

    if (GameLoop::GetInstance()->ReadState() != GameLoop::PLAYING &&
	GameLoop::GetInstance()->ReadState() != GameLoop::HAS_PLAYED)
      return;

    switch (key) {
    case KEY_MOVE_RIGHT:
      ActiveCharacter().HandleKeyReleased_MoveRight();
      return;
    case KEY_MOVE_LEFT:
      ActiveCharacter().HandleKeyReleased_MoveLeft();
      return;
    case KEY_UP:
      ActiveCharacter().HandleKeyReleased_Up();
      return;
    case KEY_DOWN:
      ActiveCharacter().HandleKeyReleased_Down();
      return;
    case KEY_JUMP:
      ActiveCharacter().HandleKeyReleased_Jump();
      return;
    case KEY_HIGH_JUMP:
      ActiveCharacter().HandleKeyReleased_HighJump();
      return;
    case KEY_BACK_JUMP:
      ActiveCharacter().HandleKeyReleased_BackJump();
      return;
    default:
      break;
	
    }
  }

  { // Managing keys related to change of character or weapon

    if (GameLoop::GetInstance()->ReadState() != GameLoop::PLAYING ||
	!ActiveTeam().GetWeapon().CanChangeWeapon())
      return;
      
    int weapon_sort = -1;
    
    switch(key) {

    case KEY_NEXT_CHARACTER:
      if (GameMode::GetInstance()->AllowCharacterSelection()) {
	Action * next_character = new Action(Action::ACTION_NEXT_CHARACTER);
	next_character->StoreActiveCharacter();
	ActiveTeam().NextCharacter();
	next_character->StoreActiveCharacter();
	ActionHandler::GetInstance()->NewAction(next_character);
      }
      return;

    case KEY_WEAPONS1:
      weapon_sort = 1;
      break;
    case KEY_WEAPONS2:
      weapon_sort = 2;
      break;
    case KEY_WEAPONS3:
      weapon_sort = 3;
      break;
    case KEY_WEAPONS4:
      weapon_sort = 4;
      break;
    case KEY_WEAPONS5:
      weapon_sort = 5;
      break;
    case KEY_WEAPONS6:
      weapon_sort = 6;
      break;
    case KEY_WEAPONS7:
      weapon_sort = 7;
      break;
    case KEY_WEAPONS8:
      weapon_sort = 8;
      break;
    default:
      break ;
    }
    
    if ( weapon_sort != -1 ) {
      Weapon::Weapon_type weapon;
      if (WeaponsList::GetInstance()->GetWeaponBySort(weapon_sort, weapon))
	ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_CHANGE_WEAPON, weapon));
      
      return;
    }
  }
}

// Refresh keys which are still pressed.
void Keyboard::Refresh()
{
  //Treat KEY_REFRESH events:
  for (int i = 0; i < 256; i++)
    if(PressedKeys[i]) {
      Key_t key = static_cast<Key_t>(i);
      
      switch(key){
      case KEY_MOVE_RIGHT:
	ActiveCharacter().HandleKeyRefreshed_MoveRight();
	break;
      case KEY_MOVE_LEFT:
	ActiveCharacter().HandleKeyRefreshed_MoveLeft();
	break;
      case KEY_UP:
	ActiveCharacter().HandleKeyRefreshed_Up();
	break;
      case KEY_DOWN:
	ActiveCharacter().HandleKeyRefreshed_Down();
	break;  
      case KEY_JUMP:
	ActiveCharacter().HandleKeyRefreshed_Jump();
	break;
      case KEY_HIGH_JUMP:
	ActiveCharacter().HandleKeyRefreshed_HighJump();
	break;
      case KEY_BACK_JUMP:
	ActiveCharacter().HandleKeyRefreshed_BackJump();
	break;
      case KEY_SHOOT:
	ActiveCharacter().HandleKeyRefreshed_Shoot();
	break;
      default:
	break;
      }
    }
}
