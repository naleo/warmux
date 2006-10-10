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

#include "game_menu.h"

#include "../game/game.h"
#include "../game/config.h"
#include "../game/game_mode.h"
#include "../graphic/video.h"
#include "../graphic/font.h"
#include "../map/maps_list.h"
#include "../include/app.h"
#include "../team/teams_list.h"
#include "../tool/i18n.h"
#include "../tool/string_tools.h"

const uint MARGIN_TOP    = 5;
const uint MARGIN_SIDE   = 5;
const uint MARGIN_BOTTOM = 70;

const uint TEAMS_W = 160;
const uint TEAMS_H = 160;
const uint TEAM_LOGO_Y = 290;
const uint TEAM_LOGO_H = 48;

const uint MAPS_X = 20;
const uint MAPS_W = 160;

const uint NBR_VER_MIN = 1;
const uint NBR_VER_MAX = 10;
const uint TPS_TOUR_MIN = 10;
const uint TPS_TOUR_MAX = 120;
const uint TPS_FIN_TOUR_MIN = 1;
const uint TPS_FIN_TOUR_MAX = 10;

TeamSelection::TeamSelection() : HBox(Rectanglei(0, 0, 180, 80), false)
{
  associated_team=NULL;

  team_logo = new PictureWidget( Rectanglei(0,0,48,48) );
  AddWidget(team_logo);
  
  Box * tmp_box = new VBox(Rectanglei(0, 0, 130, 80), false);
  team_name = new Label(_(" "), Rectanglei(0,0,130,0), 
			*Font::GetInstance(Font::FONT_NORMAL), gray_color);
  team_player = new Label(_(" "), Rectanglei(0,0,130,0), 
			*Font::GetInstance(Font::FONT_NORMAL), gray_color);
  //nb_characters = NULL;
  tmp_box->AddWidget(team_name);
  tmp_box->AddWidget(team_player);

  AddWidget(tmp_box);
}

void TeamSelection::SetTeam(Team& _team)
{
  associated_team=&_team;
  
  team_logo->SetSurface(_team.flag);
  team_name->SetText(_team.GetName());
  team_logo->SetSurface(_team.flag);
}

void TeamSelection::ClearTeam()
{
  associated_team=NULL;

  team_logo->SetNoSurface();
  team_name->SetText(" ");
  team_player->SetText(" ");
}

Team* TeamSelection::GetTeam() const
{
  return associated_team;
}

