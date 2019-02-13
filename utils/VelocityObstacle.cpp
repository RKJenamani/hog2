/*
 *  Created by Thayne Walker.
 *  Copyright (c) Thayne Walker 2017 All rights reserved.
 *
 * This file is part of HOG2.
 *
 * HOG2 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * HOG2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with HOG; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#include "VelocityObstacle.h"
#include <assert.h>
#include <iostream>
//#include <CGAL/Boolean_set_operations_2.h>
//#include <CGAL/Polygon_2.h>

VelocityObstacle::VelocityObstacle(Vector2D const& a, Vector2D const& va, Vector2D const& b, Vector2D const& vb, double r1, double r2)
  : VO(a+vb), VL(0,0), VR(0,0)
{
  if(r2==DBL_MAX){r2=r1;}
  // Compute VL and VR
  std::vector<Vector2D> tangents;
  if(getTangentOfCircle(b+vb,r1+r2,VO,tangents)){
    VL=tangents[1];
    VR=tangents[0];
  }else{ // Punt. This should *never* happen
    VL=normal(VO,b+vb);
    VR=-VL;
  }
}

// Polygon points are relative to the center fo the body (a, b respectively)
/*VelocityObstacle::VelocityObstacle(Vector2D const& a, Vector2D const& va, Vector2D const& b, Vector2D const& vb, std::vector<Vector2D>const& polyA, std::vector<Vector2D>const& polyB)
  : VO(a+vb), VL(0,0), VR(0,0)
{
  // Compute points in minkowski sum
  std::vector<Vector2D> points;
  points.reserve(polyA.size()*polyB.size()+1);

  Vector2D center(b+vb);
  for(auto const& pa:polyA){
    for(auto const& pb:polyB){
      points.push_back(center+pa+pb);
    }
  }
  
  std::vector<Vector2D> result;
  points.push_back(VO); // Add apex
  Util::convexHull(points,result);

  // Compute VL and VR
  int i=0;
  // Find the apex in the convex hull
  for(; i<result.size(); ++i){
    if(result[i] == VO){
      break;
    }
  }
  // VL and VR are on either side of the apex
  if(i==0){
    VL=result.back();
    VR=result[1];
  }
  else if(i==result.size()-1){
    VL=result[result.size()-2];
    VR=result.front();
  }else{
    VL=result[i-1];
    VR=result[i+1];
  }

  // If the center point is not between the tangents, swap them
  if(!IsInside(center)){
    Vector2D tmp(VL);
    VR=tmp;
    VL=VR;
  }
}*/

bool getTangentOfCircle(Vector2D const& center, double radius, Vector2D const& point, std::vector<Vector2D>& tangents){
  assert(fless(0,radius)); //If the agent has no size, there's no VOB to compute
 
  Vector2D n((point-center)/radius);
  double xy=n.sq();

  if(fleq(xy,1.0)){return false;} // point is in or on circle

  tangents.resize(2);
  double d(n.y*sqrt(xy-1.0));
  double tx0((n.x-d)/xy);
  double tx1((n.x+d)/xy);
  if(!fequal(0.0,n.y)){
    tangents[0].y=center.y+radius*(1.0-tx0*n.x)/n.y;
    tangents[1].y=center.y+radius*(1.0-tx1*n.x)/n.y;
  }else{
    d=radius*sqrt(1.0-tx0*tx0);
    tangents[0].y=center.y+d;
    tangents[1].y=center.y-d;
  }
  tangents[0].x=center.x+radius*tx0;
  tangents[1].x=center.x+radius*tx1;
  return true;
}

// Input is the two center points and their radiuses
/*bool VelocityObstacle::AgentOverlap(Vector2D const& A,Vector2D const& B,std::vector<Vector2D>const& polyA,std::vector<Vector2D>const& polyB){
  Points a;
  a.reserve(polyA.size());
  for(auto const& aa:polyA){
    a.push_back(A+aa);
  }
  Points b;
  b.reserve(polyB.size());
  for(auto const& bb:polyB){
    b.push_back(B+bb);
  }
  return CGAL::do_intersect(Polygon_2(a.begin(),a.end()),Polygon_2(b.begin(),b.end()));
}*/

// Input is the two center points and their radiuses
bool VelocityObstacle::AgentOverlap(Vector2D const& A,Vector2D const& B,double ar,double br){
  return fless((B-A).sq(),(ar+br)*(ar+br));
}

// Input should be a point relative to A after a normalized time step.
// e.g. VOB velocities are normalized, so the input point should be normalized
// from: http://stackoverflow.com/questions/1560492/how-to-tell-whether-a-point-is-to-the-right-or-left-side-of-a-line
bool VelocityObstacle::IsInside(Vector2D const& point) const{
     Vector2D apexDiff(point-VO);
     //Check if it is strictly right of the left vector
     return ((VL.x - VO.x)*(apexDiff.y) < (VL.y - VO.y)*(apexDiff.x)) &&
     // Check if it is strictly left of the right vector
       ((VR.x - VO.x)*(apexDiff.y) > (VR.y - VO.y)*(apexDiff.x));
}

