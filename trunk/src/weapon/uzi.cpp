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
 * Uzi !
 *****************************************************************************/

#include "uzi.h"
#include <sstream>
#include "weapon_tools.h"
#include "../game/game_loop.h"
#include "../game/time.h"
#include "../interface/game_msg.h"
#include "../map/map.h"
#include "../object/objects_list.h"
#include "../team/teams_list.h"
#include "../tool/i18n.h"

const double SOUFFLE_BALLE = 1;
const double MIN_TIME_BETWEEN_SHOOT = 70; // in milliseconds

UziBullet::UziBullet(GameLoop &p_game_loop, ExplosiveWeaponConfig& cfg) :
  WeaponBullet(p_game_loop, "uzi_bullet", cfg)
{ 
}

void UziBullet::ShootSound()
{
  jukebox.Play("share","weapon/uzi");
}

//-----------------------------------------------------------------------------

Uzi::Uzi() :
  WeaponLauncher(WEAPON_UZI,"uzi", new ExplosiveWeaponConfig())
{
  m_name = _("Uzi");
  override_keys = true ;

  m_first_shoot = 0;

  projectile = new UziBullet(game_loop, cfg());
}

void Uzi::RepeatShoot()
{  
  if (m_is_active)
    return;
      
  uint time = global_time.Read() - m_first_shoot; 
  uint tmp = global_time.Read();

  if (time >= MIN_TIME_BETWEEN_SHOOT)
  {
    m_is_active = false;
    NewActionShoot();
    m_first_shoot = tmp;
  }
}

bool Uzi::p_Shoot()
{
  jukebox.Play("share", "weapon/uzi");

  // Calculate movement of the bullet
  // Set the initial position.
  Point2i pos = ActiveCharacter().GetHandPosition();

  // Equation of movement : y = ax + b
  double angle, a, b;
  angle = ActiveTeam().crosshair.GetAngleRad();
  a = sin(angle)/cos(angle);
  b = pos.y - ( a * pos.x ) ;

  // Move the bullet !!
  //balle.PrepareTir();  
  projectile->SetXY( pos );
  projectile->Ready();
  
  projectile->is_active = true;

  while( projectile->is_active ){
    pos.y = (int)((a*pos.x) + b);

    projectile->SetXY( pos );
    // the bullet in gone outside the map
    if (projectile->IsGhost()) {
      projectile->is_active=false;
      return true;
    }
    
    // is there a collision ??
    if(projectile->CollisionTest( 0, 0 ) ){
      projectile->is_active=false;

      // Si la balle a touch� un ver, lui inflige des d�gats
      Character* ver = projectile->dernier_ver_touche;
      PhysicalObj* obj = projectile->dernier_obj_touche;
	  
      if( ver ){
          obj = ver;
       	  ver -> SetEnergyDelta (-cfg().damage);
	  }
      if( obj ) 
          obj -> AddSpeed (SOUFFLE_BALLE, angle);
      // Creuse le world
      if( !obj )
		world.Creuse(projectile->GetPosition() - projectile->impact.GetSize()/2,
		      projectile->impact);
      return true;
    }
    pos.x += ActiveCharacter().GetDirection();
  }

  return true;
}

void Uzi::HandleKeyEvent(int action, int event_type)
{
  switch (action) {    

  case ACTION_SHOOT:
    if (event_type == KEY_REFRESH)
      RepeatShoot();
    if (event_type == KEY_RELEASED)
      m_is_active = false;

    break;
  default:
    break;
  };
}

