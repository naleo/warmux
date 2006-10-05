/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
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
 * Game menu
 *****************************************************************************/

#ifndef GAME_MENU_H
#define GAME_MENU_H

#include "menu.h"
#include "../include/base.h"
#include "../graphic/font.h"

class Team;

/* class TeamSelection */
/* { */
/*   Box * container; */
/*   PictureWidget *team_logo; */
/*   Label * team_name; */

/*  protected: */
/*   TeamSelection(); */
/*   void SetTeam(Team& _team); */
/* } */

class GameMenu : public Menu
{
   /* Team controllers */
   PictureWidget* teams_logos[4];
   Label* teams_names[4];
   Team* teams[4];
   SpinButtonBig *teams_nb;

 /*   ListBox *lbox_all_teams;    */
/*    ListBox *lbox_selected_teams; */
/*    Button *bt_add_team; */
/*    Button *bt_remove_team; */
/*    PictureWidget *team_logo; */

   Team * last_team;

   /* Map controllers */
   uint selected_map_index;
   PictureWidget *map_preview_selected;
   PictureWidget *map_preview_before, *map_preview_before2;
   PictureWidget *map_preview_after, *map_preview_after2;  
   Label *map_name_label;
   Label *map_author_label;
   Button *bt_map_plus, *bt_map_minus;
   
   /* Game options controllers */
   Box * game_options;
   SpinButtonWithPicture *opt_duration_turn;
   SpinButtonWithPicture *opt_duration_end_turn;
   SpinButtonBig *opt_nb_characters;
   SpinButtonWithPicture *opt_energy_ini;


   void ChangeMap(int delta_index);   

   void SelectTeam(Team& t, int id);
   void SetNbTeams(uint nb_teams);

   void SaveOptions();
   void OnClic(const Point2i &mousePosition, int button);
   void Draw(const Point2i &mousePosition);

   void __sig_ok();
   void __sig_cancel();

public:
   GameMenu(); 
   ~GameMenu();
};

#endif