/*bool detectCollisionPolygonalAgents(Vector2D A, Vector2D const& VA, std::vector<Vector2D>const& polyA, double startTimeA, double endTimeA,
Vector2D B, Vector2D const& VB, std::vector<Vector2D>const& polyB, double startTimeB, double endTimeB){

  // check for time overlap
  if(fgreater(startTimeA,endTimeB)||fgreater(startTimeB,endTimeA)){return false;}

  if(fgreater(startTimeB,startTimeA)){
    // Move A to the same time instant as B
    A+=VA*(startTimeB-startTimeA);
    startTimeA=startTimeB;
  }else if(fless(startTimeB,startTimeA)){
    B+=VB*(startTimeA-startTimeB);
    startTimeB=startTimeA;
  }

  // Check for immediate collision
  if(VelocityObstacle::AgentOverlap(A,B,polyA,polyB)){return true;}

  // Check for collision in future
  if(!VelocityObstacle(A,VA,B,VB,polyA,polyB).IsInside(A+VA)){return false;}
  
  // If we got here, we have established that a collision will occur
  // if the agents continue indefinitely. However, we can now check
  // the end of the overlapping interval to see if the collision is
  // still in the future. If so, no collision has occurred in the interval.
  double duration(std::min(endTimeB,endTimeA)-startTimeA);
  A+=VA*duration;
  B+=VB*duration;
  
  // Check for immediate collision at end of time interval
  if(VelocityObstacle::AgentOverlap(A,B,polyA,polyB)){return true;}

  // Finally, if the collision is still in the future we're ok.
  return !VelocityObstacle(A,VA,B,VB,polyA,polyB).IsInside(A+VA);
}*/

// Detect whether a collision will occur between agent A and B inside the
// given time intervals if they continue at constant velocity with no turns
//
// Inputs: agent A position,
//         agent A velocity vector - normalized to 1 unit time step
//         agent A radius
//         agent A movement start time
//         agent A movement end time
//         same for agent B
//
bool detectCollision(Vector2D A, Vector2D const& VA, double radiusA, double startTimeA, double endTimeA,
Vector2D B, Vector2D const& VB, double radiusB, double startTimeB, double endTimeB){
  // check for time overlap
  if(fgreater(startTimeA,endTimeB)||fgreater(startTimeB,endTimeA)){return false;}

  if(fgreater(startTimeB,startTimeA)){
    // Move A to the same time instant as B
    A+=VA*(startTimeB-startTimeA);
    startTimeA=startTimeB;
  }else if(fless(startTimeB,startTimeA)){
    B+=VB*(startTimeA-startTimeB);
    startTimeB=startTimeA;
  }

  // Check for immediate collision
  if(VelocityObstacle::AgentOverlap(A,B,radiusA,radiusB)){return true;}

  // Check for collision in future
  if(!VelocityObstacle(A,VA,B,VB,radiusA,radiusB).IsInside(A+VA)){return false;}
  
  // If we got here, we have established that a collision will occur
  // if the agents continue indefinitely. However, we can now check
  // the end of the overlapping interval to see if the collision is
  // still in the future. If so, no collision has occurred in the interval.
  double duration(std::min(endTimeB,endTimeA)-startTimeA);
  A+=VA*duration;
  B+=VB*duration;
  
  // Check for immediate collision at end of time interval
  if(VelocityObstacle::AgentOverlap(A,B,radiusA,radiusB)){return true;}

  // Finally, if the collision is still in the future we're ok.
  return !VelocityObstacle(A,VA,B,VB,radiusA,radiusB).IsInside(A+VA);
}

// Detect whether collision is occurring or will occur between 2 agents
// placed at pi and pj with velocity and radius.
// Return time of collision or zero otherwise
double collisionImminent(Vector2D A, Vector2D const& VA, double radiusA, double startTimeA, double endTimeA, Vector2D B, Vector2D const& VB, double radiusB, double startTimeB, double endTimeB){
  // check for time overlap
  if(fgreater(startTimeA-radiusA,endTimeB)||fgreater(startTimeB-radiusB,endTimeA)||fequal(startTimeA,endTimeA)||fequal(startTimeB,endTimeB)){return 0;}

  if(A==B&&VA==VB&&((VA.x==0&&VA.y==0)||(VB.x==0&&VB.y==0))){
    if(fgreater(startTimeA,endTimeB)||fgreater(startTimeB,endTimeA)){return 0;}
    else{return std::max(startTimeA,startTimeB)-TOLERANCE;}
  }

  if(fgreater(startTimeB,startTimeA)){
    // Move A to the same time instant as B
    A+=VA*(startTimeB-startTimeA);
    startTimeA=startTimeB;
  }else if(fless(startTimeB,startTimeA)){
    B+=VB*(startTimeA-startTimeB);
    startTimeB=startTimeA;
  }
  //if(fequal(startTimeA,endTimeA)||fequal(startTimeB,endTimeB)){return 0;}

  // Assume an open interval just at the edge of the agents...
  double r(radiusA+radiusB-2*TOLERANCE); // Combined radius
  Vector2D w(B-A);
  double c(w.sq()-r*r);
  if(fless(c,0.0)){return startTimeA-TOLERANCE;} // Agents are currently colliding

  // Use the quadratic formula to detect nearest collision (if any)
  Vector2D v(VA-VB);
  double a(v.sq());
  double b(w*v);

  double dscr(b*b-a*c);
  if(fleq(dscr,0)){ return 0; }

  double ctime((b-sqrt(dscr))/a); // Collision time
  if(fless(ctime,0)){ return 0; }

  // Collision will occur if collision time is before the end of the shortest segment
  return fleq(ctime,std::min(endTimeB,endTimeA)-startTimeA)?ctime+startTimeA:0;
}

