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
 * Graphical interface showing various information about the game.
 *****************************************************************************/

#include "game/time.h"
#include "physic/force.h"
#include "physic/physical_engine.h"
#include "physic/physical_obj.h"
#include "physic/physical_shape.h"
#include "tool/math_tools.h"
#include "tool/debug_physics.h"
#include <iostream>

const double PIXEL_PER_METER = 20;

PhysicalEngine::PhysicalEngine() :
  frame_rate(60),
  last_step_time(0),
  iterations(10),
  m_static_step_in_progress(false),
  m_wind_vector(0,0)
{
  worldAABB.lowerBound.Set(-10000.0f, -10000.0f);
  worldAABB.upperBound.Set(10000.0f, 10000.0f);
  b2Vec2 gravity(0.0f, 30.0f);
  bool doSleep = true;

  physic_world = new b2World(worldAABB, gravity, doSleep);

  m_contact_listener = new ContactListener(this);
  physic_world->SetContactListener(m_contact_listener);

  m_debug_draw = new DebugDraw();
  physic_world->SetDebugDraw(m_debug_draw);

  // uint32 flags = 0;
  // flags += 0 * b2DebugDraw::e_shapeBit;
  // flags += 0 * b2DebugDraw::e_jointBit;
  // flags += 0 * b2DebugDraw::e_coreShapeBit;
  // flags += 0 * b2DebugDraw::e_aabbBit;
  // flags += 0 * b2DebugDraw::e_obbBit;
  // flags += 0 * b2DebugDraw::e_pairBit;
  // flags += 0 * b2DebugDraw::e_centerOfMassBit;
  // m_debug_draw->SetFlags(flags);

}

PhysicalEngine::~PhysicalEngine()
{
  delete m_contact_listener;
  delete m_debug_draw;
  delete physic_world;
}

b2World *PhysicalEngine::GetPhysicWorld()
{
  return physic_world;
}

b2Body *PhysicalEngine::GetNewGroundBody()
{
  b2BodyDef groundBodyDef;
  groundBodyDef.position.Set(0.0f, 0.0f);

  return physic_world->CreateBody(&groundBodyDef);
}

void PhysicalEngine::SetWindVector(const Point2d & i_wind_vector)
{
  m_wind_vector = i_wind_vector;
}

b2Body *PhysicalEngine::AddObject(PhysicalObj *new_obj)
{
  b2Body * body = physic_world->CreateBody(new_obj->GetBodyDef());
  objects_list[body] = new_obj;
  return body;
}

void PhysicalEngine::RemoveObject(PhysicalObj *obj)
{
  objects_list.erase(obj->GetBody());
  physic_world->DestroyBody(obj->GetBody());
}

b2Joint* PhysicalEngine::CreateJoint(b2JointDef* i_joint_def)
{
 return physic_world->CreateJoint(i_joint_def);
}

void PhysicalEngine::Step()
{
  float32 timeStep = 1.0f / frame_rate;

  if ((Time::GetInstance()->Read()-last_step_time) < (uint)lround(timeStep)) {
    return;
  }
  MSG_DEBUG("physical.step", "Engine step");

  for (uint i = 0; i< m_force_list.size();i++) {
    m_force_list[i]->ComputeForce();
  }
  for (uint i = 0; i< m_air_friction_shape_list.size(); i++){
    m_air_friction_shape_list[i]->ComputeAirFriction();
  }
  for (uint i = 0; i< m_auto_align_object_list.size(); i++){
    m_auto_align_object_list[i]->ComputeAutoAlign();
  }

  ComputeWind();

  physic_world->Step(timeStep, iterations);

  ComputeContacts();

  last_step_time = last_step_time-lround(timeStep);
}

void PhysicalEngine::StaticStep()
{
  if (!m_static_step_in_progress) {
    m_static_step_in_progress = true;
    physic_world->Step(0,0);
    ComputeContacts();
    m_static_step_in_progress = false;
  }
}

void PhysicalEngine::ComputeWind()
{
  //Apply wind and air resistance forces
  for (uint i = 0; i< m_wind_object_list.size();i++){
    //Extract object's b2body
    b2Body *body = m_wind_object_list[i]->GetBody();
    //  b2Vec2 relative_speed;
	  if(body)
	  {
      b2Vec2 force(m_wind_vector.x * m_wind_object_list[i]->GetWindFactor(),
                   m_wind_vector.y * m_wind_object_list[i]->GetWindFactor());
      body->ApplyForce(force, body->GetWorldPoint(b2Vec2(0,0)));
	  }
  }
}