// ################################################
// ##  GAME MENU CLASS
// ################################################
GameMenu::GameMenu() :
  Menu("menu/bg_play")
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml",false);
  Rectanglei rectZero(0, 0, 0, 0);
  Rectanglei stdRect (0, 0, 130, 30);

  Font * normal_font = Font::GetInstance(Font::FONT_NORMAL);

  Surface window = AppWormux::GetInstance()->video.window;

  // Calculate main box size
  uint mainBoxWidth = window.GetWidth() - 2*MARGIN_SIDE;
  uint mainBoxHeight = (window.GetHeight() - MARGIN_TOP - MARGIN_BOTTOM - 2*MARGIN_SIDE)/3;

  // ################################################
  // ##  TEAM SELECTION
  // ################################################
  Box * team_box = new HBox(Rectanglei(MARGIN_SIDE, MARGIN_TOP,
				       mainBoxWidth, mainBoxHeight));
  team_box->AddWidget(new PictureWidget(Rectanglei(0,0,38,150), "menu/teams_label"));

  // How many teams ?
  teams_nb = new SpinButtonBig(_("Number of teams:"), stdRect,
			       2, 1,
			       2, MAX_NB_TEAMS);
  team_box->AddWidget(teams_nb);
  
  Box * top_n_bottom_team_options = new VBox( Rectanglei(0, 0, mainBoxWidth-255, 0),false);
  Box * top_team_options = new HBox ( Rectanglei(0, 0, 0, mainBoxHeight/2 - 20), false);
  Box * bottom_team_options = new HBox ( Rectanglei(0, 0, 0, mainBoxHeight/2 - 20), false);
  top_team_options->SetMargin(25);
  bottom_team_options->SetMargin(25);
  
  // Initialize teams
  for (uint i=0; i < MAX_NB_TEAMS; i++) {
    teams_selections[i] = new TeamSelection();
    if ( i%2 == 0)
      top_team_options->AddWidget(teams_selections[i]);
    else
      bottom_team_options->AddWidget(teams_selections[i]);
  }

  top_n_bottom_team_options->AddWidget(top_team_options);
  top_n_bottom_team_options->AddWidget(bottom_team_options);
  
  team_box->AddWidget(top_n_bottom_team_options);

  widgets.AddWidget(team_box);

  // ################################################
  // ##  MAP SELECTION
  // ################################################
  map_box = new HBox( Rectanglei(MARGIN_SIDE, team_box->GetPositionY()+team_box->GetSizeY()+ MARGIN_SIDE,
				       mainBoxWidth, mainBoxHeight));
  map_box->AddWidget(new PictureWidget(Rectanglei(0,0,46,100), "menu/map_label"));

  // PreviousMap/NextMap buttons
  bt_map_plus = new Button(Point2i(0, 0), res, "menu/big_plus");
  bt_map_minus = new Button(Point2i(0, 0), res, "menu/big_minus");

  Box * tmp_map_box = new VBox( Rectanglei(MARGIN_SIDE, team_box->GetPositionY()+team_box->GetSizeY()+ MARGIN_SIDE,
					   mainBoxWidth-130, mainBoxHeight), false);
  tmp_map_box->SetMargin(0);
  tmp_map_box->SetBorder( Point2i(0,0) );
  uint map_preview_height = mainBoxHeight -2*10 -40;

  // Previews
  Box* previews_box = new HBox( Rectanglei(0, 0, 1, map_preview_height+10 ), false);
  previews_box->SetMargin(10);
  previews_box->SetBorder( Point2i(0,0) );

  previews_box->AddWidget(bt_map_minus);

  map_preview_before2 = new PictureWidget(Rectanglei(0, 0, map_preview_height*4/3 *3/4, map_preview_height*3/4));
  previews_box->AddWidget(map_preview_before2);

  map_preview_before = new PictureWidget(Rectanglei(0, 0, map_preview_height*4/3 *3/4, map_preview_height*3/4));
  previews_box->AddWidget(map_preview_before);

  // Selected map...
  map_preview_selected = new PictureWidget(Rectanglei(0, 0, map_preview_height*4/3, map_preview_height));
  previews_box->AddWidget(map_preview_selected);

  map_preview_after = new PictureWidget(Rectanglei(0, 0, map_preview_height*4/3 *3/4, map_preview_height*3/4));
  previews_box->AddWidget(map_preview_after);

  map_preview_after2 = new PictureWidget(Rectanglei(0, 0, map_preview_height*4/3 *3/4, map_preview_height*3/4));
  previews_box->AddWidget(map_preview_after2);

  previews_box->AddWidget(bt_map_plus);

  tmp_map_box->AddWidget(previews_box);

  // Map information
  map_name_label = new Label("Map", Rectanglei(0,0,0,0), *normal_font, gray_color, true);
  tmp_map_box->AddWidget(map_name_label);

  map_author_label = new Label("Author", Rectanglei(0,0,0,0), *Font::GetInstance(Font::FONT_SMALL), gray_color, true);
  tmp_map_box->AddWidget(map_author_label);

  map_box->AddWidget(tmp_map_box);

  widgets.AddWidget(map_box);

  // ################################################
  // ##  GAME OPTIONS
  // ################################################
  game_options = new HBox( Rectanglei(MARGIN_SIDE, map_box->GetPositionY()+map_box->GetSizeY()+ MARGIN_SIDE,
					    mainBoxWidth/2, mainBoxHeight), true);
  game_options->AddWidget(new PictureWidget(Rectanglei(0,0,39,128), "menu/mode_label"));

  Box * all_game_options = new VBox( Rectanglei(0, 0, mainBoxWidth/2-40, mainBoxHeight), false);

  Box * top_game_options = new HBox ( Rectanglei(0, 0, mainBoxWidth/2, mainBoxHeight/2), false);
  Box * bottom_game_options = new HBox ( Rectanglei(0, 0, mainBoxWidth/2, mainBoxHeight/2), false);
  top_game_options->SetMargin(25);
  bottom_game_options->SetMargin(25);

  opt_duration_turn = new SpinButtonWithPicture(_("Duration of a turn:"), "menu/timing_turn",
						stdRect,
						TPS_TOUR_MIN, 5,
						TPS_TOUR_MIN, TPS_TOUR_MAX);
  bottom_game_options->AddWidget(opt_duration_turn);

  opt_duration_end_turn = new SpinButtonWithPicture(_("Duration of the end of a turn:"), "menu/timing_end_of_turn",
						    stdRect,
						    TPS_FIN_TOUR_MIN, 1,
						    TPS_FIN_TOUR_MIN, TPS_FIN_TOUR_MAX);
  bottom_game_options->AddWidget(opt_duration_end_turn);

  opt_nb_characters = new SpinButtonBig(_("Number of players per team:"), stdRect,
				     4, 1,
				     NBR_VER_MIN, NBR_VER_MAX);
  top_game_options->AddWidget(opt_nb_characters);

  opt_energy_ini = new SpinButtonWithPicture(_("Initial energy:"), "menu/energy",
					     stdRect,
					     100, 5,
					     50, 200);

  top_game_options->AddWidget(opt_energy_ini);

  all_game_options->AddWidget(top_game_options);
  all_game_options->AddWidget(bottom_game_options);
  game_options->AddWidget(all_game_options);
  widgets.AddWidget(game_options);


  // Values initialization

  // Load Maps' list
  std::sort(MapsList::GetInstance()->lst.begin(), MapsList::GetInstance()->lst.end(), compareMaps);
  selected_map_index = MapsList::GetInstance()->GetActiveMapIndex();
  ChangeMap(0);

  // Load Teams' list
  teams_list.full_list.sort(compareTeams);

  TeamsList::full_iterator
    it=teams_list.full_list.begin(),
    end=teams_list.full_list.end();

  uint i=0, j=0;
  for (; it != end; ++it)
  {
    bool choix = teams_list.IsSelected (i);
    if (choix) {
      teams_selections[j]->SetTeam(*it);
      j++;
    }
    ++i;
  }

  if (j < 2) {
    SetNbTeams(2);
    teams_nb->SetValue(2);
  } else {
    teams_nb->SetValue(j);
  }

  // Load game options
  GameMode * game_mode = GameMode::GetInstance();
  opt_duration_turn->SetValue(game_mode->duration_turn);
  opt_duration_end_turn->SetValue(game_mode->duration_move_player);
  opt_nb_characters->SetValue(game_mode->max_characters);
  opt_energy_ini->SetValue(game_mode->character.init_energy);


  resource_manager.UnLoadXMLProfile(res);
}

