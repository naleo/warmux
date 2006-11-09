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
 * auto bazooka : launch a homing missile
 *****************************************************************************/

#include "auto_bazooka.h"
#include "explosion.h"
#include "../game/time.h"
#include "../graphic/video.h"
#include "../include/app.h"
#include "../interface/game_msg.h"
#include "../interface/mouse.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../map/wind.h"
#include "../team/teams_list.h"
#include "../tool/math_tools.h"
#include "../tool/i18n.h"
#include "../object/objects_list.h"
#ifdef __MINGW32__
#undef LoadImage
#endif


class AutomaticBazookaConfig : public ExplosiveWeaponConfig {
  public:
    uint seek_time;
    double uncontrolled_turn_speed;
    double fuel_time;
    double rocket_force;
    AutomaticBazookaConfig();
    void LoadXml(xmlpp::Element *elem);
};

RPG::RPG(AutomaticBazookaConfig& cfg,
                                         WeaponLauncher * p_launcher) :
  WeaponProjectile("rocket", cfg, p_launcher), smoke_engine(20), m_lastrefresh(0)
{
  m_targeted = false;
  explode_colliding_character = true;
}

void RPG::Shoot (double strength) 
{
  WeaponProjectile::Shoot(strength);
  angle_local=ActiveTeam().crosshair.GetAngleRad();
}

void RPG::Refresh()
{
  AutomaticBazookaConfig &acfg = dynamic_cast<AutomaticBazookaConfig &>(cfg);
  uint time = Time::GetInstance()->Read();
  float flying_time = time - begin_time;
  uint timestep = time - m_lastrefresh;
  m_lastrefresh = time;
  if (!m_targeted)
  {
    // rocket is turning around herself
    angle_local += acfg.uncontrolled_turn_speed * timestep / 1000.;
    if(angle_local > M_PI) angle_local = - M_PI;

    // TPS_AV_ATTIRANCE msec later being launched, the rocket is homing to the target
    if(flying_time>1000 * acfg.seek_time)
    {
      m_targeted = true;
      SetSpeed(0,0);
      angle_local = GetPosition().ComputeAngle( m_target );
      m_force = acfg.rocket_force;
      SetExternForce(m_force, angle_local);
    }
  }
  else
  {
    SetExternForce(m_force, angle_local+M_PI_2); // reverse the force applyed on the last Refresh()

    if(flying_time - acfg.seek_time < acfg.fuel_time*1000.) {
      smoke_engine.AddPeriodic(Point2i(GetX() + GetWidth() / 2,
                                       GetY() + GetHeight()/ 2), particle_DARK_SMOKE, false, -1, 2.0);
      angle_local = GetPosition().ComputeAngle( m_target );
      m_force = acfg.rocket_force * ((acfg.fuel_time*1300. - flying_time + acfg.seek_time)/acfg.fuel_time/1300.);
    } else {
      m_force = 0; //if there's no fuel left just let it crash into the ground somewhere
      angle_local += acfg.uncontrolled_turn_speed * timestep / 1000.;
      if(angle_local > M_PI) angle_local = - M_PI;
    }

    SetExternForce(m_force, angle_local);

  }
  image->SetRotation_deg(angle_local *180/M_PI);
}

void RPG::SignalOutOfMap()
{ 
  GameMessages::GetInstance()->Add (_("The automatic rocket has left the battlefield..."));
  WeaponProjectile::SignalOutOfMap();
}

// Set the coordinate of the target
void RPG::SetTarget (int x, int y)
{
  m_target.x = x;
  m_target.y = y;
}

//-----------------------------------------------------------------------------

AutomaticBazooka::AutomaticBazooka() : 
  WeaponLauncher(WEAPON_AUTOMATIC_BAZOOKA, "automatic_bazooka",new AutomaticBazookaConfig() )
{  
  m_name = _("Automatic Bazooka");
  mouse_character_selection = false;
  cible.choisie = false;
  cible.image = resource_manager.LoadImage( weapons_res_profile, "baz_cible");
  ReloadLauncher();
}

WeaponProjectile * AutomaticBazooka::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
      (new RPG(cfg(),dynamic_cast<WeaponLauncher *>(this)));
}

void AutomaticBazooka::Draw()
{
  Weapon::Draw();
  DrawTarget();
}

void AutomaticBazooka::Refresh()
{
  DrawTarget();
  WeaponLauncher::Refresh();
}

void AutomaticBazooka::p_Select()
{
  cible.choisie = false;
  
  Mouse::GetInstance()->SetPointer(POINTER_AIM);
}

void AutomaticBazooka::p_Deselect()
{
  if (cible.choisie) {
    // need to clear the old target
    world.ToRedrawOnMap(Rectanglei(cible.pos.x-cible.image.GetWidth()/2,
                        cible.pos.y-cible.image.GetHeight()/2,
                        cible.image.GetWidth(),
                        cible.image.GetHeight()));
  }

  Mouse::GetInstance()->SetPointer(POINTER_SELECT);
}

void AutomaticBazooka::ChooseTarget(Point2i mouse_pos)
{
  if (cible.choisie) {
    // need to clear the old target
    world.ToRedrawOnMap(Rectanglei(cible.pos.x-cible.image.GetWidth()/2,
                        cible.pos.y-cible.image.GetHeight()/2,
                        cible.image.GetWidth(),
                        cible.image.GetHeight()));
  }

  cible.pos = mouse_pos;
  cible.choisie = true;

  if(!ActiveTeam().IsLocal())
    camera.SetXYabs(mouse_pos - camera.GetSize()/2);
  DrawTarget();
  static_cast<RPG *>(projectile)->SetTarget(cible.pos.x, cible.pos.y);
}

void AutomaticBazooka::DrawTarget()
{
  if( !cible.choisie ) return;

  AppWormux::GetInstance()->video.window.Blit(cible.image, cible.pos - cible.image.GetSize()/2 - camera.GetPosition());
}

bool AutomaticBazooka::IsReady() const
{
  return (EnoughAmmo() && cible.choisie);  
}

AutomaticBazookaConfig &AutomaticBazooka::cfg() {
    return static_cast<AutomaticBazookaConfig &>(*extra_params);
}

AutomaticBazookaConfig::AutomaticBazookaConfig() {
    seek_time = 1;
    uncontrolled_turn_speed = M_PI*8;
    fuel_time = 10;
    rocket_force = 2500;
}

void AutomaticBazookaConfig::LoadXml(xmlpp::Element *elem) {
    ExplosiveWeaponConfig::LoadXml(elem);
    LitDocXml::LitUint (elem, "seek_time", seek_time);
    LitDocXml::LitDouble (elem, "uncontrolled_turn_speed", uncontrolled_turn_speed);
    LitDocXml::LitDouble (elem, "fuel_time", fuel_time);
    LitDocXml::LitDouble (elem, "rocket_force", rocket_force);
}
