/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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
 * Picture widget: A widget containing a picture
 *****************************************************************************/

#include "gui/picture_widget.h"
#include "graphic/colors.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "include/app.h"

PictureWidget::PictureWidget (const Point2i & _size)
  : disabled(false)
  , spr(NULL)
{
  size = _size;
}

PictureWidget::PictureWidget(const Point2i & _size,
                             const std::string & resource_id,
                             bool scale) :
  disabled(false),
  spr(NULL)
{
  size = _size;

  Profile *res = GetResourceManager().LoadXMLProfile( "graphism.xml", false);
  SetSurface(GetResourceManager().LoadImage(res, resource_id), scale, scale);
  GetResourceManager().UnLoadXMLProfile( res);
}

PictureWidget::PictureWidget(Profile * profile,
                             const xmlNode * pictureNode) :
  Widget(profile, pictureNode),
  disabled(false),
  spr(NULL)
{
}

PictureWidget::~PictureWidget()
{
  if (spr != NULL) {
    delete spr;
  }
}

/*
  Picture node example :
  <Picture file="menu/image.png"
           alpha="false"
           x="0" y="0"
           width="100" height="100"
           scale="true"
           antialiasing="true" />
*/
bool PictureWidget::LoadXMLConfiguration()
{
  if (NULL == profile || NULL == widgetNode) {
    //TODO error ... xml attributs not initialized !
    return false;
  }
  XmlReader * xmlFile = profile->GetXMLDocument();

  std::string file("menu/pic_not_found.png");
  xmlFile->ReadStringAttr(widgetNode, "file", file);

  bool activeAlpha = false;
  xmlFile->ReadBoolAttr(widgetNode, "alpha", activeAlpha);

  file = profile->relative_path + file;
  Surface surface(file.c_str());

  if (!activeAlpha) {
    surface = surface.DisplayFormat();
  } else {
    surface = surface.DisplayFormatAlpha();
  }

  ParseXMLGeometry();
  ParseXMLMisc();

  bool activeScale = false;
  xmlFile->ReadBoolAttr(widgetNode, "scale", activeScale);
  bool activeAntialiasing = false;
  xmlFile->ReadBoolAttr(widgetNode, "antialiasing", activeAntialiasing);

  SetSurface(surface, activeScale, activeAntialiasing);
  return true;
}

void PictureWidget::SetSurface(const Surface & s,
                               bool enable_scaling,
                               bool antialiasing)
{
  NeedRedrawing();

  if (NULL != spr) {
    delete spr;
  }

  spr = new Sprite(s, antialiasing);
  if (enable_scaling) {
    Double scale = std::min( Double(GetSizeY())/spr->GetHeight(),
                             Double(GetSizeX())/spr->GetWidth() );

    spr->Scale(scale, scale);
  }
}

void PictureWidget::SetNoSurface()
{
  NeedRedrawing();

  if (NULL != spr) {
    delete spr;
  }
  spr = NULL;
}

void PictureWidget::Draw(const Point2i &/*mousePosition*/) const
{
  if (NULL == spr) {
    return;
  }

  Surface & surf = GetMainWindow();
  Point2i pos = GetPicturePosition();

  spr->Blit(surf, pos);

  // Draw a transparency mask
  if (disabled) {
    surf.BoxColor(Rectanglei(pos, spr->GetSize()), defaultOptionColorBox);
  }
}

Point2i PictureWidget::GetPicturePosition() const
{
  return position + (size - spr->GetSize()) / 2;
}

Point2f PictureWidget::GetScale() const
{
  return Point2f(spr->GetScaleX().tofloat(), spr->GetScaleY().tofloat());
}
