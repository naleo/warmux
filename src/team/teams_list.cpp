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
 * Team handling
 *****************************************************************************/

#include <algorithm>
#include <iostream>
#include "character/character.h"
#include "character/body_list.h"
#include "include/action.h"
#include "game/config.h"
#include "game/game_mode.h"
#include "network/network.h"
#include "network/randomsync.h"
#include "tool/file_tools.h"
#include "team/team.h"
#include "team/team_config.h"
#include "team/team_energy.h"
#include "team/teams_list.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TeamsList::TeamsList():
  full_list(),
  playing_list(),
  selection(),
  active_team(playing_list.end())
{
  LoadList();
}

TeamsList::~TeamsList()
{
  /* The teamslist was never built... nothing to delete.
   * FIXME This is needed because we are lead to delete things even if they
   * were not created completely. IMHO, this reflects the fact that the object
   * life time is not well known...
   * Actually, this is not that bad whereas free(NULL) is accepted... but it
   * remains spurious. */
  if (!singleton)
  {
    fprintf(stderr, "Destructor still called on unexisting TeamsList\n");
    return;
  }

  UnloadGamingData();
  Clear();
  for(full_iterator it = full_list.begin(); it != full_list.end(); ++it)
    delete (*it);
  full_list.clear();
  singleton = NULL;
}

//-----------------------------------------------------------------------------

void TeamsList::NextTeam ()
{
  Team* next = GetNextTeam();
  SetActive (next->GetId());

  if (GameMode::GetInstance()->auto_change_character) {
    ActiveTeam().NextCharacter();
  }

  Action a(Action::ACTION_GAMELOOP_NEXT_TEAM, next->GetId());
  Character::StoreActiveCharacter(&a);
  Network::GetInstance()->SendAction(a);

  printf("\nPlaying character : %i %s\n", ActiveCharacter().GetCharacterIndex(), ActiveCharacter().GetName().c_str());
  printf("Playing team : %i %s\n", ActiveCharacter().GetTeamIndex(), ActiveTeam().GetName().c_str());
  printf("Alive characters: %i / %i\n\n",ActiveTeam().NbAliveCharacter(),ActiveTeam().GetNbCharacters());
}

//-----------------------------------------------------------------------------

Team* TeamsList::GetNextTeam()
{
  // Next team
  std::vector<Team*>::iterator it=active_team;
  do
  {
    ++it;
    if (it == playing_list.end()) it = playing_list.begin();
  } while ((**it).NbAliveCharacter() == 0);
  return (*it);
}


//-----------------------------------------------------------------------------

Team& TeamsList::ActiveTeam()
{
  ASSERT (active_team != playing_list.end());
  return **active_team;
}

//-----------------------------------------------------------------------------

void TeamsList::LoadOneTeam(const std::string &dir, const std::string &team_name)
{
  // Skip '.', '..' and hidden files
  if (team_name[0] == '.') return;

  // Is it a directory ?
  if (!IsFolderExist(dir+team_name)) return;

  // Add the team
  try {
    full_list.push_back(new Team(dir, team_name));
    std::cout << ((1<full_list.size())?", ":" ") << team_name;
    std::cout.flush();
  }

  catch (char const *error) {
    std::cerr << std::endl
              << Format(_("Error loading team :")) << team_name <<":"<< error
              << std::endl;
    return;
  }
}

//-----------------------------------------------------------------------------

void TeamsList::LoadList()
{
  playing_list.clear() ;

  std::cout << "o " << _("Load teams:");

  const Config * config = Config::GetInstance();

  // Load Wormux teams
  std::string dirname = config->GetDataDir() + "team" PATH_SEPARATOR;
  FolderSearch *f = OpenFolder(dirname);
  if (f) {
    const char *name;
    while ((name = FolderSearchNext(f)) != NULL) LoadOneTeam(dirname, name);
    CloseFolder(f);
  } else {
    Error (Format(_("Cannot open teams directory (%s)!"), dirname.c_str()));
  }

  // Load personal teams
  dirname = config->GetPersonalDataDir() + "team" PATH_SEPARATOR;
  f = OpenFolder(dirname);
  if (f) {
    const char *name;
    while ((name = FolderSearchNext(f)) != NULL) LoadOneTeam(dirname, name);
    CloseFolder(f);
  } else {
    std::cerr << std::endl
      << Format(_("Cannot open personal teams directory (%s)!"), dirname.c_str())
      << std::endl;
  }

  full_list.sort(compareTeams);

  // We need at least 2 teams
  if (full_list.size() < 2)
    Error(_("You need at least two valid teams !"));

  // Default selection
  std::list<uint> nv_selection;
  nv_selection.push_back (0);
  nv_selection.push_back (1);
  ChangeSelection (nv_selection);

  std::cout << std::endl;
  InitList(Config::GetInstance()->AccessTeamList());
}