void get2DSuccessors(Vector2D const& c, std::vector<Vector2D>& s, unsigned conn){
  s.reserve((conn*2+1)*(conn*2+1));
  for(int x(c.x-conn); x<conn*2+1+c.x; ++x)
    for(int y(c.y-conn); y<conn*2+1+c.y; ++y)
      s.push_back(Vector2D(x,y));
}

void get3DSuccessors(Vector3D const& c, std::vector<Vector3D>& s, unsigned conn){
  s.reserve((conn*2+1)*(conn*2+1)*(conn*2+1));
  for(int x(c.x-conn); x<conn*2+1+c.x; ++x)
    for(int y(c.y-conn); y<conn*2+1+c.y; ++y)
      for(int z(c.z-conn); z<conn*2+1+c.z; ++z)
        s.push_back(Vector3D(x,y,z));
}

unsigned index9(Vector2D const& s1, Vector2D d1, Vector2D s2, Vector2D d2){
  // Translate d1 and s2 relative to s1
  d1.x-=s1.x-1;
  d1.y-=s1.y-1;
  // Translate d2 relative to s2
  d2.x-=s2.x-1;
  d2.y-=s2.y-1;
  s2.x-=s1.x-1;
  s2.y-=s1.y-1;
  return d1.y*3+d1.x + 9*(s2.y*3+s2.x) + 81*(d2.y*3+d2.x);
}

unsigned index25(Vector2D const& s1, Vector2D d1, Vector2D s2, Vector2D d2){
  // Translate d1 and s2 relative to s1
  d1.x-=s1.x-2;
  d1.y-=s1.y-2;
  // Translate d2 relative to s2
  d2.x-=s2.x-2;
  d2.y-=s2.y-2;
  s2.x-=s1.x-2;
  s2.y-=s1.y-2;
  return d1.y*5+d1.x + 25*(s2.y*5+s2.x) + 625*(d2.y*5+d2.x);
}

unsigned index49(Vector2D const& s1, Vector2D d1, Vector2D s2, Vector2D d2){
  // Translate d1 and s2 relative to s1
  d1.x-=s1.x-3;
  d1.y-=s1.y-3;
  d2.x-=s2.x-3;
  d2.y-=s2.y-3;
  s2.x-=s1.x-3;
  s2.y-=s1.y-3;
  return d1.y*7+d1.x + 49*(s2.y*7+s2.x) + 2401*(d2.y*7+d2.x);
}

unsigned index27(Vector3D const& s1, Vector3D d1, Vector3D s2, Vector3D d2){
  // Translate d1 and s2 relative to s1
  d1.x-=s1.x-1;
  d1.y-=s1.y-1;
  d1.z-=s1.z-1;
  d2.x-=s2.x-1;
  d2.y-=s2.y-1;
  d2.z-=s2.z-1;
  s2.x-=s1.x-1;
  s2.y-=s1.y-1;
  s2.z-=s1.z-1;
  return d1.z*9+d1.y*3+d1.x + 27*(s2.z*9+s2.y*3+s2.x) + 729*(d2.z*9+d2.y*3+d2.x);
}

unsigned index125(Vector3D const& s1, Vector3D d1, Vector3D s2, Vector3D d2){
  // Translate d1 and s2 relative to s1
  d1.x-=s1.x-2;
  d1.y-=s1.y-2;
  d1.z-=s1.z-2;
  d2.x-=s2.x-2;
  d2.y-=s2.y-2;
  d2.z-=s2.z-2;
  s2.x-=s1.x-2;
  s2.y-=s1.y-2;
  s2.z-=s1.z-2;
  return d1.z*25+d1.y*5+d1.x + 125*(s2.z*25+s2.y*3+s2.x) + 15625*(d2.z*25+d2.y*5+d2.x);
}


void fillArray(unsigned* bitarray,unsigned (*index)(Vector2D const&,Vector2D, Vector2D,Vector2D),unsigned conn,double radius){
  std::vector<Vector2D> n;
  Vector2D center(6,6);
  get2DSuccessors(center,n,conn);
  for(auto const& m:n){
    for(auto const& o:n){
      std::vector<Vector2D> p;
      get2DSuccessors(o,p,conn);
      for(auto const& q:p){
        if(Util::fatLinesIntersect(center,m,radius,o,q,radius)){
          set(bitarray,(*index)(center,m,o,q));
        }
      }
    }
  }
}

void fillArray(unsigned* bitarray,unsigned (*index)(Vector3D const&,Vector3D, Vector3D,Vector3D),unsigned conn, double radius){
  std::vector<Vector3D> n;
  Vector3D center(6,6,6);
  get3DSuccessors(center,n,conn);
  for(auto const& m:n){
    for(auto const& o:n){
      std::vector<Vector3D> p;
      get3DSuccessors(o,p,conn);
      for(auto const& q:p){
        if(Util::fatLinesIntersect(center,m,radius,o,q,radius)){
          set(bitarray,(*index)(center,m,o,q));
        }
      }
    }
  }
}

