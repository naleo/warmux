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
 * Options menu
 *****************************************************************************/

#include "options_menu.h"

#include "../include/app.h"
#include "../game/game_mode.h"
#include "../game/config.h"
#include "../graphic/video.h"
#include "../graphic/font.h"
#include "../map/maps_list.h"
#include "../team/teams_list.h"
#include "../tool/i18n.h"
#include "../tool/string_tools.h"
#include <sstream>

const uint SOUND_X = 30;
const uint SOUND_Y = 30;
const uint SOUND_W = 230;
const uint SOUND_H = 170;

const uint GRAPHIC_X = 30;
const uint GRAPHIC_Y = SOUND_X + SOUND_H + 30;
const uint GRAPHIC_W = 230;
const uint GRAPHIC_H = 240;

OptionMenu::OptionMenu() :
  Menu("menu/bg_option")
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  Rectanglei zeroRect (0, 0, 0, 0);

  normal_font = Font::GetInstance(Font::FONT_NORMAL);

  /* Grapic options */
  Box * graphic_options = new HBox( Rectanglei(GRAPHIC_X, GRAPHIC_Y, GRAPHIC_W, GRAPHIC_H));

  Surface tmp_surf = resource_manager.LoadImage(res, "menu/video_label");
  PictureWidget * pic_widget = new PictureWidget(Rectanglei(0,0,tmp_surf.GetWidth(),tmp_surf.GetHeight()));
  pic_widget->SetSurface(tmp_surf, false);
  graphic_options->AddWidget(pic_widget);

  Box * tmp_graphic_options = new VBox( Rectanglei(GRAPHIC_X, GRAPHIC_Y, GRAPHIC_W, GRAPHIC_H), false);
  tmp_graphic_options->AddWidget(new Label(_("Graphic options"), zeroRect, *normal_font));

  lbox_video_mode = new ListBox( Rectanglei(0, 0, 0, 80) );
  tmp_graphic_options->AddWidget(lbox_video_mode);

  opt_max_fps = new SpinButton(_("Maximum number of FPS:"), zeroRect,
			       50, 5,
			       20, 120);
  tmp_graphic_options->AddWidget(opt_max_fps);

  full_screen = new CheckBox(_("Fullscreen?"), zeroRect);
  tmp_graphic_options->AddWidget(full_screen);

  opt_display_wind_particles = new CheckBox(_("Display wind particles?"), zeroRect);
  tmp_graphic_options->AddWidget(opt_display_wind_particles);

  opt_display_energy = new CheckBox(_("Display player energy?"), zeroRect);
  tmp_graphic_options->AddWidget(opt_display_energy);

  opt_display_name = new CheckBox(_("Display player's name?"), zeroRect);
  tmp_graphic_options->AddWidget(opt_display_name);

  graphic_options->AddWidget(tmp_graphic_options);
  widgets.AddWidget(graphic_options);

  /* Sound options */  
  Box * sound_options = new HBox( Rectanglei(SOUND_X, SOUND_Y, SOUND_W, SOUND_H));
  Surface tmp_sound_surf = resource_manager.LoadImage(res, "menu/audio_label");
  pic_widget = new PictureWidget(Rectanglei(0,0,tmp_surf.GetWidth(),tmp_surf.GetHeight()));
  pic_widget->SetSurface(tmp_sound_surf, false);
  sound_options->AddWidget(pic_widget);

  Box * tmp_sound_options = new VBox( Rectanglei(SOUND_X, SOUND_Y, SOUND_W, SOUND_H),false);
  tmp_sound_options->AddWidget(new Label(_("Sound options"), zeroRect, *normal_font));

  lbox_sound_freq = new ListBox( Rectanglei(0, 0, 0, 80) );
  tmp_sound_options->AddWidget(lbox_sound_freq);

  opt_music = new CheckBox(_("Music?"), zeroRect);
  tmp_sound_options->AddWidget(opt_music);

  opt_sound_effects = new CheckBox(_("Sound effects?"), zeroRect);
  tmp_sound_options->AddWidget(opt_sound_effects);

  sound_options->AddWidget(tmp_sound_options);
  widgets.AddWidget(sound_options);

  // Values initialization

  // Get available video resolution
  AppWormux * app = AppWormux::GetInstance();
  std::list<Point2i>& video_res = AppWormux::GetInstance()->video.GetAvailableConfigs();

  std::list<Point2i>::iterator it = video_res.begin(), end = video_res.end();
  for (; it != end ; ++it) {
    std::ostringstream ss;
    ss << (*it).x << "x" << (*it).y ;
    if ((*it).x == app->video.window.GetWidth() && (*it).y == app->video.window.GetHeight())
      lbox_video_mode->AddItem(true, ss.str(), ss.str());
    else
      lbox_video_mode->AddItem(false, ss.str(), ss.str());
  }

  // Generate sound mode list
  uint current_freq = jukebox.GetFrequency();
  lbox_sound_freq->AddItem (current_freq == 11025, "11 kHz", "11025");
  lbox_sound_freq->AddItem (current_freq == 22050, "22 kHz", "22050");
  lbox_sound_freq->AddItem (current_freq == 44100, "44 kHz", "44100");

  resource_manager.UnLoadXMLProfile( res);

  Config * config = Config::GetInstance();

  opt_max_fps->SetValue (app->video.GetMaxFps());
  opt_display_wind_particles->SetValue (config->GetDisplayWindParticles());
  opt_display_energy->SetValue (config->GetDisplayEnergyCharacter());
  opt_display_name->SetValue (config->GetDisplayNameCharacter());
  full_screen->SetValue (app->video.IsFullScreen());


  opt_music->SetValue( jukebox.UseMusic() );
  opt_sound_effects->SetValue( jukebox.UseEffects() );
}

OptionMenu::~OptionMenu()
{
}

void OptionMenu::OnClic(const Point2i &mousePosition, int button)
{
  widgets.Clic(mousePosition, button);
}

void OptionMenu::SaveOptions()
{
  // Save values
  Config * config = Config::GetInstance();
  config->SetDisplayWindParticles(opt_display_wind_particles->GetValue());
  config->SetDisplayEnergyCharacter(opt_display_energy->GetValue());
  config->SetDisplayNameCharacter(opt_display_name->GetValue());

  AppWormux * app = AppWormux::GetInstance();
  app->video.SetMaxFps(opt_max_fps->GetValue());
  // Video mode
  std::string s_mode = lbox_video_mode->ReadValue();
  int w, h;
  sscanf(s_mode.c_str(),"%dx%d", &w, &h);
  app->video.SetConfig(w, h, full_screen->GetValue());

  uint x = app->video.window.GetWidth() / 2;
  uint y = app->video.window.GetHeight() - 50;

  SetActionButtonsXY(x, y);

  // Sound
  jukebox.ActiveMusic( opt_music->GetValue() );
  jukebox.ActiveEffects( opt_sound_effects->GetValue() );
  std::string sfreq = lbox_sound_freq->ReadValue();
  long freq;
  if (str2long(sfreq,freq)) jukebox.SetFrequency (freq);

  jukebox.Init(); // commit modification on sound options

  //Save options in XML
  config->Save();
}

void OptionMenu::__sig_ok()
{
  SaveOptions();
}

void OptionMenu::__sig_cancel()
{
  // Nothing to do
}

void OptionMenu::Draw(const Point2i &mousePosition)
{
}

