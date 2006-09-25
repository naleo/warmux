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

const uint MARGIN_TOP    = 10;
const uint MARGIN_SIDE   = 5;
const uint MARGIN_BOTTOM = 70;

const uint TEAMS_W = 160;
const uint TEAMS_H = 160;
const uint TEAM_LOGO_Y = 290;
const uint TEAM_LOGO_H = 48;

const uint MAPS_X = 20;
const uint MAPS_W = 160;

const uint NBR_VER_MIN = 1;
const uint NBR_VER_MAX = 6;
const uint TPS_TOUR_MIN = 10;
const uint TPS_TOUR_MAX = 120;
const uint TPS_FIN_TOUR_MIN = 1;
const uint TPS_FIN_TOUR_MAX = 10;

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

  /* Choose the teams !! */  
  Box * team_box = new HBox(Rectanglei(MARGIN_SIDE, MARGIN_TOP, 
				       mainBoxWidth, mainBoxHeight));
  team_box->AddWidget(new PictureWidget(Rectanglei(0,0,38,150), "menu/teams_label"));

  Box * tmp_team_box = new VBox(Rectanglei(MARGIN_SIDE, MARGIN_TOP, 
					   mainBoxWidth-60, mainBoxHeight), false);
  Label * select_teams_label = new Label(_("Select the teams:"), rectZero, *normal_font);

  tmp_team_box->AddWidget(select_teams_label);

  Box * tmp_box = new HBox( Rectanglei(0,0, mainBoxWidth, 
				       mainBoxHeight - select_teams_label->GetSizeY())
			    , false);
  tmp_box->SetMargin(10);
  tmp_box->SetBorder( Point2i(0,0) );

  lbox_all_teams = new ListBox( Rectanglei( 0, 0, TEAMS_W, tmp_box->GetSizeY() - 2*10 ));
  lbox_all_teams->always_one_selected = false;
  tmp_box->AddWidget(lbox_all_teams);

  Box * buttons_tmp_box = new VBox(Rectanglei(0, 0, 68, 1), false);

  bt_add_team = new Button( Point2i(0, 0) ,res,"menu/arrow-right");
  buttons_tmp_box->AddWidget(bt_add_team);

  bt_remove_team = new Button( Point2i( 0, 0),res,"menu/arrow-left");
  buttons_tmp_box->AddWidget(bt_remove_team);

  team_logo = new PictureWidget( Rectanglei(0,0,48,48) );
  buttons_tmp_box->AddWidget(team_logo);
  last_team = NULL;

  tmp_box->AddWidget(buttons_tmp_box);
  lbox_selected_teams = new ListBox( Rectanglei(0, 0, TEAMS_W, tmp_box->GetSizeY() - 2*10 ));
  lbox_selected_teams->always_one_selected = false;
  tmp_box->AddWidget(lbox_selected_teams);

  tmp_team_box->AddWidget(tmp_box);
  team_box->AddWidget(tmp_team_box);
  widgets.AddWidget(team_box);

  /* Choose the map !! */
  Box * map_box = new HBox( Rectanglei(MARGIN_SIDE, team_box->GetPositionY()+team_box->GetSizeY()+ MARGIN_SIDE, 
				       mainBoxWidth, mainBoxHeight));
  map_box->AddWidget(new PictureWidget(Rectanglei(0,0,46,100), "menu/map_label"));

  Box * tmp_map_box = new VBox( Rectanglei(MARGIN_SIDE, team_box->GetPositionY()+team_box->GetSizeY()+ MARGIN_SIDE, 
				       mainBoxWidth-60, mainBoxHeight), false);

  Label * select_world_label = new Label(_("Select the world:"), rectZero, *normal_font);
  tmp_map_box->AddWidget(select_world_label);

  uint map_preview_height = mainBoxHeight - select_world_label->GetSizeY() -2*10;
  
  tmp_box = new HBox( Rectanglei(0, 0, 1, map_preview_height+2*10 ), false);
  tmp_box->SetMargin(10);
  tmp_box->SetBorder( Point2i(0,0) );

  lbox_maps = new ListBox( Rectanglei(0, 0, MAPS_W, map_preview_height ));
  tmp_box->AddWidget(lbox_maps);

  map_preview = new PictureWidget(Rectanglei(0, 0, map_preview_height*4/3, map_preview_height));
  tmp_box->AddWidget(map_preview);

  tmp_map_box->AddWidget(tmp_box);
  map_box->AddWidget(tmp_map_box);
  widgets.AddWidget(map_box);

  /* Choose other game options */
  game_options = new HBox( Rectanglei(MARGIN_SIDE, map_box->GetPositionY()+map_box->GetSizeY()+ MARGIN_SIDE, 
					    mainBoxWidth/2, mainBoxHeight) );  
  game_options->AddWidget(new PictureWidget(Rectanglei(0,0,39,128), "menu/mode_label"));

  opt_duration_turn = new SpinButtonWithPicture(_("Duration of a turn:"), "menu/timing_turn",
						stdRect,
						TPS_TOUR_MIN, 5,
						TPS_TOUR_MIN, TPS_TOUR_MAX);
  game_options->AddWidget(opt_duration_turn);

  opt_duration_end_turn = new SpinButtonWithPicture(_("Duration of the end of a turn:"), "menu/timing_end_of_turn",
						    stdRect,
						    TPS_FIN_TOUR_MIN, 1,
						    TPS_FIN_TOUR_MIN, TPS_FIN_TOUR_MAX);
  game_options->AddWidget(opt_duration_end_turn);

  opt_nb_characters = new SpinButtonBig(_("Number of players per team:"), stdRect,
				     4, 1,
				     NBR_VER_MIN, NBR_VER_MAX);
  game_options->AddWidget(opt_nb_characters);

  opt_energy_ini = new SpinButtonWithPicture(_("Initial energy:"), "menu/energy",
					     stdRect,
					     100, 5,
					     50, 200);

  game_options->AddWidget(opt_energy_ini);
  widgets.AddWidget(game_options);


  // Values initialization

  // Load Maps' list
  std::sort(lst_terrain.liste.begin(), lst_terrain.liste.end(), compareMaps);

  ListeTerrain::iterator
    terrain=lst_terrain.liste.begin(),
    fin_terrain=lst_terrain.liste.end();
  for (; terrain != fin_terrain; ++terrain)
  {
    bool choisi = terrain -> name == lst_terrain.TerrainActif().name;
    lbox_maps->AddItem (choisi, terrain -> name, terrain -> name);
  }

  ChangeMap();

  // Load Teams' list
  teams_list.full_list.sort(compareTeams);

  TeamsList::full_iterator
    it=teams_list.full_list.begin(),
    end=teams_list.full_list.end();

  uint i=0;
  for (; it != end; ++it)
  {
    bool choix = teams_list.IsSelected (i);
    if (choix)
      lbox_selected_teams->AddItem (false, (*it).GetName(), (*it).GetId());
    else
      lbox_all_teams->AddItem (false, (*it).GetName(), (*it).GetId());
    ++i;
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
  if (lbox_maps->Clic(mousePosition, button)) {
    ChangeMap();
  } else if (lbox_all_teams->Clic(mousePosition, button)) {

  } else if (lbox_selected_teams->Clic(mousePosition, button)) {

  } else if ( bt_add_team->Contains(mousePosition)) {
    if (lbox_selected_teams->GetItemsList()->size() < GameMode::GetInstance()->max_teams)
      MoveTeams(lbox_all_teams, lbox_selected_teams, false);
  } else if ( bt_remove_team->Contains(mousePosition)) {
    MoveTeams(lbox_selected_teams, lbox_all_teams, true);
  } else if ( game_options->Clic(mousePosition, button)) {

  }
}