// Check for collision between entities moving from A1 to A2 and B1 to B2
// Speed is optional. If provided, should be in grids per unit time; time should also be pre adjusted to reflect speed.
bool collisionCheck(TemporalVector const& A1, TemporalVector const& A2, TemporalVector const& B1, TemporalVector const& B2, double radiusA, double radiusB, double speedA, double speedB){
  unsigned dim(std::max(std::max(fabs(A1.x-A2.x),fabs(B1.x-B2.x)),std::max(fabs(A1.y-A2.y),fabs(B1.y-B2.y))));
  unsigned ssx(fabs(A1.x-B1.x));
  unsigned sdx(fabs(A1.x-B2.x));
  unsigned ssy(fabs(A1.y-B1.y));
  unsigned sdy(fabs(A1.y-B2.y));

  switch(dim){
    case 0:
    case 1:
      if(std::max(radiusA,radiusB)>.25){
        static unsigned bitarray9r_5[9*9*9/WORD_BITS+1];
        if(!bitarray9r_5[0]){
          fillArray(bitarray9r_5,*index9,1,.5);
        }

        if(ssx<2 && ssy<2 && sdx <3 && sdy<3){
          if(!get(bitarray9r_5,index9(A1,A2,B1,B2)))
             return false;
        }else if(sdx<2 && sdy<2 && ssx <3 && ssy<3){
          if(!get(bitarray9r_5,index9(A1,A2,B2,B1)))
             return false;
        }else{return false;}
      }else{
        static unsigned bitarray9r_25[9*9*9/WORD_BITS+1];
        if(!bitarray9r_25[0]){
          fillArray(bitarray9r_25,*index9,1,.25);
        }

        if(ssx<2 && ssy<2 && sdx <3 && sdy<3){
          if(!get(bitarray9r_25,index9(A1,A2,B1,B2)))
             return false;
        }else if(sdx<2 && sdy<2 && ssx <3 && ssy<3){
          if(!get(bitarray9r_25,index9(A1,A2,B2,B1)))
             return false;
        }else{return false;}
      }
      break;
    case 2:
      if(std::max(radiusA,radiusB)>.25){
        static unsigned bitarray25r_5[25*25*25/WORD_BITS+1];
        if(!bitarray25r_5[0]){
          fillArray(bitarray25r_5,*index25,2,.5);
        }

        if(ssx<3 && ssy<3 && sdx <5 && sdy<5){
          if(!get(bitarray25r_5,index25(A1,A2,B1,B2)))
             return false;
        }else if(sdx<3 && sdy<3 && ssx <5 && ssy<5){
          if(!get(bitarray25r_5,index25(A1,A2,B2,B1)))
             return false;
        }else{return false;}
      }else{
        static unsigned bitarray25r_25[25*25*25/WORD_BITS+1];
        if(!bitarray25r_25[0]){
          fillArray(bitarray25r_25,*index25,2,.25);
        }

        if(ssx<3 && ssy<3 && sdx <5 && sdy<5){
          if(!get(bitarray25r_25,index25(A1,A2,B1,B2)))
             return false;
        }else if(sdx<3 && sdy<3 && ssx <5 && ssy<5){
          if(!get(bitarray25r_25,index25(A1,A2,B2,B1)))
             return false;
        }else{return false;}
      }
      break;
    case 3:
      if(std::max(radiusA,radiusB)>.25){
        static unsigned bitarray49r_5[49*49*49/WORD_BITS+1];
        if(!bitarray49r_5[0]){
          fillArray(bitarray49r_5,*index49,3,.5);
        }

        if(ssx<4 && ssy<4 && sdx <7 && sdy<7){
          if(!get(bitarray49r_5,index49(A1,A2,B1,B2)))
             return false;
        }else if(sdx<4 && sdy<4 && ssx <7 && ssy<7){
          if(!get(bitarray49r_5,index49(A1,A2,B2,B1)))
             return false;
        }else{return false;}
      }else{
        static unsigned bitarray49r_25[49*49*49/WORD_BITS+1];
        if(!get(bitarray49r_25,CENTER_IDX49)){
          fillArray(bitarray49r_25,*index49,3,.25);
        }

        if(ssx<4 && ssy<4 && sdx <7 && sdy<7){
          if(!get(bitarray49r_25,index49(A1,A2,B1,B2)))
             return false;
        }else if(sdx<4 && sdy<4 && ssx <7 && ssy<7){
          if(!get(bitarray49r_25,index49(A1,A2,B2,B1)))
             return false;
        }else{return false;}
      }
      break;
    default:
      break;
  };
  Vector2D VA(A2-A1);
  VA.Normalize();
  VA*=speedA;
  Vector2D VB(B2-B1);
  VB.Normalize();
  VB*=speedA;
  return collisionImminent(A1,VA,radiusA,A1.t,A2.t,B1,VB,radiusB?radiusB:radiusA,B1.t,B2.t);
}

double collisionTime(Vector3D A, Vector3D const& VA, double radiusA, double startTimeA, double endTimeA, Vector3D B, Vector3D const& VB, double radiusB, double startTimeB, double endTimeB){
  // check for time overlap
  if(fgreater(startTimeA-radiusA,endTimeB)||fgreater(startTimeB-radiusB,endTimeA)||fequal(startTimeA,endTimeA)||fequal(startTimeB,endTimeB)){return 0;}

  if(A==B&&VA==VB&&((VA.x==0&&VA.y==0)||(VB.x==0&&VB.y==0))){
    if(fgreater(startTimeA,endTimeB)||fgreater(startTimeB,endTimeA)){return 0;}
    else{return std::max(startTimeA,startTimeB)-TOLERANCE;}
  }

  if(fgreater(startTimeB,startTimeA)){
    // Move A to the same time instant as B
    A+=VA*(startTimeB-startTimeA);
    startTimeA=startTimeB;
  }else if(fless(startTimeB,startTimeA)){
    B+=VB*(startTimeA-startTimeB);
    startTimeB=startTimeA;
  }
  //if(fequal(startTimeA,endTimeA)||fequal(startTimeB,endTimeB)){return 0;}

  // Assume an open interval just at the edge of the agents...
  double r(radiusA+radiusB-2*TOLERANCE); // Combined radius
  Vector3D w(B-A);
  double c(w.sq()-r*r);
  if(fless(c,0)){return startTimeA-TOLERANCE;} // Agents are currently colliding

  // Use the quadratic formula to detect nearest collision (if any)
  Vector3D v(VA-VB);
  double a(v.sq());
  double b(w*v);

  double dscr(b*b-a*c);
  if(fleq(dscr,0)){ return 0; }

  double ctime((b-sqrt(dscr))/a); // Collision time
  if(fless(ctime,0)){ return 0; }

  // Collision will occur if collision time is before the end of the shortest segment
  return fleq(ctime,std::min(endTimeB,endTimeA)-startTimeA)?ctime+startTimeA:0;
}

