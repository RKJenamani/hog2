/*
 * $Id: unit.cpp,v 1.18 2007/02/22 01:50:51 bulitko Exp $
 *
 *  Hierarchical Open Graph File
 *
 *  Created by Nathan Sturtevant on 9/28/04.
 *  Copyright 2004 Nathan Sturtevant. All rights reserved.
 *
 * This file is part of HOG.
 *
 * HOG is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * HOG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with HOG; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "Unit.h"
#include "GLUtil.h"
#include "FPUtil.h"

//GLuint unit::sphereDispList = 0;
//int unit::unitID = 0;

///** Create a unit
//*
//* Create a unit starting at a given x/y location, with a potential target.
//*/
//unit::unit(int _x, int _y, unit *_target)
//:x(_x), y(_y), target(_target), group(0)
//{
//	unitType = kDisplayOnly;
//	speed = .5;
//	map_revision = -1;
//	//	blocking = true;
//	//	lastMoveSuccess = true;
//	//	aMap = 0;
//	
//	r = (GLfloat)(random()%256)/256.0;
//	g = (GLfloat)(random()%256)/256.0;
//	b = (GLfloat)(random()%256)/256.0;
//	if (target)
//	{
//		if (target->getObjectType() == kDisplayOnly)
//			target->setColor(r, g, b);
//		else
//			setColor(target->r*.8, target->g*.8, target->b*.8);
//	}
//
//	id = unit::unitID++;
//
//}
//
//
///** Create a unit
//*
//* Create a unit starting at a given x/y location, r/g/b (0.255), with a potential target.
//*/
//// Create a unit with specified color
//unit::unit(int _x, int _y, int _r, int _g, int _b, unit *_target)
//:x(_x), y(_y), target(_target), group(0)
//{
//	unitType = kDisplayOnly;
//	speed = .5;
//	map_revision = -1;
//	
//	//	blocking = true;
//	//	lastMoveSuccess = true;
//	//	aMap = 0;
//	
//	r = (GLfloat)(_r%256)/256.0;
//	g = (GLfloat)(_g%256)/256.0;
//	b = (GLfloat)(_b%256)/256.0;
//	if (target)
//	{
//		if (target->getObjectType() == kDisplayOnly)
//			target->setColor(r, g, b);
//		else
//			setColor(target->r*.8, target->g*.8, target->b*.8);
//	}
//
//	id = unit::unitID++;
//}
//
///** Create a unit
//*
//* Create a unit starting at a given x/y location, r/g/b (0.255), with a potential target.
//*/
//// Create a unit with specified color
//unit::unit(int _x, int _y, float _r, float _g, float _b, unit *_target)
//:x(_x), y(_y), target(_target), group(0)
//{
//	unitType = kDisplayOnly;
//	speed = .5;
//	map_revision = -1;
//	
//	//	blocking = true;
//	//	lastMoveSuccess = true;
//	//	aMap = 0;
//	
//	r = _r;
//	g = _g;
//	b = _b;
//	if (target)
//	{
//		if (target->getObjectType() == kDisplayOnly)
//			target->setColor(r, g, b);
//		else
//			setColor(target->r*.8, target->g*.8, target->b*.8);
//	}
//	
//	id = unit::unitID++;
//	
//}
//
//
//unit::~unit()
//{
//}
//
//void unit::setUnitGroup(unitGroup *_group)
//{
//	// If we already set to the given group then do nothing
//	if (_group == group)
//		return;
//
//	unitGroup *tmp = group;
//	// Set the back pointer
//	group = _group;
//
//	// If we had a group before then move
//	if (tmp != 0)
//	{
//		tmp->removeUnit(this);
//		if (_group)
//			_group->addUnit(this);
//	}
//}
//
////	if (_group == group)
////		return;
////	if (group)
////		group->removeUnit(this);
////	group = _group;
////	if (group)
////		group->addUnit(this);
//
//
////void unit::madeMove(unit *, tDirection ) { }
//
///** Make a move in the environment
//*
//* This is the most important function for any unit. Given an abstract map of the
//* world, the unit must decide how to act next, by returning a direction for the
//* next step. Overload this function to define your own unit movement. The default
//* action is to stay put.
//*/
//tDirection unit::makeMove(MapProvider *, reservationProvider *, SimulationInfo *)
//{
//	return kStay;
//}
//
///** Log any stats the unit might have collected so far.
//*
//* No stats collected by default.
//*/
//void unit::LogStats(StatCollection *)
//{
//}
//
///** is map updated?
//*
//* returns true if the map has been udpated from the last time we checked.
//*/
//bool unit::mapUpdated(MapAbstraction *aMap)
//{
//	Map *map = aMap->GetMap();
//	if (map_revision != map->GetRevision())
//	{
//		map_revision = map->GetRevision();
//		//printf("%p map_revision updated to %d\n", this, map_revision);
//		return true;
//	}
//	return false;
//}
//
///**
//* getLocation of unit
// *
// * returns the current unit location by reference
// */
//void unit::getLocation(int &_x, int &_y)
//{
//	_x = x; _y = y;
//}
//
///**
//* Get OpenGL coordinates of unit.
// *
// * Returns the x/y/z open GL coordinates of a unit, as well as the radius of the unit
// * in the world.
// */
//void unit::getOpenGLLocation(Map *map, GLdouble &_x, GLdouble &_y, GLdouble &_z, GLdouble &radius)
//{
//	map->GetOpenGLCoord(x, y, _x, _y, _z, radius);
//}
//
///**
//* Draw the unit.
// *
// * Draw the unit using openGL. Overload this if you want a custom look for the unit.
// */
//void unit::OpenGLDraw(MapProvider *mp, SimulationInfo *)
//{
//	Map *map = mp->GetMap();
//	GLdouble xx, yy, zz, rad;
//	if ((x < 0) || (x >= map->GetMapWidth()) || (y < 0) || (y >= map->GetMapHeight()))
//		return;
//	map->GetOpenGLCoord(x, y, xx, yy, zz, rad);
//	glColor3f(r, g, b);
//	if (getObjectType() == kDisplayOnly)
//		drawTriangle(xx, yy, zz, rad);
//	else
//		drawSphere(xx, yy, zz, rad);
//}
//
///**
//* Draw a triangle for the unit.
// *
// * Draw's a pyramid at the location specified. Used for drawing kDisplayOnly objects.
// */
//void unit::drawTriangle(GLdouble _x, GLdouble _y, GLdouble _z, GLdouble tRadius)
//{
//	glEnable(GL_LIGHTING);
//	DrawPyramid(_x, _y, _z, tRadius, 0.75*tRadius);
//}
//
///**
//* Draw a dome for the unit.
// *
// * Draw a dome at the coordinates specified; used for all other units by default.
// */
//void unit::drawSphere(GLdouble _x, GLdouble _y, GLdouble _z, GLdouble tRadius)
//{
//	glEnable(GL_LIGHTING);
//	if (sphereDispList)
//	{
//		glTranslatef(_x, _y, _z);
//		glCallList(sphereDispList);
//		glTranslatef(-_x, -_y, -_z);
//		return;
//	}
//	
//	sphereDispList = glGenLists(1);
//	glTranslatef(_x, _y, _z);
//	glNewList(sphereDispList, GL_COMPILE_AND_EXECUTE);
//	int i,j;
//	int n = 64; // precision
//	double theta1,theta2,theta3;
//	point3d e,p,c(0, 0, 0);
//	
//	if (tRadius < 0) tRadius = -tRadius;
//	if (n < 0) n = -n;
//	if (n < 4 || tRadius <= 0)
//	{
//		glBegin(GL_POINTS);
//		glVertex3f(c.x,c.y,c.z);
//		glEnd();
//	}
//	else {
//		for (j=n/4;j<n/2;j++)
//		{
//			theta1 = j * TWOPI / n - PID2;
//			theta2 = (j + 1) * TWOPI / n - PID2;
//			
//			glBegin(GL_QUAD_STRIP);
//			//glBegin(GL_POINTS);
//			//glBegin(GL_TRIANGLE_STRIP);
//			//glBegin(GL_LINE_STRIP);
//			for (i=0;i<=n;i++)
//			{
//				theta3 = i * TWOPI / n;
//				
//				e.x = cos(theta2) * cos(theta3);
//				e.y = cos(theta2) * sin(theta3);
//				e.z = sin(theta2);
//				p.x = c.x + tRadius * e.x;
//				p.y = c.y + tRadius * e.y;
//				p.z = c.z - tRadius * e.z;
//				
//				glNormal3f(-e.x,-e.y,e.z);
//				//glTexCoord2f(i/(double)n,2*(j+1)/(double)n);
//				glVertex3f(p.x,p.y,p.z);
//				
//				e.x = cos(theta1) * cos(theta3);
//				e.y = cos(theta1) * sin(theta3);
//				e.z = sin(theta1);
//				p.x = c.x + tRadius * e.x;
//				p.y = c.y + tRadius * e.y;
//				p.z = c.z - tRadius * e.z;
//				
//				glNormal3f(-e.x,-e.y,e.z);
//				//glTexCoord2f(i/(double)n,2*j/(double)n);
//				glVertex3f(p.x,p.y,p.z);
//			}
//			glEnd();
//		}
//	}
//	glEndList();
//	glTranslatef(-_x, -_y, -_z);
//}
//