void PhysicalEngine::ComputeContacts()
{
  for (uint i = 0; i < added_contact_list.size(); i++) {

    b2ContactPoint contact = added_contact_list.at(i);

    if ((objects_list.count(contact.shape1->GetBody()) == 1)
	&& (objects_list[contact.shape1->GetBody()] != NULL)) {

      PhysicalObj  *collider =  objects_list[contact.shape1->GetBody()];

      collider->AddContact(collider->GetShape(contact.shape1));

      collider->SignalRebound();

      Point2d vel = Point2d(contact.velocity.x*PIXEL_PER_METER,contact.velocity.y*PIXEL_PER_METER);

      collider->SignalCollision(vel);

      if (objects_list.count(contact.shape2->GetBody()) > 0) {

        collider->SignalObjectCollision(objects_list[contact.shape2->GetBody()],collider->GetShape(contact.shape1), vel);
      } else {

        collider->SignalGroundCollision(vel);
      }
    }

    if ((objects_list.count(contact.shape2->GetBody()) == 1)
	&& (objects_list[contact.shape2->GetBody()] != NULL)) {

      PhysicalObj  *collider = objects_list[contact.shape2->GetBody()];

      collider->AddContact(collider->GetShape(contact.shape2));

      collider->SignalRebound();

      Point2d vel = Point2d(contact.velocity.x*PIXEL_PER_METER,contact.velocity.y*PIXEL_PER_METER);

      collider->SignalCollision(vel);

      if (objects_list.count(contact.shape1->GetBody()) > 0) {

        collider->SignalObjectCollision(objects_list[contact.shape1->GetBody()],collider->GetShape(contact.shape2), vel);
      } else {

        collider->SignalGroundCollision(vel);
      }
    }

  }

  for (uint i = 0; i < persist_contact_list.size(); i++) {
     // std::cout<<"Pesist"<<std::endl;
  }


  for (uint i = 0; i < removed_contact_list.size(); i++) {


    b2ContactPoint contact = removed_contact_list[i];

    if ((objects_list.count(contact.shape1->GetBody()) == 1)
	&& (objects_list[contact.shape1->GetBody()] != NULL)) {
      PhysicalObj  *collider =  objects_list[contact.shape1->GetBody()];

      collider->RemoveContact(collider->GetShape(contact.shape1));
    }

    if ((objects_list.count(contact.shape2->GetBody()) == 1)
	&& (objects_list[contact.shape2->GetBody()] != NULL)) {
      PhysicalObj  *collider =  objects_list[contact.shape2->GetBody()];

      collider->RemoveContact(collider->GetShape(contact.shape2));
    }
  }
  ClearContact();

}


void PhysicalEngine::ClearContact()
{
  added_contact_list.clear();
  persist_contact_list.clear();
  removed_contact_list.clear();
  result_contact_list.clear();
}


void PhysicalEngine::AddForce(Force * force)
{
    m_force_list.push_back(force);
}

void PhysicalEngine::RemoveForce(Force *force)
{

  std::vector<Force *>::iterator it;
  for (it = m_force_list.begin(); it != m_force_list.end(); it++){
    if (*it == force) {
      m_force_list.erase(it);
      break;
    }

  }

}


void PhysicalEngine::AddAirFrictionShape(PhysicalShape * shape)
{
  m_air_friction_shape_list.push_back(shape);
}

void PhysicalEngine::RemoveAirFrictionShape(PhysicalShape *shape)
{
  std::vector<PhysicalShape *>::iterator it;
  for (it = m_air_friction_shape_list.begin(); it != m_air_friction_shape_list.end(); it++){
    if (*it == shape) {
      m_air_friction_shape_list.erase(it);
      break;
    }
  }
}



void PhysicalEngine::AddWindObject(PhysicalObj *i_object)
{
	m_wind_object_list.push_back(i_object);
}

void PhysicalEngine::RemoveWindObject(PhysicalObj *i_object)
{
  std::vector<PhysicalObj *>::iterator it;
  for (it = m_wind_object_list.begin(); it != m_wind_object_list.end(); it++){
    if (*it == i_object) {
      m_wind_object_list.erase(it);
      break;
    }
  }
}

void PhysicalEngine::AddAutoAlignObject(PhysicalObj * object)
{
    m_auto_align_object_list.push_back(object);
}

void PhysicalEngine::RemoveAutoAlignObject(PhysicalObj *object)
{
  std::vector<PhysicalObj *>::iterator it;
  for (it = m_auto_align_object_list.begin(); it != m_auto_align_object_list.end(); it++){
    if (*it == object) {
      m_auto_align_object_list.erase(it);
      break;
    }
  }
}


void PhysicalEngine::AddContactPoint(b2ContactPoint contact,ContactType type)
{
  switch(type) {
  case ADD:
    added_contact_list.push_back(contact);
    break;
  case PERSIST:
    persist_contact_list.push_back(contact);
    break;
  case REMOVE:
    removed_contact_list.push_back(contact);
    break;
  }
}


void PhysicalEngine::AddContactResult(b2ContactResult contact)
{
  result_contact_list.push_back(contact);
}



ContactListener::ContactListener(PhysicalEngine *e):b2ContactListener()
{
  engine = e;
}

void ContactListener::Add(const b2ContactPoint* point)
{
  b2ContactPoint contact = *point;
  engine->AddContactPoint(contact,PhysicalEngine::ADD);
}

void ContactListener::Persist(const b2ContactPoint* point)
{
  b2ContactPoint contact = *point;
  engine->AddContactPoint(contact,PhysicalEngine::PERSIST);
}

void ContactListener::Remove(const b2ContactPoint* point)
{
  b2ContactPoint contact = *point;
  engine->AddContactPoint(contact,PhysicalEngine::REMOVE);
}

void ContactListener::Result(const b2ContactResult* point)
{
  b2ContactResult contact = *point;
  engine->AddContactResult(contact);
}



