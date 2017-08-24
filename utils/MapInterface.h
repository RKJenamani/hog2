/*
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
 */ 

#ifndef MAPInterface_H
#define MAPInterface_H

#include "GLUtil.h"

class MapInterface{
public:
  virtual bool GetOpenGLCoord(int _x, int _y, GLdouble &x, GLdouble &y, GLdouble &z, GLdouble &radius) const=0;
  virtual bool GetOpenGLCoord(float _x, float _y, GLdouble &x, GLdouble &y, GLdouble &z, GLdouble &radius) const=0;
  virtual bool GetOpenGLCoord(int _x, int _y, int _z, GLdouble &x, GLdouble &y, GLdouble &z, GLdouble &radius) const=0;
  virtual bool GetOpenGLCoord(float _x, float _y, float _z, GLdouble &x, GLdouble &y, GLdouble &z, GLdouble &radius) const=0;
  virtual inline long GetMapWidth() const=0;
  virtual inline long GetMapHeight() const=0;
};

#endif