bool collisionImminent(Vector3D A, Vector3D const& VA, double radiusA, double startTimeA, double endTimeA, Vector3D B, Vector3D const& VB, double radiusB, double startTimeB, double endTimeB){
  // assume time overlap
  //if(fgreater(startTimeA-radiusA,endTimeB)||fgreater(startTimeB-radiusB,endTimeA)||fequal(startTimeA,endTimeA)||fequal(startTimeB,endTimeB)){return false;}

  if(A==B&&VA==VB&&(VA.x==0&&VA.y==0)){
    if(fgreater(startTimeA,endTimeB)||fgreater(startTimeB,endTimeA)){return false;}
    else{return true;}
  }

  if(fgreater(startTimeB,startTimeA)){
    // Move A to the same time instant as B
    A+=VA*(startTimeB-startTimeA);
    startTimeA=startTimeB;
  }else if(fless(startTimeB,startTimeA)){
    B+=VB*(startTimeA-startTimeB);
    startTimeB=startTimeA;
  }
  //if(fequal(startTimeA,endTimeA)||fequal(startTimeB,endTimeB)){return 0;}

  // Assume an open interval just at the edge of the agents...
  double r(radiusA+radiusB-2*TOLERANCE); // Combined radius
  Vector3D w(B-A);
  double c(w.sq()-r*r);
  if(fless(c,0)){return true;} // Agents are currently colliding

  // Use the quadratic formula to detect nearest collision (if any)
  Vector3D v(VA-VB);
  double a(v.sq());
  double b(w*v);

  double dscr(b*b-a*c);
  if(fleq(dscr,0)){ return false; }

  double ctime((b-sqrt(dscr))/a); // Collision time
  if(fless(ctime,0)){ return false; }

  // Collision will occur if collision time is before the end of the shortest segment
  return fleq(ctime,std::min(endTimeB,endTimeA)-startTimeA);
}


double collisionCheck3DSlow(TemporalVector3D const& A1, TemporalVector3D const& A2, TemporalVector3D const& B1, TemporalVector3D const& B2, double radiusA, double radiusB, double speedA, double speedB){
  Vector3D VA(A2-A1);
  VA.Normalize();
  VA*=speedA;
  Vector3D VB(B2-B1);
  VB.Normalize();
  VB*=speedA;
  return collisionImminent(A1,VA,radiusA,A1.t,A2.t,B1,VB,radiusB?radiusB:radiusA,B1.t,B2.t);
}

// Check for collision between entities moving from A1 to A2 and B1 to B2
// Speed is optional. If provided, should be in grids per unit time; time should also be pre adjusted to reflect speed.
double collisionTime3D(TemporalVector3D const& A1, TemporalVector3D const& A2, TemporalVector3D const& B1, TemporalVector3D const& B2, double radiusA, double radiusB, double speedA, double speedB){
  unsigned sdx(fabs(A1.x-B2.x));
  unsigned sdy(fabs(A1.y-B2.y));
  unsigned sdz(fabs(A1.z-B2.z));
  // Same edge in reverse?
  if(sdx==0&&sdy==0&&sdz==0&&B1.x==A2.x&&B1.y==A2.y&&B1.z==A2.z){
    double dist(sqrt(distanceSquared(A1,B1))-(A1.t>B1.t?(A1.t-B1.t)*speedA:(B1.t-A1.t)*speedB));
    double radius((radiusA*speedA+radiusB*speedB));
    double speed(speedA+speedB);
    double ctime(std::max(A1.t,B1.t)+(dist-radius)/speed);
    return ctime;
  }

  unsigned ssx(fabs(A1.x-B1.x));
  unsigned ssy(fabs(A1.y-B1.y));
  unsigned ssz(fabs(A1.z-B1.z));
  // Same start? 
  double diam(radiusA+radiusB);
  if(ssx==0 && ssy==0 && ssz==0){
    if(A1.t==B1.t){
      return A1.t;
    }
    double tdist(fgreater(A1.t,B1.t)?speedA*(A1.t-B1.t):speedB*(B1.t-A1.t));
    if(tdist<diam){
      return std::min(A1.t,B1.t);
    }
  }
  //same end?
  if(A2.x==B2.x && A2.y==B2.y && A2.z==B2.z){
    if(A2.t==B2.t){
      return A2.t;
    }
    double tdist(fgreater(A2.t,B2.t)?speedA*(A2.t-B2.t):speedB*(B2.t-A2.t));
    // Distance based on time offset
    if(tdist<diam){
      // collides at min-tdist
      return std::min(A1.t,B1.t)-tdist;
    }
  }


  unsigned dim(std::max(std::max(std::max(fabs(A1.x-A2.x),fabs(B1.x-B2.x)),std::max(fabs(A1.y-A2.y),fabs(B1.y-B2.y))),std::max(fabs(A1.z-A2.z),fabs(B1.z-B2.z))));

  switch(dim){
    case 0:
    case 1:
        if(ssx<2 && ssy<2 && ssz<2 && sdx<3 && sdy<3 && sdz<3){
        }else if(sdx<2 && sdy<2 && sdz<2 && ssx<3 && ssy<3 && ssz<3){
        }else{return 0;}
      break;
    case 2:
        if(ssx<3 && ssy<3 && ssz<3 && sdx<5 && sdy<5 && sdz<5){
        }else if(sdx<3 && sdy<3 && sdz<3 && ssx<5 && ssy<5 && ssz<5){
        }else{return 0;}
      break;
    default:
      break;
  };
  Vector3D VA(A2-A1);
  VA.Normalize();
  VA*=speedA;
  Vector3D VB(B2-B1);
  VB.Normalize();
  VB*=speedA;
  return collisionImminent(A1,VA,radiusA,A1.t,A2.t,B1,VB,radiusB?radiusB:radiusA,B1.t,B2.t);
}