//-----------------------------------------------------------------------------

void TeamsList::LoadGamingData()
{
  std::sort(playing_list.begin(), playing_list.end(), compareTeams); // needed to fix bug #9820
  active_team = playing_list.begin();

  iterator it=playing_list.begin(), end=playing_list.end();

  // Load the data of all teams
  for (; it != end; ++it) {

    // Local or AI ?
    if ( (*it)->IsLocal() && (*it)->GetPlayerName() == "AI-stupid")
      (*it)->SetLocalAI();

    (**it).LoadGamingData();
  }
}

void TeamsList::RandomizeFirstPlayer()
{
  active_team = playing_list.begin();
  int skip = RandomSync().GetLong(0, playing_list.size() - 1);
  for(int i = 0; i < skip; i++)
    active_team++;
}

//-----------------------------------------------------------------------------

void TeamsList::UnloadGamingData()
{
  BodyList::GetRef().FreeMem();
  iterator it=playing_list.begin(), end = playing_list.end();

  // Unload the data of all teams
  for (; it != end; ++it) (**it).UnloadGamingData();
}

//-----------------------------------------------------------------------------

Team *TeamsList::FindById (const std::string &id, int &pos)
{
  full_iterator it=full_list.begin(), end = full_list.end();
  int i=0;
  for (; it != end; ++it, ++i)
  {
    if ((*it)->GetId() == id)
    {
      pos = i;
      return (*it);
    }
  }
  pos = -1;
  return NULL;
}

//-----------------------------------------------------------------------------

Team *TeamsList::FindByIndex (uint index)
{
  full_iterator it=full_list.begin(), end = full_list.end();
  uint i=0;
  for (; it != end; ++it, ++i)
  {
    if (i == index)
      return (*it);
  }
  ASSERT (false);
  return NULL;
}

//-----------------------------------------------------------------------------

Team *TeamsList::FindPlayingByIndex (uint index)
{
  ASSERT(index < playing_list.size());
  return playing_list[index];
}

//-----------------------------------------------------------------------------

Team* TeamsList::FindPlayingById(const std::string &id, int &index)
{
  iterator it = playing_list.begin(), end = playing_list.end();
  index=0;
  for (; it != end; ++it, ++index)
  {
    if ((*it) -> GetId() == id)
      return *it;
  }

  index = -1;
  ASSERT(false);
  return NULL;
}

//-----------------------------------------------------------------------------

void TeamsList::InitList (const std::list<ConfigTeam> &lst)
{
  Clear();
  std::list<ConfigTeam>::const_iterator it=lst.begin(), end=lst.end();
  for (; it != end; ++it) {
    AddTeam (*it, true, false);
  }
  active_team = playing_list.begin();
}

//-----------------------------------------------------------------------------

void TeamsList::InitEnergy()
{
  // Looking at team with the greatest energy
  // (in case teams does not have same amount of character)
  iterator it=playing_list.begin(), end = playing_list.end();
  uint max = 0;
  for (; it != end; ++it)
  {
    if( (**it).ReadEnergy() > max)
      max = (**it).ReadEnergy();
  }

  // Init each team's energy bar
  it=playing_list.begin();
  for (; it != end; ++it)
  {
    (**it).InitEnergy (max);
  }

  // Initial ranking
  it=playing_list.begin();
  for (; it != end; ++it)
  {
    uint rank = 0;
    iterator it2=playing_list.begin();
    for (; it2 != end; ++it2)
    {
      if((it != it2)
          && (**it2).ReadEnergy() > (**it).ReadEnergy() )
        ++rank;
    }
    (**it).energy.rank_tmp = rank;
  }
  it=playing_list.begin();
  for (; it != end; ++it)
  {
    uint rank = (**it).energy.rank_tmp;
    iterator it2=playing_list.begin();
    for (it2 = it; it2 != end; ++it2)
    {
      if((it != it2)
          && (**it2).ReadEnergy() == (**it).ReadEnergy() )
        ++rank;
    }
    (**it).energy.SetRanking(rank);
  }
}

