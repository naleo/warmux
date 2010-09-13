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
 *****************************************************************************/

#include <map>
#include <iostream>
#include "character/character.h"
#include "character/member.h"
#include "character/movement.h"
#include "game/game.h"
#include "graphic/sprite.h"
#include "team/teams_list.h"
#include "team/team.h"
#include "tool/resource_manager.h"
#include "tool/string_tools.h"
#include "tool/xml_document.h"

Member::Member(const xmlNode *     xml,
               const std::string & main_folder)
  : parent(NULL)
  , angle_rad(0)
  , alpha(0)
  , go_through_ground(false)
  , attached_members()
  , pos(0,0)
  , scale(0,0)
  , spr(NULL)
  , name("")
  , type("")
  , anchor(0,0)
{
  if (NULL == xml) {
    return;
  }

  XmlReader::ReadStringAttr(xml, "name", name);
  ASSERT(name!="");
  name_is_weapon = name == "weapon";

  // Load the sprite
  spr = GetResourceManager().LoadSprite(xml, name, main_folder);
  //spr->EnableRotationCache(32);
  //spr->EnableFlippingCache();
  spr->EnableLastFrameCache();

  // Get the various option
  XmlReader::ReadStringAttr(xml, "type", type);
  ASSERT(type!="");
  type_is_weapon = type == "weapon";

  const xmlNode * el = XmlReader::GetMarker(xml, "anchor");

  if (NULL != el) {
    int dx = 0, dy = 0;
    XmlReader::ReadIntAttr(el, "dx", dx);
    XmlReader::ReadIntAttr(el, "dy", dy);
    MSG_DEBUG("body", "   Member %s has anchor (%i,%i)\n", name.c_str(), dx, dy);
    anchor = Point2d(dx, dy);
    spr->SetRotation_HotSpot(Point2i(dx, dy));
  } else {
    MSG_DEBUG("body", "   Member %s has no anchor\n", name.c_str());
  }

  XmlReader::ReadBoolAttr(xml, "go_through_ground", go_through_ground);

  xmlNodeArray                 nodes = XmlReader::GetNamedChildren(xml, "attached");
  xmlNodeArray::const_iterator it    = nodes.begin();
  xmlNodeArray::const_iterator itEnd = nodes.end();

  std::string att_type;
  int         dx = 0;
  int         dy = 0;
  Point2d     d;     // TODO: Rename !!
  std::string frame_str;

  for (; it != itEnd; ++it) {
    //std::string att_type; // TODO lami : can be move ?!

    if (!XmlReader::ReadStringAttr(*it, "member_type", att_type)) {
      std::cerr << "Malformed attached member definition" << std::endl;
      continue;
    }

    XmlReader::ReadIntAttr(*it, "dx", dx);
    XmlReader::ReadIntAttr(*it, "dy", dy);
    MSG_DEBUG("body", "   Attached member %s has anchor (%i,%i)\n", att_type.c_str(), dx, dy);
    d.SetValues(dx, dy);
    XmlReader::ReadStringAttr(*it, "frame", frame_str);

    if ("*" == frame_str) {
      v_attached rot_spot;
      rot_spot.assign(spr->GetFrameCount(), d);
      attached_members[att_type] = rot_spot;
    } else {
      int frame;

      if (!str2int(frame_str, frame) || frame < 0 || frame >= (int)spr->GetFrameCount()) {
        std::cerr << "Malformed attached member definition (wrong frame number)" << std::endl;
        continue;
      }

      if(attached_members.find(att_type) == attached_members.end()) {
        v_attached rot_spot;
        rot_spot.resize(spr->GetFrameCount(), Point2d(0.0, 0.0));
        attached_members[att_type] = rot_spot;
      }
      (attached_members.find(att_type)->second)[frame] = d;
    }
  }

  ResetMovement();
}

Member::Member(const Member & m)
  : parent(NULL)
  , angle_rad(m.angle_rad)
  , alpha(m.alpha)
  , go_through_ground(m.go_through_ground)
  , attached_members()
  , pos(m.pos)
  , scale(m.scale)
  , spr(new Sprite(*m.spr))
  , name(m.name)
  , type(m.type)
  , anchor(m.anchor)
  , name_is_weapon(m.name_is_weapon)
  , type_is_weapon(m.type_is_weapon)
{
  Point2i rot(anchor.x, anchor.y);
  spr->SetRotation_HotSpot(rot);

  // TODO: Move ! ... No process in any constructor !
  for (std::map<std::string, v_attached>::const_iterator it = m.attached_members.begin();
      it != m.attached_members.end();
      ++it) {
    attached_members[it->first] = it->second;
  }
  ResetMovement();
}

Member::~Member()
{
  delete spr;
  attached_members.clear();
}

