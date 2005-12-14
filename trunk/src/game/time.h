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
 * Refresh du temps qui passe. Le temps du jeu peut �tre mise en pause.
 *****************************************************************************/

#ifndef TEMPS_H
#define TEMPS_H
//-----------------------------------------------------------------------------
#include "../include/base.h"
#include <string>
//-----------------------------------------------------------------------------

namespace Wormux
{

class Time
{
private:
  uint debut_pause; // D�but de la pause
  uint dt_pause;    // D�calage entre get_time() et LitTemps()
  bool mode_pause;  // Le jeu est en pause ?
   
public:
  Time();
  void Reset();

  // On est en pause ?
  bool IsInPause() const { return mode_pause; }

  // Lit le temps, horloge du jeu
  // Elle diff�re l�g�rement de CL_System::get_time() car le jeu
  // peut se mettre en pause
  uint Read() const;

  // Lit l'horloge du jeu (formatee)
  uint Clock_Sec();
  uint Clock_Min();
  std::string GetString();

  // Passe/reprend
  void Pause();
  void Continue();
};

 extern Time global_time;
}
//-----------------------------------------------------------------------------
#endif
