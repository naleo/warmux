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
 * Progress bar for GUI.
 *****************************************************************************/

#include "gui/progress_bar.h"
#include <SDL.h>
#include "graphic/video.h"
#include "include/app.h"
#include "map/map.h"
#include "tool/math_tools.h"

ProgressBar::ProgressBar():
  border_color(0, 0, 0, 255),
  value_color(255, 255, 255, 255),
  background_color(100, 100, 100, 255),
  image(),
  coefRed(),
  coefGreen(),
  coefBlue(),
  coefAlpha(),
  divisor(),
  gradientMode(false),
  x(0),
  y(0),
  larg(0),
  haut(0),
  val(0),
  min(0),
  max(0),
  m_use_ref_val(false),
  m_ref_val(0),
  val_barre(0),
  orientation(),
  colorMin(),
  colorMax(),
  marqueur()
{
}

void ProgressBar::SetMinMaxValueColor(const Color & min, 
                                      const Color & max) 
{
  this->gradientMode = true;
  this->colorMin     = min;
  this->colorMax     = max;
}

void ProgressBar::InitPos(uint px, 
                          uint py, 
			  uint plarg, 
			  uint phaut)
{
  ASSERT (3 <= plarg);
  ASSERT (3 <= phaut);
  x    = px;
  y    = py;
  larg = plarg;
  haut = phaut;
  image.NewSurface(Point2i(larg, haut), SDL_SWSURFACE | SDL_SRCALPHA, true);
}

/*
 * intitialize the progress bar
 * orientation is set with ProgressBar::PROG_BAR_VERTICAL or
 *                         ProgressBar::PROG_BAR_HORIZONTAL
 * default orientation is ProgressBar::PROG_BAR_HORIZONTAL
 */
void ProgressBar::InitVal (long pval, 
                           long pmin, 
			   long pmax, 
			   enum orientation porientation)
{
  ASSERT (pmin < pmax);
  val         = pval;
  min         = pmin;
  max         = pmax;
  orientation = porientation;
  val_barre   = ComputeBarValue(val);

  if (gradientMode) {
    coefRed   = (colorMax.GetRed()   - colorMin.GetRed())   / static_cast<float>(max);
    coefGreen = (colorMax.GetGreen() - colorMin.GetGreen()) / static_cast<float>(max);
    coefBlue  = (colorMax.GetBlue()  - colorMin.GetBlue())  / static_cast<float>(max);
    coefAlpha = (colorMax.GetAlpha() - colorMin.GetAlpha()) / static_cast<float>(max);
  }
}

void ProgressBar::UpdateValue(long pval)
{
  val       = ComputeValue(pval);
  val_barre = ComputeBarValue(val);

  if (gradientMode) {
    long absVal = abs(val);
    value_color.SetColor(colorMin.GetRed()   + (coefRed   * absVal),
                         colorMin.GetGreen() + (coefGreen * absVal),
                         colorMin.GetBlue()  + (coefBlue  * absVal),
                         colorMin.GetAlpha() + (coefAlpha * absVal));
  }
		       
}

long ProgressBar::ComputeValue(long pval) const 
{
  return InRange_Long(pval, min, max);
}

uint ProgressBar::ComputeBarValue(long val) const
{
  if (PROG_BAR_HORIZONTAL == orientation) {
    return (ComputeValue(val) -min)*(larg-2)/(max-min);
  } else {
    return (ComputeValue(val) -min)*(haut-2)/(max-min);
  }
}

// TODO pass a Surface as parameter
void ProgressBar::DrawXY(const Point2i & pos) const 
{
  int begin, end;

  // Bordure
  image.Fill(border_color);

  // Fond
  Rectanglei r_back(1, 1, larg - 2, haut - 2);
  image.FillRect(r_back, background_color);

  // Valeur
  if (m_use_ref_val) {
    int ref = ComputeBarValue (m_ref_val);
    if (val < m_ref_val) { // FIXME hum, this seems buggy
      begin = 1+val_barre;
      end = 1+ref;
    } else {
      begin = 1+ref;
      end = 1+val_barre;
    }
  } else {
    begin = 1;
    end = 1+val_barre;
  }

  Rectanglei r_value;
  if(PROG_BAR_HORIZONTAL == orientation)
    r_value = Rectanglei(begin, 1, end - begin, haut - 2);
  else
    r_value = Rectanglei(1, haut - end + begin - 1, larg - 2, end -1 );

  image.FillRect(r_value, value_color);

  if (m_use_ref_val) {
    int ref = ComputeBarValue (m_ref_val);
    Rectanglei r_ref;
    if(PROG_BAR_HORIZONTAL == orientation)
       r_ref = Rectanglei(1 + ref, 1, 1, haut - 2);
    else
       r_ref = Rectanglei(1, 1 + ref, larg - 2, 1);
    image.FillRect(r_ref, border_color);
  }

  // Marqueurs
  marqueur_it_const it = marqueur.begin(), it_end = marqueur.end();
  for (; it != it_end; ++it)
  {
    Rectanglei r_marq;
    if(PROG_BAR_HORIZONTAL == orientation)
      r_marq = Rectanglei(1 + it->val, 1, 1, haut - 2);
    else
      r_marq = Rectanglei(1, 1 + it->val, larg -2, 1);
    image.FillRect( r_marq, it->color);
  }
  Rectanglei dst(pos.x, pos.y, larg, haut);
  GetMainWindow().Blit(image, pos);

  GetWorld().ToRedrawOnScreen(dst);
}

// Ajoute/supprime un marqueur
ProgressBar::marqueur_it ProgressBar::AddTag(long val, 
                                             const Color & color)
{
  marqueur_t m;
  m.val   = ComputeBarValue(val);
  m.color = color;
  marqueur.push_back(m);

  return --marqueur.end();
}

void ProgressBar::SetReferenceValue (bool use, 
                                     long value)
{
  m_use_ref_val = use;
  m_ref_val     = ComputeValue(value);
}
