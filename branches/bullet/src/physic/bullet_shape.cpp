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
 * Bullet shape
 *****************************************************************************/
#include "physic/bullet_shape.h"
#include "graphic/video.h"
#include "map/camera.h"
#include "physic/bullet_engine.h"

BulletShape::BulletShape()
{
  m_native_shape = NULL;
}

BulletRectangle::BulletRectangle(double width, double height):PhysicalRectangle(width,height),
m_shape(btVector3(width*GetScale(),height*GetScale(),100*GetScale()))
{

}

BulletRectangle::~BulletRectangle()
{

}

void BulletRectangle::Generate()
{
  btBoxShape * new_shape = new btBoxShape(btVector3(m_width*GetScale(),m_height*GetScale(),100*GetScale()));



  btScalar mass(1.0f);
  btVector3 localInertia(0, 0, -50*GetScale());


  new_shape->calculateLocalInertia(mass,localInertia);

  if(m_native_shape)
  {
    delete m_native_shape;
  }

  m_native_shape = new_shape;

}
 double BulletRectangle::Area() const
 {
   return m_width*m_height;
 }
  double BulletRectangle::GetCurrentWidth() const
  {

   /*  m_shape.getAabb   ( const btTransform &      t,
         btVector3 &     aabbMin,
         btVector3 &     aabbMax
 )    */
    return m_width;
  }

  double BulletRectangle::GetCurrentHeight() const
  {
    return m_height;
  }
  double BulletRectangle::GetInitialWidth() const
  {
    return m_width;
  }
  double BulletRectangle::GetInitialHeight() const
  {
    return m_height;
  }

  double BulletRectangle::GetCurrentMinX() const
  {
    return m_position.x;
  }
  double BulletRectangle::GetCurrentMaxX() const
  {
    return m_position.x+m_width;

  }
  double BulletRectangle::GetCurrentMinY() const
  {
    return m_position.y;
  }
  double BulletRectangle::GetCurrentMaxY() const
  {
    return m_position.y+m_height;
  }

  double BulletRectangle::GetInitialMinX() const
  {
    return m_position.x;
  }
  double BulletRectangle::GetInitialMaxX() const
  {
    return m_position.x+m_width;
  }
  double BulletRectangle::GetInitialMinY() const
  {
    return m_position.y;
  }
  double BulletRectangle::GetInitialMaxY() const
  {
    return m_position.y+m_height;
  }


#ifdef DEBUG
void BulletRectangle::DrawBorder(const Color& color) const
{
  ASSERT(m_parent);

  ASSERT(m_parent);

    btVector3 pos;
    btScalar radius;
    m_native_shape->getBoundingSphere(pos,radius);

    GetMainWindow().CircleColor(pos.x()*GetScale()+m_parent->GetPosition().x - Camera::GetInstance()->GetPosition().x,pos.y()*GetScale()+m_parent->GetPosition().y - Camera::GetInstance()->GetPosition().y,radius*GetScale(),color);
    GetMainWindow().LineColor(m_parent->GetPosition().x-1 - Camera::GetInstance()->GetPosition().x,m_parent->GetPosition().x+1 - Camera::GetInstance()->GetPosition().x, m_parent->GetPosition().y - Camera::GetInstance()->GetPosition().y,m_parent->GetPosition().y - Camera::GetInstance()->GetPosition().y,Color(0,0,0,255));
    GetMainWindow().LineColor(m_parent->GetPosition().x - Camera::GetInstance()->GetPosition().x,m_parent->GetPosition().x - Camera::GetInstance()->GetPosition().x, m_parent->GetPosition().y-1 - Camera::GetInstance()->GetPosition().y,m_parent->GetPosition().y+1 - Camera::GetInstance()->GetPosition().y,Color(0,0,0,255));
/*  b2PolygonShape* polygon = (b2PolygonShape*)m_shape;

  ASSERT(polygon->GetVertexCount() > 2);
  b2XForm xf = m_body->GetXForm();
  int init_x = lround(( b2Mul(xf,polygon->GetVertices()[0]).x)*PIXEL_PER_METER) - Camera::GetInstance()->GetPosition().x;
  int init_y = lround(( b2Mul(xf,polygon->GetVertices()[0]).y)*PIXEL_PER_METER) - Camera::GetInstance()->GetPosition().y;
  int prev_x = init_x;
  int prev_y = init_y;
  int x, y;

  for (uint i = 1; i< uint(polygon->GetVertexCount()); i++) {

    x = lround(( b2Mul(xf,polygon->GetVertices()[i]).x)*PIXEL_PER_METER) - Camera::GetInstance()->GetPosition().x;
    y = lround(( b2Mul(xf,polygon->GetVertices()[i]).y)*PIXEL_PER_METER) - Camera::GetInstance()->GetPosition().y;

    GetMainWindow().LineColor(prev_x, x, prev_y, y, color);
    prev_x = x;
    prev_y = y;
  }

  GetMainWindow().LineColor(prev_x, init_x, prev_y, init_y, color);*/
}
#endif


  BulletPolygon::~BulletPolygon()
  {
      delete m_native_shape;
  }

  void BulletPolygon::Generate()
 {
      btConvexHullShape * new_shape = new btConvexHullShape();

      for(uint i=0;i < m_point_list.size(); i++)
      {
          new_shape->addPoint(btVector3(m_point_list[i].x/GetScale(),m_point_list[i].y/GetScale(),-50/GetScale()));
          new_shape->addPoint(btVector3(m_point_list[i].x/GetScale(),m_point_list[i].y/GetScale(),50/GetScale()));

      }

      if(m_native_shape)
      {
        delete m_native_shape;
      }

      m_native_shape = new_shape;


     // btCollisionShape* new_shape = new btBoxShape(btVector3(100,100,100));
     // m_native_shape = new_shape;
 }