void Member::RotateSprite()
{
  bool refreshSprite = false;

  if (spr->GetRotation_rad() != angle_rad) {
    spr->SetRotation_rad(angle_rad);
    refreshSprite = true;
  }

  if (spr->GetScaleX() != scale.x && spr->GetScaleY() != scale.y) {
    spr->Scale(scale.x, scale.y);
    refreshSprite = true;
  }

  if (refreshSprite) {
    spr->RefreshSurface();
  }
}

void Member::RefreshSprite(LRDirection direction)
{
  // The sprite pointer may be invalid at the weapon sprite.
  // Those are just asserts and not ASSERTs because they have never happened in fact
  assert(!name_is_weapon && !type_is_weapon);
  assert(parent != NULL || type == "body");

  if (DIRECTION_RIGHT == direction) {
    spr->SetRotation_rad(angle_rad);
    spr->Scale(scale.x, scale.y);
  } else {
    spr->Scale(-scale.x, scale.y);
    spr->SetRotation_rad(-angle_rad);
  }

  spr->SetAlpha(alpha);
  spr->Update();
}

void Member::Draw(const Point2i & _pos,
                  int             flip_center,
                  LRDirection     direction)
{
  assert(!name_is_weapon && !type_is_weapon);
  assert(parent || type == "body");

  Point2i posi(pos.x, pos.y);
  posi += _pos;

  if (DIRECTION_LEFT == direction) {
    posi.x = 2 * flip_center - posi.x - spr->GetWidth();
  }

  spr->Draw(posi);
}

void Member::ApplySqueleton(Member * parent_member)
{
  // Place the member to shape the skeleton
  assert(parent_member);

  if (!parent_member) {
    std::cerr << "Member " << name << " have no parent member!" << std::endl;
    return;
  }
  parent = parent_member;

  assert(!parent->name_is_weapon && !parent->type_is_weapon);

  // Set the position
  pos = parent->pos - anchor;

  std::map<std::string, v_attached>::iterator itAttachedMember = parent->attached_members.find(type);

  if (itAttachedMember != parent->attached_members.end()) {
    pos += itAttachedMember->second[parent->spr->GetCurrentFrame()];
  }
}


// TODO lami : THE function to optimize !!! 30 % CPU !!!

void Member::ApplyMovement(const member_mvt &        mvt,
                           std::vector<junction *> & skel_lst)
{
  // Apply the movment to the member,
  // And apply the movement accordingly to the child members

  uint frame = 0;

  if (NULL != spr) { // spr == NULL when Member is the weapon
    frame = spr->GetCurrentFrame();
  }

  Double radius;

  // We first apply to the child (makes calcules simpler in this order):
  for (std::map<std::string, v_attached>::iterator child = attached_members.begin();
      child != attached_members.end();
      ++child) {

    // Find this member in the skeleton:
    for (std::vector<junction *>::iterator member = skel_lst.begin();
        member != skel_lst.end();
        ++member) {

      if ((*member)->member->type != child->first) {
        continue;
      }

      // Calculate the movement to apply to the child
      member_mvt child_mvt;
      child_mvt.SetAngle(mvt.GetAngle());
      child_mvt.pos = mvt.pos;

      if (mvt.GetAngle() != ZERO) {
        Point2d child_delta = child->second[frame] - anchor;
        radius = child_delta.x*child_delta.x + child_delta.y*child_delta.y;
        if (ZERO != radius) {
          radius = sqrt_approx(radius);
          Double angle_init = child_delta.ComputeAngle() + angle_rad;
          Double angle_new  = angle_init + mvt.GetAngle();
          child_mvt.pos.x  += radius * (cos(angle_new) - cos(angle_init));
          child_mvt.pos.y  += radius * (sin(angle_new) - sin(angle_init));
        }
      }

      // Apply recursively to children:
      (*member)->member->ApplyMovement(child_mvt, skel_lst);

    }
  }

  // Apply the movement to the current member
  SetAngle(angle_rad + mvt.GetAngle());
  pos   += mvt.pos;
  alpha *= mvt.alpha;
  scale = scale * mvt.scale;
}

void Member::ResetMovement()
{
  pos.x     = 0;
  pos.y     = 0;
  angle_rad = 0;
  alpha     = 1.0;
  scale.x   = 1.0;
  scale.y   = 1.0;
}

WeaponMember::WeaponMember(void) :
  Member(NULL, "")
{
  name   = "weapon";
  type   = "weapon";
  spr    = NULL;
  anchor = Point2d(0.0, 0.0);
  name_is_weapon = true;
  type_is_weapon = true;
}

void WeaponMember::Draw(const Point2i & /*_pos*/,
                        int /*flip_center*/,
                        LRDirection /*direction*/)
{
  if (!ActiveCharacter().IsDead() && (Game::END_TURN != Game::GetInstance()->ReadState()) ) {
      ActiveTeam().crosshair.Draw();
      ActiveTeam().AccessWeapon().Draw();
  }
}
