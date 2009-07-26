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
 * Physical Engine using Bullet Physic library.
 *****************************************************************************/
#include "physic/bullet_engine.h"
#include "physic/bullet_obj.h"
#include "physic/bullet_shape.h"
#include "game/time.h"
#include "physical_engine.h"
#include "bullet_engine.h"
#include <iostream>

BulletEngine::BulletEngine() : PhysicalEngine() {
    m_scale = 100.0;
    m_frame_rate = 60;
    ///collision configuration contains default setup for memory, collision setup
    btDefaultCollisionConfiguration *collision_configuration = new btDefaultCollisionConfiguration();
    //m_collisionConfiguration->setConvexConvexMultipointIterations();

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    btCollisionDispatcher *dispatcher = new btCollisionDispatcher(collision_configuration);

    btDbvtBroadphase *broadphase = new btDbvtBroadphase();

    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

    m_world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_configuration);
    //m_world = new btContinuousDynamicsWorld(dispatcher, broadphase, solver, collision_configuration);

    m_world->setGravity(btVector3(0, 10, 0));


    //Debug ground
     btRigidBody* m_body;
      btTransform startTransform;
    startTransform.setIdentity();

    btScalar mass(0.f);
    btVector3 localInertia(0, 0, 0);

    startTransform.setOrigin(btVector3(1500/GetScale(), 1500/GetScale(), 0));
    btCollisionShape* colShape = new btBoxShape(btVector3(10000/GetScale(),100/GetScale(),100/GetScale()));

   // colShape->calculateLocalInertia(mass,localInertia);

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
    m_body = new btRigidBody(rbInfo);
    m_body->setActivationState(ISLAND_SLEEPING);

  //  m_world->addRigidBody(m_body,0xFFFF,0xFFFF);
    m_world->addRigidBody(m_body,0x0002,0xFFFF);
    m_body->setActivationState(ACTIVE_TAG);
    m_body->setRestitution(0.5);
}

BulletEngine::~BulletEngine()
{

}

PhysicalObj *BulletEngine::CreateObject(PhysicalEngine::ObjectType /*type*/)
{

    return new BulletObj();
}

PhysicalPolygon *BulletEngine::CreatePolygonShape()
{
    return new BulletPolygon();
}
PhysicalCircle *BulletEngine::CreateCircleShape()
{
    return new BulletCircle();
}
PhysicalRectangle *BulletEngine::CreateRectangleShape(double width, double height)
{
    return new BulletRectangle( width,  height);
}


void
BulletEngine::AddObject(PhysicalObj *new_obj)
{
    BulletObj *obj = reinterpret_cast<BulletObj *>(new_obj);
    obj->GetBody()->setActivationState(ISLAND_SLEEPING);
    m_world->addRigidBody(obj->GetBody(), obj->GetCollisionCategory(),obj->GetcollisionMask());
    //m_world->addRigidBody(obj->GetBody());
    obj->GetBody()->setActivationState(ACTIVE_TAG);
std::cout<<"Add "<<new_obj<<" x="<<new_obj->GetPosition().x<<" y="<<new_obj->GetPosition().y<<std::endl;
    /* b2Body * body = physic_world->CreateBody(new_obj->GetBodyDef());
  objects_list[body] = new_obj;
  return body;*/
}

void BulletEngine::RemoveObject(PhysicalObj *obj)
{
  BulletObj *bobj = reinterpret_cast<BulletObj *>(obj);
  m_world->removeRigidBody(bobj->GetBody());
  std::cout<<"Remove "<<obj<<std::endl;
}

void BulletEngine::Step()
{

  btScalar timeStep = 1.0f / m_frame_rate;

  if ((Time::GetInstance()->Read()-m_last_step_time) < (uint)lround(timeStep)) {
    return;
  }
 MSG_DEBUG("physical.step", "Engine step");
  m_world->stepSimulation(timeStep);
  m_last_step_time = m_last_step_time +lround(timeStep);

  /*
  

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
  ComputeModifiedGravity();

  physic_world->Step(timeStep, iterations);

  ComputeContacts();

  */
}

double BulletEngine::GetScale() const
{
  return m_scale;
}