// Check for collision between entities moving from A1 to A2 and B1 to B2
// Speed is optional. If provided, should be in grids per unit time; time should also be pre adjusted to reflect speed.
bool collisionCheck3D(TemporalVector3D const& A1, TemporalVector3D const& A2, TemporalVector3D const& B1, TemporalVector3D const& B2, double radiusA, double radiusB, double speedA, double speedB){
  unsigned sdx(fabs(A1.x-B2.x));
  unsigned sdy(fabs(A1.y-B2.y));
  unsigned sdz(fabs(A1.z-B2.z));
  // Same edge in reverse?
  if(sdx==0&&sdy==0&&sdz==0&&B1.x==A2.x&&B1.y==A2.y&&B1.z==A2.z){
    return true;
  }

  unsigned ssx(fabs(A1.x-B1.x));
  unsigned ssy(fabs(A1.y-B1.y));
  unsigned ssz(fabs(A1.z-B1.z));
  // Same start? 
  double diam(radiusA+radiusB);
  if(ssx==0 && ssy==0 && ssz==0){
    if(A1.t==B1.t){
      return true;
    }
    double tdist(fgreater(A1.t,B1.t)?speedA*(A1.t-B1.t):speedB*(B1.t-A1.t));
    if(tdist<diam){
      return true;
    }
  }
  //same end?
  if(A2.x==B2.x && A2.y==B2.y && A2.z==B2.z){
    if(A2.t==B2.t){
      return true;
    }
    double tdist(fgreater(A2.t,B2.t)?speedA*(A2.t-B2.t):speedB*(B2.t-A2.t));
    // Distance based on time offset
    if(tdist<diam){
      // collides at min-tdist
      return true;
    }
  }


  unsigned dim(std::max(std::max(std::max(fabs(A1.x-A2.x),fabs(B1.x-B2.x)),std::max(fabs(A1.y-A2.y),fabs(B1.y-B2.y))),std::max(fabs(A1.z-A2.z),fabs(B1.z-B2.z))));

  switch(dim){
    case 0:
    case 1:
        if(ssx<2 && ssy<2 && ssz<2 && sdx<3 && sdy<3 && sdz<3){
        }else if(sdx<2 && sdy<2 && sdz<2 && ssx<3 && ssy<3 && ssz<3){
        }else{return false;}
      break;
    case 2:
        if(ssx<3 && ssy<3 && ssz<3 && sdx<5 && sdy<5 && sdz<5){
        }else if(sdx<3 && sdy<3 && sdz<3 && ssx<5 && ssy<5 && ssz<5){
        }else{return false;}
      break;
    default:
      break;
  };
  Vector3D VA(A2-A1);
  VA.Normalize();
  VA*=speedA;
  Vector3D VB(B2-B1);
  VB.Normalize();
  VB*=speedA;
  return collisionImminent(A1,VA,radiusA,A1.t,A2.t,B1,VB,radiusB?radiusB:radiusA,B1.t,B2.t);
}

