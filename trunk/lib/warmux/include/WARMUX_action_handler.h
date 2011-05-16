/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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
 * Define all Warmux actions.
 *****************************************************************************/

#ifndef WARMUX_ACTION_HANDLER_H
#define WARMUX_ACTION_HANDLER_H
//-----------------------------------------------------------------------------
#include <map>
#include <list>
#include <WARMUX_action.h>
#include <WARMUX_types.h>
//-----------------------------------------------------------------------------

// Forward declarations
struct SDL_mutex;

class WActionHandler
{
  // Mutex needed to handle action queue in a thread-safe way (network etc)
  SDL_mutex* mutex;

  // Handler for each action
  typedef void (*callback_t) (Action *a);
  static callback_t handlers[Action::NUM_ACTIONS];

  // Action strings
  static std::string action_names[Action::NUM_ACTIONS];

protected:
  WActionHandler();
  ~WActionHandler();

  // Action queue
  std::list<Action*> queue;

  void Exec(Action *a) { handlers[a->GetType()](a); }
  void NewAction(Action* a);

public:
  void Flush();
  void ExecActions();

  inline void Lock();
  inline void UnLock();

  // To call when locked
  void Register(Action::Action_t action, const std::string &name, callback_t fct)
  {
    handlers[action] = fct;
    action_names[action] = name;
  }

  const std::string& GetActionName(Action::Action_t a) const { return action_names[a]; }

  bool IsEmpty() const { return queue.empty(); }
};

#endif