#ifdef DEBUG

#include <iostream>
void BulletPolygon::DrawBorder(const Color& color) const
{
  ASSERT(m_parent);




  ASSERT(m_point_list.size() > 2);


  int init_x = lround(m_point_list[0].x+m_parent->GetPosition().x- Camera::GetInstance()->GetPosition().x);
  int init_y = lround(m_point_list[0].y+m_parent->GetPosition().y - Camera::GetInstance()->GetPosition().y);
  int prev_x = init_x;
  int prev_y = init_y;
  int x;
  int y;

  for(uint i = 1; i < m_point_list.size();i++){

        x = lround(m_point_list[i].x+m_parent->GetPosition().x- Camera::GetInstance()->GetPosition().x);
        y =  lround(m_point_list[i].y+m_parent->GetPosition().y - Camera::GetInstance()->GetPosition().y);

       GetMainWindow().LineColor(prev_x, x, prev_y, y, color);
       prev_x = x;
       prev_y = y;
  }
  GetMainWindow().LineColor(prev_x, init_x, prev_y, init_y, color);
  GetMainWindow().LineColor(m_parent->GetPosition().x-1 - Camera::GetInstance()->GetPosition().x,m_parent->GetPosition().x+1 - Camera::GetInstance()->GetPosition().x, m_parent->GetPosition().y - Camera::GetInstance()->GetPosition().y,m_parent->GetPosition().y - Camera::GetInstance()->GetPosition().y,Color(0,0,0,255));
  GetMainWindow().LineColor(m_parent->GetPosition().x - Camera::GetInstance()->GetPosition().x,m_parent->GetPosition().x - Camera::GetInstance()->GetPosition().x, m_parent->GetPosition().y-1 - Camera::GetInstance()->GetPosition().y,m_parent->GetPosition().y+1 - Camera::GetInstance()->GetPosition().y,Color(0,0,0,255));

}
#endif

///////////
  //Bullet Circle


  BulletCircle::~BulletCircle()
  {
      delete m_native_shape;
  }

  void BulletCircle::Generate()
  {
    btCylinderShapeZ * new_shape = new btCylinderShapeZ(btVector3(m_radius/GetScale(),m_radius/GetScale(),100/GetScale()));

    //new_shape->setLocalScaling(btVector3(m_radius,m_radius,10));


   if(m_native_shape)
   {
     delete m_native_shape;
   }

   m_native_shape = new_shape;
  }

#ifdef DEBUG
void BulletCircle::DrawBorder(const Color& color) const
{
  ASSERT(m_parent);



    GetMainWindow().CircleColor(GetPosition().x+m_parent->GetPosition().x - Camera::GetInstance()->GetPosition().x,GetPosition().y+m_parent->GetPosition().y - Camera::GetInstance()->GetPosition().y,m_radius,color);


    GetMainWindow().LineColor(0 ,1000   , 1400 - Camera::GetInstance()->GetPosition().y, 1400 - Camera::GetInstance()->GetPosition().y, color );
    GetMainWindow().LineColor(0 ,1000   , 1600 - Camera::GetInstance()->GetPosition().y, 1600 - Camera::GetInstance()->GetPosition().y, color );

    GetMainWindow().LineColor(m_parent->GetPosition().x-1 - Camera::GetInstance()->GetPosition().x,m_parent->GetPosition().x+1 - Camera::GetInstance()->GetPosition().x, m_parent->GetPosition().y - Camera::GetInstance()->GetPosition().y,m_parent->GetPosition().y - Camera::GetInstance()->GetPosition().y,Color(0,0,0,255));
   GetMainWindow().LineColor(m_parent->GetPosition().x - Camera::GetInstance()->GetPosition().x,m_parent->GetPosition().x - Camera::GetInstance()->GetPosition().x, m_parent->GetPosition().y-1 - Camera::GetInstance()->GetPosition().y,m_parent->GetPosition().y+1 - Camera::GetInstance()->GetPosition().y,Color(0,0,0,255));

/*  b2PolygonShape* polygon = (b2PolygonShape*)m_shape;

  ASSERT(polygon->GetVertexCount() > 2);
  b2XForm xf = m_body->GetXForm();
  int init_x = lround(( b2Mul(xf,polygon->GetVertices()[0]).x)*PIXEL_PER_METER) - Camera::GetInstance()->GetPosition().x;
  int init_y = lround(( b2Mul(xf,polygon->GetVertices()[0]).y)*PIXEL_PER_METER) - Camera::GetInstance()->GetPosition().y;
  int prev_x = init_x;
  int prev_y = init_y;
  int x, y;

  for (uint i = 1; i< uint(polygon->GetVertexCount()); i++) {

    x = lround(( b2Mul(xf,polygon->GetVertices()[i]).x)*PIXEL_PER_METER) - Camera::GetInstance()->GetPosition().x;
    y = lround(( b2Mul(xf,polygon->GetVertices()[i]).y)*PIXEL_PER_METER) - Camera::GetInstance()->GetPosition().y;

    GetMainWindow().LineColor(prev_x, x, prev_y, y, color);
    prev_x = x;
    prev_y = y;
  }

  GetMainWindow().LineColor(prev_x, init_x, prev_y, init_y, color);*/
}
#endif

double BulletShape::GetScale() const
  {
   return (reinterpret_cast<BulletEngine *>(PhysicalEngine::GetInstance()))->GetScale();
  }

