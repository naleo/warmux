/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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
 * Define all Wormux actions.
 *****************************************************************************/

#ifndef ACTION_HANDLER_H
#define ACTION_HANDLER_H
//-----------------------------------------------------------------------------
#include <map>
#include <list>
#include "include/action.h"
#include "include/base.h"
#include <WORMUX_singleton.h>
//-----------------------------------------------------------------------------

// Forward declarations
struct SDL_mutex;

class ActionHandler : public Singleton<ActionHandler>
{
private:
  /* If you need this, you probably made an error in your code... */
  ActionHandler(const ActionHandler&);
  const ActionHandler& operator=(const ActionHandler&);
  /****************************************************************/

  // Mutex needed to be thread safe for the network
  SDL_mutex* mutex;

  // Handler for each action
  typedef void (*callback_t) (Action *a);
  std::map<Action::Action_t, callback_t> handler;
  typedef std::map<Action::Action_t, callback_t>::const_iterator handler_it;

  // Action strings
  std::map<Action::Action_t, std::string> action_name;
  typedef std::map<Action::Action_t, std::string>::const_iterator name_it;

  // Action queue
  std::list<Action*> queue;

  void Exec(Action *a);

protected:
  friend class Singleton<ActionHandler>;
  ActionHandler();
  ~ActionHandler();

public:
  void NewAction(Action* a, bool repeat_to_network=true);
  void NewActionActiveCharacter(Action* a); // send infos (on the network) about active character in the same time

  void Flush();
  void ExecActions();

  void Lock();
  void UnLock();

  // To call when locked
  void Register(Action::Action_t action, const std::string &name, callback_t fct);

  const std::string &GetActionName(Action::Action_t action) const;
};

void Action_Handler_Init();

// TODO: Move it in an object !

// Send character information over the network (it's totally stupid to send it locally ;-)
void SendCharacterInfo(int team_no, int char_no);
void SendActiveCharacterInfo(bool can_be_dropped = false);

// Send character information + an action over the network
// WARNING: it does not post the action in local queue!!
void SendActiveCharacterAction(const Action& a);

void SendGameMode();
void SyncCharacters();



//-----------------------------------------------------------------------------
#endif
