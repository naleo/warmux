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
 * Particle Engine
 *****************************************************************************/

#ifndef FIRE_H
#define FIRE_H
#include "particles/particle.h"

class FireParticle : public Particle
{
  int creation_time;
  bool on_ground;
  int oscil_delta;
  public:
    FireParticle();
    virtual ~FireParticle();
    void Refresh();
    void Draw();

    void SetVivacity(int i_vivacity) { m_vivacity = i_vivacity; };

  protected:
    void SignalDrowning();
    void SignalOutOfMap();
    void SignalGroundCollision(const Point2d&);
    virtual void SignalObjectCollision(GameObj * obj,PhysicalShape * shape, const Point2d& my_speed_before);

    void Split();

    int m_rebound_count;
    int m_vivacity;
    uint m_living_time;

};

#endif /* FIRE_H */