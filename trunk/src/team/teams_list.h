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
 * Refresh des diff�entes �uipes.
 *****************************************************************************/

#ifndef GESTION_EQUIPE_H
#define GESTION_EQUIPE_H
//-----------------------------------------------------------------------------
#include "team.h"
#include <list>
//-----------------------------------------------------------------------------

class TeamsList
{
public:
  typedef std::list<Team>::iterator full_iterator;
  typedef std::vector<Team*>::iterator iterator;
  std::list<Team> full_list;
  std::vector<Team*> playing_list;

private:
  typedef std::list<uint>::iterator selection_iterator;
  std::list<uint> selection;
  std::vector<Team*>::iterator m_equipe_active;
  void LoadOneTeam (const std::string &dir, const std::string &file);

public:
  TeamsList();
  ~TeamsList();
  void LoadList ();
  void NextTeam (bool debut_jeu);
  Team& ActiveTeam();
  void LoadGamingData(uint how_many_characters);
  void UnloadGamingData();
  void Clear();


  // Add a new team to playing, and change active team
  void AddTeam (const std::string &id, bool generate_error=true);
  void DelTeam (const std::string &id);
  void SetActive(const std::string &id);
  void InitList (const std::list<std::string> &liste_nom);
  void InitEnergy ();
  void RefreshEnergy (); //Actualise les jauges d'�ergie
  void RefreshSort (); //Actualise le classement des jauges
  void ChangeSelection (const std::list<uint>& liste);
  bool IsSelected (uint index);

  // Find a team by its id or index (in full_list)
  Team* FindById (const std::string &id, int &pos);
  Team* FindByIndex (uint index);
  // Find a team by its id or index (in playing full_list)
  Team* FindPlayingById(const std::string &id, uint &index);
  Team* FindPlayingByIndex(uint index);
};

extern TeamsList teams_list;
//-----------------------------------------------------------------------------

// Team active
Team& ActiveTeam();

// Ver actif
Character& ActiveCharacter();

//-----------------------------------------------------------------------------

bool compareTeams(const Team& a, const Team& b);

//-----------------------------------------------------------------------------
#endif
