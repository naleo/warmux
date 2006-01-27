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
 * Bonus Box
 *****************************************************************************/

#ifndef BONUS_BOX_H
#define BONUS_BOX_H
//-----------------------------------------------------------------------------
#include <SDL.h>
#include "../include/base.h"
#include "../object/physical_obj.h"
#include "../team/team.h"
//-----------------------------------------------------------------------------

class BonusBox : public PhysicalObj
{
private:
  bool enable;
  bool still_visible;
  bool parachute; 

  Sprite *anim;
  uint time;

  enum
  {
    // Si vous touchez � cet enum, modifiez aussi nbr_bonus_diff
    bonusDYNAMITE=1,
    bonusTELEPORTATION,
    bonusENERGY,
    bonusTRAP,
    bonusAIR_ATTACK,
    bonusAUTO_BAZOOKA
  } bonus_weapons;
  static const uint nb_bonus = bonusAUTO_BAZOOKA;

public:
  // Initialise les donn�es
  BonusBox();
  void Init();
  void FreeMem();
  void Reset();

  // Active les caisses ?
  void Active (bool actif);

  // Signale la fin d'une chute
  virtual void SignalFallEnding();  

  // Signale un changement d'etat
  virtual void SignalGhostState (bool etait_mort);

  void Draw();
  void Refresh();
  void NewBonusBox();
 private:
  void ApplyBonus (Team &team, Character &character);

};

extern BonusBox bonus_box;
//-----------------------------------------------------------------------------
#endif