//-----------------------------------------------------------------------------

void TeamsList::RefreshEnergy()
{
  // In the order of the priorit :
  // - finish current action
  // - change a teams energy
  // - change ranking
  // - prepare energy bar for next event

  iterator it=playing_list.begin(), end = playing_list.end();
  energy_t status;

  bool waiting = true; // every energy bar are waiting

  for (; it != end; ++it) {
    if( (**it).energy.status != EnergyStatusWait) {
      waiting = false;
      break;
    }
  }

  // one of the energy bar is changing ?
  if(!waiting) {
    status = EnergyStatusOK;

    // change an energy bar value ?
    for (it=playing_list.begin(); it != end; ++it) {
      if( (**it).energy.status == EnergyStatusValueChange) {
        status = EnergyStatusValueChange;
        break;
      }
    }

    // change a ranking ?
    for (it=playing_list.begin(); it != end; ++it) {
      if( (**it).energy.status == EnergyStatusRankChange
             && ((**it).energy.IsMoving() || status == EnergyStatusOK)) {
        status = EnergyStatusRankChange;
        break;
      }
    }
  }
  else {
    // every energy bar are waiting
    // -> set state ready for a new event
    status = EnergyStatusOK;
  }

  // Setting event to process in every energy bar
  if(status != EnergyStatusOK || waiting) {
    it=playing_list.begin();
    for (; it != end; ++it) {
      (**it).energy.status = status;
    }
  }

  // Actualisation des valeurs (pas d'actualisation de l'affichage)
  for (it=playing_list.begin(); it != end; ++it) {
    (**it).UpdateEnergyBar();
    RefreshSort();
  }
}
//-----------------------------------------------------------------------------

void TeamsList::RefreshSort ()
{
  iterator it=playing_list.begin(), end = playing_list.end();
  uint rank;

  // Find a ranking without taking acount of the equalities
  it=playing_list.begin();
  for (; it != end; ++it)
  {
    rank = 0;
    iterator it2=playing_list.begin();
    for (; it2 != end; ++it2)
    {
      if((it != it2)
          && (**it2).ReadEnergy() > (**it).ReadEnergy() )
        ++rank;
    }
    (**it).energy.rank_tmp = rank;
  }

  // Fix equalities
  it=playing_list.begin();
  for (; it != end; ++it)
  {
    rank = (**it).energy.rank_tmp;
    iterator it2=playing_list.begin();
    for (it2 = it; it2 != end; ++it2)
    {
      if((it != it2)
          && (**it2).ReadEnergy() == (**it).ReadEnergy() )
        ++rank;
    }
    (**it).energy.NewRanking(rank);
  }
}

//-----------------------------------------------------------------------------

void TeamsList::ChangeSelection (const std::list<uint>& nv_selection)
{
  selection = nv_selection;

  selection_iterator it=selection.begin(), end = selection.end();
  playing_list.clear();
  for (; it != end; ++it) playing_list.push_back (FindByIndex(*it));
  active_team = playing_list.begin();
}

//-----------------------------------------------------------------------------

bool TeamsList::IsSelected (uint index)
{
  selection_iterator pos = std::find (selection.begin(), selection.end(), index);
  return pos != selection.end();
}

void TeamsList::Clear()
{
  selection.clear();
  playing_list.clear();
}

//-----------------------------------------------------------------------------

void TeamsList::AddTeam(Team* the_team, int pos, const ConfigTeam &the_team_cfg,
			bool is_local)
{
  ASSERT(the_team != NULL);

  if (is_local) {
    the_team->SetLocal();
  } else {
    the_team->SetRemote();
  }
  UpdateTeam(the_team, the_team_cfg);

  selection.push_back (pos);
  playing_list.push_back (the_team);

  active_team = playing_list.begin();
}

