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
 * Interface showing various informations about the game.
 *****************************************************************************/

#ifndef WEAPON_MENU_H
#define WEAPON_MENU_H

#include "../include/base.h"
#include "../character/character.h"
#include "../team/team.h"
#include "../weapon/weapon.h"
#include "../graphic/sprite.h"
#include "../graphic/polygon.h"
#include <vector>

class WeaponMenuItem : public PolygonItem {
 public:
  Weapon* weapon;
  int zoom_start_time;
  bool zoom;

 public:
  WeaponMenuItem(Weapon * weapon, const Point2d & position);
  bool IsMouseOver();
  void SetZoom(bool value);
  void Draw(Surface * dest);
  Weapon * GetWeapon() const;
};

class WeaponsMenu
{
 public:
  static const int MAX_NUMBER_OF_WEAPON;

 private:
  Polygon * weapons_menu;
  Polygon * tools_menu;
  WeaponMenuItem * current_overfly_item;
  AffineTransform2D position;
  AffineTransform2D shear;
  AffineTransform2D rotation;
  AffineTransform2D zoom;
  Sprite * infinite;
  bool show;
  uint motion_start_time;

  int nbr_weapon_type; // number of weapon type = number of rows
  int * nb_weapon_type;
  uint max_weapon;  // max number of weapon in a weapon type = number of lines

 public:
  WeaponsMenu();
  void AddWeapon(Weapon* new_item);
  void Draw();
  void SwitchDisplay();
  AffineTransform2D ComputeWeaponTransformation();
  AffineTransform2D ComputeToolTransformation();
  void Show();
  void Hide();
  void Reset();
  bool IsDisplayed() const;
  bool ActionClic(const Point2i &mouse_pos);
  Sprite * GetInfiniteSymbol() const;
  Weapon * UpdateCurrentOverflyItem(Polygon * poly);
};

#endif