std::pair<double,double> collisionInterval3D(TemporalVector3D const& A1, TemporalVector3D const& A2, TemporalVector3D const& B1, TemporalVector3D const& B2, double radiusA, double radiusB, double speedA, double speedB){
  unsigned sdx(fabs(A1.x-B2.x));
  unsigned sdy(fabs(A1.y-B2.y));
  unsigned sdz(fabs(A1.z-B2.z));
  // Same edge in reverse?
  if(sdx==0&&sdy==0&&sdz==0&&B1.x==A2.x&&B1.y==A2.y&&B1.z==A2.z){
    double dist(sqrt(distanceSquared(A1,B1))-(A1.t>B1.t?(A1.t-B1.t)*speedA:(B1.t-A1.t)*speedB));
    double radius((radiusA*speedA+radiusB*speedB));
    double speed(speedA+speedB);
    double ctime(std::max(A1.t,B1.t)+(dist-radius)/speed);
    return {ctime,ctime+radius/speed};
  }

  // Same start && same end?
  if(A1.x==B1.x && A1.y==B1.y && A1.z==B1.z
      && A2.x==B2.x && A2.y==B2.y && A2.z==B2.z){
    double radius((radiusA*speedA+radiusB*speedB));
    // Distance based on time offset
    double tdist(fgreater(A1.t,B1.t)?speedA*(A1.t-B1.t):speedB*(B1.t-A1.t));
    if(radius > tdist){
      // immediately colliding
      double begin(std::max(A1.t,B1.t));
      double end(std::min(A2.t,B2.t));
      if(fequal(speedA,speedB)){
        return {begin,end}; // Collision continues for entire duration
      }else if(fgreater(speedA,speedB)){ // A is moving faster
        // How much distance is required?
        double reqd(fgreater(A1.t,B1.t)?tdist-radius:tdist+radius);
        end = std::min(end,begin + reqd*(speedA-speedB));
      }else{ // B is moving faster
        double reqd(fless(A1.t,B1.t)?tdist-radius:tdist+radius);
        end = std::min(end,begin + reqd*(speedB-speedA));
      }
      return {begin,end};
    }
    // Determine if collision is going to happen in the future
  }

  unsigned ssx(fabs(A1.x-B1.x));
  unsigned ssy(fabs(A1.y-B1.y));
  unsigned ssz(fabs(A1.z-B1.z));

  unsigned dim(std::max(std::max(std::max(fabs(A1.x-A2.x),fabs(B1.x-B2.x)),std::max(fabs(A1.y-A2.y),fabs(B1.y-B2.y))),std::max(fabs(A1.z-A2.z),fabs(B1.z-B2.z))));

  switch(dim){
    case 0:
    case 1:
        if(ssx<2 && ssy<2 && ssz<2 && sdx<3 && sdy<3 && sdz<3){
        }else if(sdx<2 && sdy<2 && sdz<2 && ssx<3 && ssy<3 && ssz<3){
        }else{return {0,0};}
      break;
    case 2:
        if(ssx<3 && ssy<3 && ssz<3 && sdx<5 && sdy<5 && sdz<5){
        }else if(sdx<3 && sdy<3 && sdz<3 && ssx<5 && ssy<5 && ssz<5){
        }else{return {0,0};}
      break;
    default:
      break;
  };
  Vector3D VA(A2-A1);
  VA.Normalize();
  VA*=speedA;
  Vector3D VB(B2-B1);
  VB.Normalize();
  VB*=speedA;
  return getCollisionInterval(A1,VA,radiusA,A1.t,A2.t,B1,VB,radiusB?radiusB:radiusA,B1.t,B2.t);
}

// Get collision time between two agents - that is the time that they will "start" colliding.
// Agents have a position, velocity, start time, end time and radius.
// Note: this is analogous to an agent traversing an edge from time "start" to "end" at constant velocity.
// -1 is a seminal value meaning "no collision in the future."
double getCollisionTime(Vector2D A, Vector2D const& VA, double radiusA, double startTimeA, double endTimeA, Vector2D B, Vector2D const& VB, double radiusB, double startTimeB, double endTimeB){
  // check for time overlap
  if(fgreater(startTimeA-radiusA,endTimeB)||fgreater(startTimeB-radiusB,endTimeA)||fequal(startTimeA,endTimeA)||fequal(startTimeB,endTimeB)){return -1;}

  if(A==B&&VA==VB&&((VA.x==0&&VA.y==0)||(VB.x==0&&VB.y==0))){
    if(fgreater(startTimeA,endTimeB)||fgreater(startTimeB,endTimeA)){return -1;}
    else{return std::max(startTimeA,startTimeB);}
  }

  if(fgreater(startTimeB,startTimeA)){
    // Move A forward to the same time instant as B
    A+=VA*(startTimeB-startTimeA);
    startTimeA=startTimeB;
  }else if(fless(startTimeB,startTimeA)){
    B+=VB*(startTimeA-startTimeB);
    startTimeB=startTimeA;
  }
  if(fequal(startTimeA,endTimeA)||fequal(startTimeB,endTimeB)){return -1;}


  // Assume an open interval just at the edge of the agents...
  double r(radiusA+radiusB-2*TOLERANCE); // Combined radius
  Vector2D w(B-A);
  double c(w.sq()-r*r);
  if(fless(c,0.0)){return startTimeA-TOLERANCE;} // Agents are currently colliding

  // Use the quadratic formula to detect nearest collision (if any)
  Vector2D v(VA-VB);
  double a(v.sq());
  double b(w*v);

  double dscr(b*b-a*c);
  if(fleq(dscr,0)){ return -1; } // No collision will occur in the future

  return (b-sqrt(dscr))/a + startTimeA; // Absolute collision time
}