void TeamsList::AddTeam(const ConfigTeam &the_team_cfg, bool is_local,
			bool generate_error)
{
  int pos;
  Team *the_team = FindById (the_team_cfg.id, pos);
  if (the_team != NULL) {

    AddTeam(the_team, pos, the_team_cfg, is_local);

  } else {
    std::string msg = Format(_("Can't find team %s!"), the_team_cfg.id.c_str());
    if (generate_error)
      Error (msg);
    else
      std::cout << "! " << msg << std::endl;
  }
}

//-----------------------------------------------------------------------------

void TeamsList::UpdateTeam(Team* the_team, const ConfigTeam &the_team_cfg)
{
  ASSERT(the_team != NULL);

  // set the player name and number of characters
  the_team->SetPlayerName(the_team_cfg.player_name);
  the_team->SetNbCharacters(the_team_cfg.nb_characters);
}

void TeamsList::UpdateTeam (const std::string& old_team_id,
			    const ConfigTeam &the_team_cfg)
{
  int pos;

  if (old_team_id == the_team_cfg.id) {
    // this is a simple update

    Team *the_team = FindById (the_team_cfg.id, pos);
    if (the_team != NULL) {
      UpdateTeam(the_team, the_team_cfg);
    } else {
      Error(Format(_("Can't find team %s!"), the_team_cfg.id.c_str()));
      return;
    }

  } else {

    // here we are replacing a team by another one
    Team *the_old_team = FindById (old_team_id, pos);
    if (the_old_team == NULL) {
      Error(Format(_("Can't find team %s!"), old_team_id.c_str()));
      return;
    }

    Team *the_team = FindById (the_team_cfg.id, pos);
    if (the_team == NULL) {
      Error(Format(_("Can't find team %s!"), old_team_id.c_str()));
      return;
    }

    bool is_local = (the_old_team->IsLocal() || the_old_team->IsLocalAI());
    DelTeam(the_old_team);
    AddTeam(the_team, pos, the_team_cfg, is_local);
  }

}

//-----------------------------------------------------------------------------

void TeamsList::DelTeam(Team* the_team)
{
  uint pos = 0;

  ASSERT(the_team != NULL);

  the_team->SetDefaultPlayingConfig();

  selection_iterator it = find(selection.begin(), selection.end(), pos);

  if (it != selection.end()) {
    selection.erase(it);
  }

  iterator playing_it = find(playing_list.begin(), playing_list.end(), the_team);

  ASSERT(playing_it != playing_list.end());

  if (playing_it != playing_list.end()) {
    playing_list.erase(playing_it);
  }

  active_team = playing_list.begin();
}

void TeamsList::DelTeam(const std::string &id)
{
  int pos;
  Team *the_team = FindById (id, pos);

  DelTeam(the_team);
}

//-----------------------------------------------------------------------------

void TeamsList::SetActive(const std::string &id)
{
  iterator it = playing_list.begin(),
  end = playing_list.end();
  for (; it != end; ++it)
  {
    Team &team = **it;
    if (team.GetId() == id)
    {
      active_team = it;
      ActiveTeam().PrepareTurn();
      return;
    }
  }
  Error (Format(_("Can't find team %s!"), id.c_str()));
}

//-----------------------------------------------------------------------------

std::string TeamsList::GetLocalHeadCommanders() const
{
  std::string nickname;

  for (std::vector<Team*>::const_iterator it = playing_list.begin();
       it != playing_list.end();
       it++) {
    if ((*it)->IsLocal()) {
      if (nickname != "") nickname += "+";

      nickname += (*it)->GetPlayerName();
    }
  }

  return nickname;
}

//-----------------------------------------------------------------------------

Team& ActiveTeam()
{
  return GetTeamsList().ActiveTeam();
}

//-----------------------------------------------------------------------------

Character& ActiveCharacter()
{
  return ActiveTeam().ActiveCharacter();
}

//-----------------------------------------------------------------------------

bool compareTeams(const Team *a, const Team *b)
{
  return a->GetName() < b->GetName();
}

//-----------------------------------------------------------------------------