GameMenu::~GameMenu()
{
}

void GameMenu::OnClic(const Point2i &mousePosition, int button)
{
  if ( game_options->Clic(mousePosition, button)) {
  
  } else if (button == SDL_BUTTON_LEFT && map_preview_before2->Contains(mousePosition) ) {
    ChangeMap(-2);
  } else if (   (button == SDL_BUTTON_LEFT && bt_map_minus->Contains(mousePosition))
	     || (button == SDL_BUTTON_LEFT && map_preview_before->Contains(mousePosition)) 
	     || (button == SDL_BUTTON_WHEELUP && map_box->Contains(mousePosition))) {
    ChangeMap(-1);
  } else if (   (button == SDL_BUTTON_LEFT && bt_map_plus->Contains(mousePosition)) 
	     || (button == SDL_BUTTON_LEFT && map_preview_after->Contains(mousePosition)) 
	     || (button == SDL_BUTTON_WHEELDOWN && map_box->Contains(mousePosition))) {
    ChangeMap(+1);
  } else if (map_preview_after2->Contains(mousePosition) ) {
    ChangeMap(+2);
  } else  if (teams_nb->Clic(mousePosition, button)){
    SetNbTeams(teams_nb->GetValue());

  } else {
    for (uint i=0; i<MAX_NB_TEAMS ; i++) {
      if ( teams_selections[i]->Contains(mousePosition)) {
	NextTeam(i);
	break;
      }
    }
  }

}

