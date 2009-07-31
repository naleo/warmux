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
 * PhysicTile
 *****************************************************************************/

#ifndef PHYSIC_TILE_H
#define PHYSIC_TILE_H

#include <WORMUX_point.h>
class TileItem;
class PhysicalShape;
class PhysicalGround;

#ifdef DEBUG
class Color;
#endif

class PhysicTile
{
private:

  typedef enum {EMPTY,FULL,MIXTE} Fullness;

  Fullness m_fullness;

  bool m_is_subdivided;
  bool m_is_containing_polygon;

  PhysicalShape *m_shape;
  PhysicTile *m_physic_tiles[4];

  PhysicTile * m_parent_physic_tile;
  TileItem * m_parent_tile;

  Point2i m_size;
  Point2i m_offset;
  Point2i m_tile_offset;
  PhysicalGround* m_physical_ground;

  int m_level;

  Fullness IsFull() const;
  bool GeneratePolygone();
  void Generate();
  void GenerateEmpty();
  void GenerateFull();
  void GenerateMixte();
  void Clean();
  void InitShape();

public:

  PhysicTile(Point2i size, Point2i offset , Point2i tile_offset, TileItem *tile, PhysicTile *parent_physic_tile, int level);
  ~PhysicTile();
#ifdef DEBUG
  void DrawBorder(const Color& color) const;
#endif
};

#endif
