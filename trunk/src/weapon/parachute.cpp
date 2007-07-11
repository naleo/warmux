/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
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
 * Parachute !
 *****************************************************************************/

#include "parachute.h"
#include "explosion.h"
#include "character/character.h"
#include "game/game.h"
#include "game/game_mode.h"
#include "game/game_loop.h"
#include "graphic/sprite.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "object/physical_obj.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "team/team.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"

Parachute::Parachute() : Weapon(WEAPON_PARACHUTE, "parachute", new ParachuteConfig(), NEVER_VISIBLE)
{
  m_name = _("Parachute");
  m_category = MOVE;
  m_initial_nb_ammo = 2;
  m_x_extern = 0.0;
  use_unit_on_first_shoot = false;

  image = resource_manager.LoadSprite(weapons_res_profile, "parachute_sprite");
}

void Parachute::p_Select()
{
  open = false;
  closing = false;
  image->animation.SetShowOnFinish(SpriteAnimation::show_last_frame);
}

void Parachute::p_Deselect()
{
  ActiveCharacter().ResetConstants();
}

bool Parachute::IsInUse() const
{
  return GameLoop::GetInstance()->GetRemainingTime() > 0 &&
         GameLoop::GetInstance()->ReadState() == GameLoop::PLAYING;
}

bool Parachute::p_Shoot()
{
  GameMessages::GetInstance()->Add(_("Parachute is activated automatically."));
  return false;
}

void Parachute::Draw()
{
  if(open) {
    image->Update();
    image->Draw(ActiveCharacter().GetHandPosition() - Point2i(image->GetWidth()/2,image->GetHeight()));
  }
}

void Parachute::Refresh()
{
  double speed;
  double angle;

  ActiveCharacter().GetSpeed(speed, angle);

  if(ActiveCharacter().FootsInVacuum() && speed != 0.0) { // We are falling
    if(!open && (speed > GameMode::GetInstance()->safe_fall)) { // with a sufficient speed
      if(EnoughAmmo()) { // We have enough ammo => start opening the parachute
        UseAmmo();
        ActiveCharacter().SetAirResistFactor(cfg().air_resist_factor);
        ActiveCharacter().SetWindFactor(cfg().wind_factor);
        open = true;
        image->animation.SetPlayBackward(false);
        image->Start();
        ActiveCharacter().SetSpeedXY(Point2d(0,0));
        ActiveCharacter().SetMovement("parachute");
        camera.SetCloseFollowing(true);
        camera.FollowObject(&ActiveCharacter(), true, true, true);
      }
    }
  } else { // We are on the ground
    if(open) { // The parachute is opened
      if (!closing) { // We have just hit the ground. Start closing animation
        image->animation.SetPlayBackward(true);
        image->animation.SetShowOnFinish(SpriteAnimation::show_blank);
        image->Start();
        closing = true;
        return;
      } else { // The parachute is closing
        if(image->IsFinished()) {
          // The animation is finished... We are done with the parachute
          open = false;
          closing = false;
          UseAmmoUnit();
        }
      }
    }
  }
  // If parachute is open => character can move a little to the left or to the right
  if(open)
    ActiveCharacter().SetExternForce(m_x_extern, 0.0);
}

void Parachute::SignalTurnEnd()
{
  p_Deselect();
}

std::string Parachute::GetWeaponWinString(const char *TeamName, uint items_count )
{
  return Format(ngettext(
            "%s team has won %u parachute!",
            "%s team has won %u parachutes!",
            items_count), TeamName, items_count);
}

void Parachute::HandleKeyPressed_Shoot()
{
  if(open) {
    image->Finish();
    open = false;
    closing = false;
    UseAmmoUnit();
  } else {
    Weapon::HandleKeyPressed_Shoot();
  }
}

void Parachute::HandleKeyPressed_MoveRight()
{
  if(open) {
    ActiveCharacter().SetDirection(Body::DIRECTION_RIGHT);
    m_x_extern = cfg().force_side_displacement;
  } else {
    Weapon::HandleKeyPressed_MoveRight();
  }
}

void Parachute::HandleKeyReleased_MoveRight()
{
  if(open)
    m_x_extern = 0.0;
  else
    Weapon::HandleKeyReleased_MoveRight();
}

void Parachute::HandleKeyPressed_MoveLeft()
{
  if(open) {
    ActiveCharacter().SetDirection(Body::DIRECTION_LEFT);
    m_x_extern = -cfg().force_side_displacement;
  } else {
    Weapon::HandleKeyPressed_MoveLeft();
  }
}

void Parachute::HandleKeyReleased_MoveLeft()
{
  if(open)
    m_x_extern = 0.0;
  else
    Weapon::HandleKeyReleased_MoveLeft();
}

ParachuteConfig& Parachute::cfg() {
  return static_cast<ParachuteConfig&>(*extra_params);
}

ParachuteConfig::ParachuteConfig(){
  wind_factor = 10.0;
  air_resist_factor = 140.0;
  force_side_displacement = 2000.0;
}

void ParachuteConfig::LoadXml(xmlpp::Element *elem){
  WeaponConfig::LoadXml(elem);
  XmlReader::ReadDouble(elem, "wind_factor", wind_factor);
  XmlReader::ReadDouble(elem, "air_resist_factor", air_resist_factor);
  XmlReader::ReadDouble(elem, "force_side_displacement", force_side_displacement);
}