void GameMenu::NextTeam(int i)
{
  int index = -1;
  teams_list.FindById(teams_selections[i]->GetTeam()->GetId(), index);
  if (index > -1) {
    index++;
    if (index < int(teams_list.full_list.size()))
      teams_selections[i]->SetTeam(*(teams_list.FindByIndex(index)));
    else index = -1;
  }
  
  if (index == -1)
    teams_selections[i]->SetTeam(*(teams_list.FindByIndex(0)));
}

void GameMenu::SaveOptions()
{
  MapsList::GetInstance()->SelectMapByIndex(selected_map_index);

  // teams
  std::list<uint> selection;

  uint nb_teams=0;
  for (uint i=0; i < MAX_NB_TEAMS; i++) {
    if (teams_selections[i]->GetTeam() != NULL)
      nb_teams++;
  }

  if (nb_teams >= 2) {
    std::list<uint> selection;
    
    for (uint i=0; i < MAX_NB_TEAMS; i++) {
      if (teams_selections[i]->GetTeam() != NULL) {
	int index = -1;
	
	teams_list.FindById(teams_selections[i]->GetTeam()->GetId(), index);
	if (index > -1)
	  selection.push_back(uint(index));
      }
    }
    teams_list.ChangeSelection (selection);
  }

  //Save options in XML
  Config::GetInstance()->Save();

  GameMode * game_mode = GameMode::GetInstance();
  game_mode->duration_turn = opt_duration_turn->GetValue() ;
  game_mode->duration_move_player = opt_duration_end_turn->GetValue() ;
  game_mode->max_characters = opt_nb_characters->GetValue() ;

  game_mode->character.init_energy = opt_energy_ini->GetValue() ;

}

void GameMenu::__sig_ok()
{
  SaveOptions();
  Game::GetInstance()->Start();
}

void GameMenu::__sig_cancel()
{
  // Nothing to do
}

void GameMenu::ChangeMap(int delta_index)
{
  int tmp = selected_map_index + delta_index;
  if (tmp < 0 || tmp > int(MapsList::GetInstance()->lst.size() - 1)) return;

  selected_map_index = tmp;

  map_preview_selected->SetSurface(MapsList::GetInstance()->lst[selected_map_index].preview, true);
  map_name_label->SetText(MapsList::GetInstance()->lst[selected_map_index].name);
  map_author_label->SetText(MapsList::GetInstance()->lst[selected_map_index].author_info);

  if (selected_map_index > 0)
    map_preview_before->SetSurface(MapsList::GetInstance()->lst[selected_map_index-1].preview, true);
  else
    map_preview_before->SetNoSurface();

  if (selected_map_index > 1)
    map_preview_before2->SetSurface(MapsList::GetInstance()->lst[selected_map_index-2].preview, true);
  else
    map_preview_before2->SetNoSurface();

  if (selected_map_index+1 < MapsList::GetInstance()->lst.size() )
    map_preview_after->SetSurface(MapsList::GetInstance()->lst[selected_map_index+1].preview, true);
  else
    map_preview_after->SetNoSurface();

  if (selected_map_index+2 < MapsList::GetInstance()->lst.size() )
    map_preview_after2->SetSurface(MapsList::GetInstance()->lst[selected_map_index+2].preview, true);
  else
    map_preview_after2->SetNoSurface();
}

void GameMenu::SetNbTeams(uint nb_teams)
{
  // we hide the useless teams selector
  for (uint i=nb_teams; i<MAX_NB_TEAMS; i++) {
    teams_selections[i]->ClearTeam();
  }

  for (uint i=0; i<nb_teams;i++) {
    if (teams_selections[i]->GetTeam() == NULL) {
      // we should find an available team
      teams_selections[i]->SetTeam(*(teams_list.FindByIndex(i)));
    }
  }
}

void GameMenu::Draw(const Point2i &mousePosition)
{
//   int t = lbox_all_teams->MouseIsOnWhichItem(mousePosition);
//   if (t != -1) {
//     int index = -1;
//     Team * new_team = teams_list.FindById(lbox_all_teams->ReadValue(t), index);
//     SelectTeamLogo(new_team);
//   } else {
//     t = lbox_selected_teams->MouseIsOnWhichItem(mousePosition);
//     if (t != -1) {
//       int index = -1;
//       Team * new_team = teams_list.FindById(lbox_selected_teams->ReadValue(t), index);
//       SelectTeamLogo(new_team);
//     }
//   }
}

