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
 * Artificial intelligence stupid engine
 *****************************************************************************/
#ifndef AI_STUPID_ENGINE
#define AI_STUPID_ENGINE

#include "ai_engine.h"
#include "../character/character.h"
#include "../game/time.h"

class AIStupidEngine
{
 private:
  static AIStupidEngine * singleton;
  AIStupidEngine();

  Character* m_last_char;
  uint m_begin_turn_time;
  uint m_step;

 private:
  void BeginTurn();

 public:
  static AIStupidEngine* GetInstance();

  void Refresh();
};

#endif