double getCollisionTime(Vector3D A, Vector3D const& VA, double radiusA, double startTimeA, double endTimeA, Vector3D B, Vector3D const& VB, double radiusB, double startTimeB, double endTimeB){
  // check for time overlap
  if(fgreater(startTimeA-radiusA,endTimeB)||fgreater(startTimeB-radiusB,endTimeA)||fequal(startTimeA,endTimeA)||fequal(startTimeB,endTimeB)){return -1;}

  if(A==B&&VA==VB&&((VA.x==0&&VA.y==0)||(VB.x==0&&VB.y==0))){
    if(fgreater(startTimeA,endTimeB)||fgreater(startTimeB,endTimeA)){return -1;}
    else{return std::max(startTimeA,startTimeB);}
  }

  if(fgreater(startTimeB,startTimeA)){
    // Move A forward to the same time instant as B
    A+=VA*(startTimeB-startTimeA);
    startTimeA=startTimeB;
  }else if(fless(startTimeB,startTimeA)){
    B+=VB*(startTimeA-startTimeB);
    startTimeB=startTimeA;
  }
  if(fequal(startTimeA,endTimeA)||fequal(startTimeB,endTimeB)){return -1;}

  // Assume an open interval just at the edge of the agents...
  double r(radiusA+radiusB-2*TOLERANCE); // Combined radius
  Vector3D w(B-A);
  double c(w.sq()-r*r);
  if(fless(c,0.0)){return startTimeA-TOLERANCE;} // Agents are currently colliding

  // Use the quadratic formula to detect nearest collision (if any)
  Vector3D v(VA-VB);
  double a(v.sq());
  double b(w*v);

  double dscr(b*b-a*c);
  if(fleq(dscr,0)){ return -1; } // No collision will occur in the future

  return (b-sqrt(dscr))/a + startTimeA; // Absolute collision time
}

// Get continuous collision interval for two agents (return -1,-1 if such does not exist)
std::pair<double,double> getCollisionInterval(Vector3D A, Vector3D const& VA, double radiusA, double startTimeA, double endTimeA, Vector3D B, Vector3D const& VB, double radiusB, double startTimeB, double endTimeB){
  // check for time overlap
  if(fgreater(startTimeA-radiusA,endTimeB)||fgreater(startTimeB-radiusB,endTimeA)||fequal(startTimeA,endTimeA)||fequal(startTimeB,endTimeB)){return {-1,-1};}

  // Detect a wait action...
  if(A==B&&VA==VB&&((VA.x==0&&VA.y==0)||(VB.x==0&&VB.y==0))){
    if(fgreater(startTimeA,endTimeB)||fgreater(startTimeB,endTimeA)){return {-1,-1};}
    else{return std::make_pair(std::max(startTimeA,startTimeB),std::max(startTimeA,startTimeB)+radiusA+radiusB);}
  }

  if(fgreater(startTimeB,startTimeA)){
    // Move A forward to the same time instant as B
    A+=VA*(startTimeB-startTimeA);
    startTimeA=startTimeB;
  }else if(fless(startTimeB,startTimeA)){
    B+=VB*(startTimeA-startTimeB);
    startTimeB=startTimeA;
  }
  if(fequal(startTimeA,endTimeA)||fequal(startTimeB,endTimeB)){return {-1,-1};}

  // Assume an open interval just at the edge of the agents...
  double r(radiusA+radiusB-2*TOLERANCE); // Combined radius
  Vector3D w(B-A);
  double c(w.sq()-r*r);
  if(fless(c,0.0)){return {startTimeA-TOLERANCE,startTimeA+r+TOLERANCE*3};} // Agents are currently colliding

  // Use the quadratic formula to detect nearest collision (if any)
  Vector3D v(VA-VB);
  double a(v.sq());
  double b(w*v);

  double dscr(b*b-a*c);
  if(fleq(dscr,0)){ return {-1,-1}; } // No collision will occur in the future
  double sqrtd(sqrt(dscr));
  return std::make_pair((b-sqrtd)/a + startTimeA,(b+sqrtd)/a + startTimeA); // Absolute collision time
}

std::pair<double,double> getCollisionInterval(Vector2D A, Vector2D const& VA, double radiusA, double startTimeA, double endTimeA, Vector2D B, Vector2D const& VB, double radiusB, double startTimeB, double endTimeB){
  // check for time overlap
  if(fgreater(startTimeA-radiusA,endTimeB)||fgreater(startTimeB-radiusB,endTimeA)||fequal(startTimeA,endTimeA)||fequal(startTimeB,endTimeB)){return {-1,-1};}

  // Detect a wait action...
  if(A==B&&VA==VB&&((VA.x==0&&VA.y==0)||(VB.x==0&&VB.y==0))){
    if(fgreater(startTimeA,endTimeB)||fgreater(startTimeB,endTimeA)){return {-1,-1};}
    else{return std::make_pair(std::max(startTimeA,startTimeB),std::max(startTimeA,startTimeB)+radiusA+radiusB);}
  }

  if(fgreater(startTimeB,startTimeA)){
    // Move A forward to the same time instant as B
    A+=VA*(startTimeB-startTimeA);
    startTimeA=startTimeB;
  }else if(fless(startTimeB,startTimeA)){
    B+=VB*(startTimeA-startTimeB);
    startTimeB=startTimeA;
  }
  if(fequal(startTimeA,endTimeA)||fequal(startTimeB,endTimeB)){return {-1,-1};}

  // Assume an open interval just at the edge of the agents...
  double r(radiusA+radiusB-2*TOLERANCE); // Combined radius
  Vector3D w(B-A);
  double c(w.sq()-r*r);
  if(fless(c,0.0)){return {startTimeA-TOLERANCE,startTimeA+r+TOLERANCE*3};} // Agents are currently colliding

  // Use the quadratic formula to detect nearest collision (if any)
  Vector3D v(VA-VB);
  double a(v.sq());
  double b(w*v);

  double dscr(b*b-a*c);
  if(fleq(dscr,0)){ return {-1,-1}; } // No collision will occur in the future
  double sqrtd(sqrt(dscr));
  return std::make_pair((b-sqrtd)/a + startTimeA,(b+sqrtd)/a + startTimeA); // Absolute collision time
}
