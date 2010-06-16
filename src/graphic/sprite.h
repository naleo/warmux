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
 * Sprite:     Simple sprite management
 ******************************************************************************
 * 2005/09/21: Jean-Christophe Duberga (jcduberga@gmx.de)
 *             Initial version
 *****************************************************************************/

#ifndef _SPRITE_H
#define _SPRITE_H

#include <SDL.h>
#include <vector>
#include <WORMUX_base.h>
#include <WORMUX_debug.h>
#include "spritecache.h"
#include "spriteanimation.h"

// Forward declarations
class SpriteFrame;

typedef enum {
  top_left,
  top_center,
  top_right,
  left_center,
  center,
  right_center,
  bottom_left,
  bottom_center,
  bottom_right,
  user_defined // Sprite::rot_hotspot is set to this value, when the hotspot is set as a Point2i
} Rotation_HotSpot;

class Sprite
{
private:
  bool smooth;

public:
  SpriteCache cache;
  SpriteAnimation animation;

  explicit Sprite(bool _smooth=false);
  explicit Sprite(const Surface& surface, bool _smooth=false);
  Sprite(const Sprite &other);
  ~Sprite() { frames.clear(); }

  void Init(Surface& surface, const Point2i &frameSize, int nb_frames_x, int nb_frames_y);
  Surface GetSurface() const;

  // Frame number
  unsigned int GetCurrentFrame() const
  {
    ASSERT(current_frame < frames.size());
    return current_frame;
  }
  void SetCurrentFrame( unsigned int frame_no)
  {
    ASSERT (frame_no < frames.size());
    if (current_frame != frame_no) {
      cache.InvalidLastFrame();
      MSG_DEBUG("sprite", "Set current frame : %d", frame_no);
    }
    current_frame = frame_no;
  }
  unsigned int GetFrameCount() const { return frames.size(); };

  // Antialiasing
  void SetAntialiasing(bool on)
  {
    smooth = on;
    cache.InvalidLastFrame();
  }
  bool IsAntialiased() const { return smooth; };

  // Size
  unsigned int GetWidth() const
  {
    Double one_half = 0.5;
    return static_cast<int>(round(frame_width_pix * (scale_x > 0 ? scale_x : -scale_x)));
  }
  // gives height of the surface (takes rotations into acount)
  unsigned int GetWidthMax() const
  {
    if(!current_surface.IsNull() )
      return current_surface.GetWidth();
    else
      return GetWidth();
  }
  unsigned int GetHeight() const
  {
    return static_cast<int>(round(frame_height_pix * (scale_y > 0 ? scale_y : -scale_y)));
  }
  // gives height of the surface (takes rotations into acount)
  unsigned int GetHeightMax() const
  {
    if(!current_surface.IsNull() )
      return current_surface.GetHeight();
    else
      return GetHeight();
  }
  Point2i GetSize() const { return Point2i(GetWidth(), GetHeight()); };
  Point2i GetSizeMax() const { return Point2i(GetWidthMax(), GetHeightMax()); };

  void GetScaleFactors(Double &_scale_x, Double &_scale_y) const
  {
    _scale_x = this->scale_x;
    _scale_y = this->scale_y;
  }
  Double GetScaleX(void) const { return scale_x; }
  Double GetScaleY(void) const { return scale_y; }
  void SetSize(unsigned int w, unsigned int h)
  {
    ASSERT(frame_width_pix == 0 && frame_height_pix == 0)

    frame_width_pix = w;
    frame_height_pix = h;
  }
  void SetSize(const Point2i &size) { SetSize(size.x, size.y); };

  void Scale(Double _scale_x, Double _scale_y)
  {
    this->scale_x = _scale_x;
    this->scale_y = _scale_y;
    cache.InvalidLastFrame();
  }
  void ScaleSize(int width, int height)
  {
    Scale(Double(width)/Double(frame_width_pix),
          Double(height)/Double(frame_height_pix));
  }
  void ScaleSize(const Point2i& size) { ScaleSize(size.x, size.y); };

  // Rotation
  void SetRotation_rad( Double angle_rad);
  const Double &GetRotation_rad() const
  {
    ASSERT(rotation_rad > -2*PI && rotation_rad <= 2*PI);
    return rotation_rad;
  }
  void SetRotation_HotSpot( const Point2i& new_hotspot);
  void SetRotation_HotSpot( const Rotation_HotSpot rhs) { rot_hotspot = rhs; };
  const Point2i& GetRotationPoint() const { return rotation_point; };

  SpriteFrame& operator[] (unsigned int index) { return frames.at(index); };
  const SpriteFrame& operator[] (unsigned int index) const { return frames.at(index); };
  const SpriteFrame& GetCurrentFrameObject() const { return frames[current_frame]; };

  // Prepare animation
  void AddFrame( const Surface& surf, unsigned int delay = 100);
  void SetFrameSpeed(unsigned int nv_fs)
  {
    for (uint f = 0 ; f < frames.size() ; f++)
      frames[f].delay = nv_fs;
  }

  // Animation
  void Start();
  void Update() { animation.Update(); };
  void Finish();
  bool IsFinished() const { return animation.IsFinished(); };

  // Alpha
   // Can't be combined with per pixel alpha
  void SetAlpha(Double _alpha)
  {
    ASSERT(_alpha >= ZERO && _alpha <= ONE);
    this->alpha = _alpha;
  }
  Double GetAlpha() const { return alpha; };

  // Cache
  void EnableRotationCache(unsigned int cache_size)
  { cache.EnableRotationCache(frames, cache_size); }
  void EnableFlippingCache() { cache.EnableFlippingCache(frames); };

  // Show flag
  void Show() { show = true; };
  void Hide() { show = false; };

  // Draw
  void Blit(Surface &dest, uint pos_x, uint pos_y)
  {
    RefreshSurface();
    Blit(dest, pos_x, pos_y, 0, 0, current_surface.GetWidth(), current_surface.GetHeight());
  }
  void Blit(Surface &dest, const Point2i &pos) { Blit(dest, pos.GetX(), pos.GetY()); };
  void Blit(Surface &dest, const Rectanglei &srcRect, const Point2i &destPos)
  {
    Blit(dest, destPos.GetX(), destPos.GetY(), srcRect.GetPositionX(),
         srcRect.GetPositionY(), srcRect.GetSizeX(), srcRect.GetSizeY());
  }
  void Blit(Surface &dest, int pox_x, int pos_y, int src_x, int src_y, uint w, uint h);
  void Draw(const Point2i &pos);
  void DrawXY(const Point2i &pos);

  void RefreshSurface();

private:
  Surface current_surface;
  bool show;
  // Frames
  unsigned int current_frame;
  int frame_width_pix,frame_height_pix;
  std::vector<SpriteFrame> frames;

  // Gfx
  Double alpha;
  Double scale_x,scale_y;
  Double rotation_rad;
  Point2i rhs_pos;
  Rotation_HotSpot rot_hotspot;
  Point2i rotation_point;

  void Constructor();
  void Calculate_Rotation_Offset(const Surface& tmp_surface);
};

#endif /* _SPRITE_H */
