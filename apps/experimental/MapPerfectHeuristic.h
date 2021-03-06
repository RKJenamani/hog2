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
#include "MapInterface.h"
#include "Heuristic.h"
#include "TemplateAStar.h"

// Computes a perfect heuristic given a goal state.
// Note that this only works assuming that agents are
// holonomic (otherwise a reverse search becomes imperative)
#define INF 9999999

template<typename state, typename action>
class MapPerfectHeuristic: public Heuristic<state> {

  public:
    // constructor
    MapPerfectHeuristic( MapInterface *_m, SearchEnvironment<state,action>* _e): m(_m),e(_e),elapsed(0.0),loaded(false),list(m->GetMapHeight()*m->GetMapWidth()){}

    double HCost( const state &s1, const state &s2 ) const {
      if(!loaded){
        goal=s2;
        Timer tmr;
        tmr.StartTimer();
        TemplateAStar<state,action,SearchEnvironment<state,action>> astar;
        //std::cout << "Loading heuristic\n";
        astar.SetVerbose(false);
        astar.SetHeuristic(new ZeroHeuristic<state>);
        astar.SetStopAfterGoal(false); // Search the entire space
        // Now perform a search to get all costs
        // NOTE: This should be a reverse-search, but our agents are holonomic
        // so the costs forward are the same as the costs backward
        std::vector<state> path;
        astar.GetPath(e,goal,s1,path);
        for(int w(0); w<m->GetMapWidth(); ++w){
          for(int h(0); h<m->GetMapHeight(); ++h){
            uint64_t h1(e->GetStateHash({w,h}));
            uint64_t id1;
            if(kClosedList==astar.GetOpenList()->Lookup(h1,id1))
              list[w*m->GetMapHeight()+h]=astar.GetOpenList()->Lookat(id1).g;
            else
              list[w*m->GetMapHeight()+h]=INF;
          }
        }
        loaded=true;
        elapsed+=tmr.EndTimer();
      }
      assert(s2.sameLoc(goal));
      return list[s1.x*m->GetMapHeight()+s1.y];
    }
    mutable double elapsed;

  private:
    MapInterface *m;
    SearchEnvironment<state,action> *e;
    mutable bool loaded;
    mutable std::vector<float> list;
    mutable state goal;

};