void GameMenu::SaveOptions()
{
  // Save values
  std::string map_id = lbox_maps->ReadLabel();
  lst_terrain.ChangeTerrainNom (map_id);

  // teams
  std::vector<list_box_item_t> *
    selected_teams = lbox_selected_teams->GetItemsList();

  if (selected_teams->size() > 1) {
    std::list<uint> selection;

    std::vector<list_box_item_t>::iterator
      it = selected_teams->begin(),
      end = selected_teams->end();

    int index = -1;
    for (; it != end; ++it) {
      teams_list.FindById(it->value, index);
      if (index > -1)
	selection.push_back(uint(index));
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

void GameMenu::ChangeMap()
{
  std::string map_id = lbox_maps->ReadLabel();
  uint map = lst_terrain.FindMapById(map_id);

  map_preview->SetSurface(lst_terrain.liste[map].preview, true);
}

void GameMenu::MoveTeams(ListBox * from, ListBox * to, bool sort)
{
  if (from->GetSelectedItem() != -1) {
    to->AddItem (false,
		 from->ReadLabel(),
		 from->ReadValue());
    to->Deselect();
    if (sort) to->Sort();

    from->RemoveSelected();
  }
}

void GameMenu::SelectTeamLogo(Team * t)
{
  if (last_team != t) {
    last_team = t;
    team_logo->SetSurface(last_team->flag);
  }
}

void GameMenu::Draw(const Point2i &mousePosition)
{
  int t = lbox_all_teams->MouseIsOnWhichItem(mousePosition);
  if (t != -1) {
    int index = -1;
    Team * new_team = teams_list.FindById(lbox_all_teams->ReadValue(t), index);
    SelectTeamLogo(new_team);
  } else {
    t = lbox_selected_teams->MouseIsOnWhichItem(mousePosition);
    if (t != -1) {
      int index = -1;
      Team * new_team = teams_list.FindById(lbox_selected_teams->ReadValue(t), index);
      SelectTeamLogo(new_team);
    }
  }
}

