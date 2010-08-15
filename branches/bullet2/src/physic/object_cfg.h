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
 * Class for object physics
 *****************************************************************************/

#ifndef OBJECT_CFG_H
#define OBJECT_CFG_H
//-----------------------------------------------------------------------------
#include <string>

#include "physical_engine.h"
//-----------------------------------------------------------------------------

class ObjectConfig
{
public:
  // Wind effect factor on the object. 0 means not affected.
  double m_wind_factor;

  // Air resistance factor. 1 = normal air resistance.
  double m_air_resist_factor ;

  // Define how the object is affected by gravity.
  double m_gravity_factor ;

  // Object rebound factor when the object collide with the ground.
  // 0.0f for no rebound
  double m_rebound_factor;

  // Object supports rotation ?
  bool m_rotating;

  // Object try to align with movement direction
  double m_auto_align_force;

  // Position of mass and rotation center
  Point2d  m_center_position;


  PhysicalEngine::ObjectType m_type;

  ObjectConfig();
  virtual ~ObjectConfig() { };

  void LoadXml(const std::string& obj_name, const std::string &config_file);
};

#endif // OBJECT_CFG_H