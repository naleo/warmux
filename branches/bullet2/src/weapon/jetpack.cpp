/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2009 Wormux Team.
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
 * Jet Pack :-)
 *****************************************************************************/

#include "weapon/jetpack.h"
#include "weapon/explosion.h"
#include "character/character.h"
#include "game/game.h"
#include "game/game_mode.h"
#include "game/time.h"
#include "include/action_handler.h"
#include "interface/cursor.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "physic/physical_obj.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "team/team.h"

const double JETPACK_FORCE = 1800.0;

const uint DELTA_FUEL_DOWN = 100 ;  // Delta time between 2 fuel unit consumption.

JetPack::JetPack() : Weapon(WEAPON_JETPACK, "jetpack",
                            new WeaponConfig(),
                            NEVER_VISIBLE)
{
  UpdateTranslationStrings();

  m_category = MOVE;
  m_unit_visibility = VISIBLE_ONLY_WHEN_ACTIVE;

  use_unit_on_first_shoot = false;

  m_force_index = NULL;

  move_up = false;
  m_flying = false;
}

void JetPack::UpdateTranslationStrings()
{
  m_name = _("Jetpack");
  /* TODO: FILL IT */
  /* m_help = _(""); */
}


bool JetPack::IsInAir()
{
  return !ActiveCharacter().HasGroundUnderFeets() || move_up;
}

void JetPack::Refresh()
{
  if (IsInUse())
  {
    Point2d F(0.0, 0.0);
    if (move_up) {
      F.y = -(ActiveCharacter().GetPhysic()->GetMass() * GameMode::GetInstance()->gravity + JETPACK_FORCE);
    }
    const WalkIntention & walk_intention = ActiveCharacter().GetWalkIntention();
    if (walk_intention.IsToWalk() && IsInAir()) {
      LRDirection direction = walk_intention.GetDirection();
      if (direction == DIRECTION_LEFT)
        F.x = -JETPACK_FORCE;
      else
        F.x = JETPACK_FORCE;
      ActiveCharacter().SetDirection(direction);
    }

    if (F.IsNull() && m_flying)
      StopFlying();
    else if (!F.IsNull() && !m_flying)
      StartFlying();

    ActiveCharacter().GetPhysic()->RemoveExternForce(m_force_index);
    m_force_index = ActiveCharacter().GetPhysic()->AddExternForceXY(F);

    if (!F.IsNull())
    {
      // We are using fuel !!!
      uint current = Time::GetInstance()->Read() ;
      double delta = (double)(current - m_last_fuel_down);

      while (delta >= DELTA_FUEL_DOWN)
      {
        if (EnoughAmmoUnit())
        {
          UseAmmoUnit();
          m_last_fuel_down += DELTA_FUEL_DOWN ;
          delta -= DELTA_FUEL_DOWN ;
        }
        else
        {
          p_Deselect();
          break;
        }
      }
    }
  }
}

void JetPack::p_Select()
{
  ActiveCharacter().SetClothe("jetpack");

  m_unit_visibility = VISIBLE_ONLY_WHEN_ACTIVE;
}

void JetPack::p_Deselect()
{
  move_up = false;
  ActiveCharacter().GetPhysic()->RemoveExternForce(m_force_index);
  StopFlying();
  ActiveCharacter().SetClothe("normal");
  ActiveCharacter().SetMovement("breathe");
}

void JetPack::StartFlying()
{
  if (m_flying)
    return;

  ActiveCharacter().SetMovement("jetpack-fire");

  m_last_fuel_down = Time::GetInstance()->Read();
  flying_sound.Play(ActiveTeam().GetSoundProfile(),"weapon/jetpack", -1);

  Camera::GetInstance()->FollowObject(&ActiveCharacter(), true);


  // this avoids to show the arrow on top of character that can hide the ammo units
  ActiveCharacter().UpdateLastMovingTime();

  // do not display the character on top of the active character
  // else it will hide the ammo unit of the jetpack (bug #11479)
  CharacterCursor::GetInstance()->Hide();

  m_flying = true;
}

void JetPack::StopFlying()
{
  if (!m_flying)
    return;

  ActiveCharacter().SetMovement("jetpack-nofire");
  flying_sound.Stop();
  m_flying = false;
}

void JetPack::StartMovingUp()
{
  move_up = true;
}

void JetPack::StopMovingUp()
{
  move_up = false;
}

bool JetPack::IsPreventingLRMovement()
{
  return IsInAir();
}

bool JetPack::IsPreventingJumps()
{
  return IsInAir();
}

bool JetPack::IsPreventingWeaponAngleChanges()
{
  return true;
}

void JetPack::HandleKeyPressed_Up(bool /*slowly*/)
{
  if (IsInUse())
    StartMovingUpForAllPlayers();
}

void JetPack::HandleKeyReleased_Up(bool /*slowly*/)
{
  if (IsInUse())
    StopMovingUpForAllPlayers();
}

void JetPack::StartShooting()
{
  if (IsInUse()) {
    p_Deselect();
    ActiveTeam().AccessNbUnits() = 0;
  } else {
    Weapon::StartShooting();
  }
}

bool JetPack::p_Shoot()
{
  ActiveCharacter().SetClothe("jetpack-fire");

  return true;
}

std::string JetPack::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u jetpack! Groovy!",
            "%s team has won %u jetpacks! Groovy!",
            items_count), TeamName, items_count);
}
