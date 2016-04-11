/**
 * @author  Daniel Maturana
 * @year    2015
 *
 * @attention Copyright (c) 2015
 * @attention Carnegie Mellon University
 * @attention All rights reserved.
 *
 **@=*/


#ifndef RAYCASTING_HPP_OLVFBMND
#define RAYCASTING_HPP_OLVFBMND

#include <Eigen/Core>

#include <pcl_util/point_types.hpp>

#include "scrollgrid/grid_types.hpp"
#include "scrollgrid/box.hpp"
#include "scrollgrid/ray.hpp"
#include "scrollgrid/scrollgrid3.hpp"
#include "scrollgrid/dense_array3.hpp"

namespace ca
{

/**
 * Axis-aligned bounding box intersection test.
 * Reference:
 * An Efficient and Robust Ray–Box Intersection Algorithm, Williams et al. 2004
 * tmin and tmax are updated
 */
template<typename Scalar>
bool aabb_ray_intersect(const ca::scrollgrid::Box<Scalar, 3>& box,
                        ca::scrollgrid::Ray3<Scalar> &r) {
  Scalar tmin = (box.bound(   boost::get<0>(r.sign) ).x() - r.origin.x()) * r.invdir.x();
  Scalar tmax = (box.bound( 1-boost::get<0>(r.sign) ).x() - r.origin.x()) * r.invdir.x();

  Scalar tymin = (box.bound(  boost::get<1>(r.sign) ).y() - r.origin.y()) * r.invdir.y();
  Scalar tymax = (box.bound(1-boost::get<1>(r.sign) ).y() - r.origin.y()) * r.invdir.y();

  if ((tmin > tymax) || (tymin > tmax)) { return false; }
  if (tymin > tmin) { tmin = tymin; }
  if (tymax < tmax) { tmax = tymax; }

  Scalar tzmin = (box.bound(  boost::get<2>(r.sign)).z() - r.origin.z()) * r.invdir.z();
  Scalar tzmax = (box.bound(1-boost::get<2>(r.sign)).z() - r.origin.z()) * r.invdir.z();

  if ((tmin > tzmax) || (tzmin > tmax)) { return false; }
  if (tzmin > tmin) { tmin = tzmin; }
  if (tzmax < tmax) { tmax = tzmax; }
  if (tmin > r.tmin) { r.tmin = tmin; }
  if (tmax < r.tmax) { r.tmax = tmax; }
  return true;
}

/**
 * Trace a straight line from start_pos to end_pos.
 * At each step fun(i, j, k) is called.
 *
 * NOTE start_pos and end_pos should be inside the grid
 *
 * Reference: graphics gems article
 * TODO consider DDA-type raytracing.
 */
template<class TraceFunctor>
void bresenham_trace(const Vec3Ix& start_pos,
                     const Vec3Ix& end_pos,
                     const TraceFunctor& fun) {
  // beware: vec3ix are int64_t
  int x = start_pos[0],
      y = start_pos[1],
      z = start_pos[2];
  int dx = end_pos[0] - start_pos[0],
      dy = end_pos[1] - start_pos[1],
      dz = end_pos[2] - start_pos[2];
  int sx, sy, sz;
  //X
  if ( dx>0 ) {
    sx = 1;
  } else if ( dx<0 ) {
    sx = -1;
    dx = -dx;
  } else {
    sx = 0;
  }

  //Y
  if ( dy>0 ) {
    sy = 1;
  } else if ( dy<0 ) {
    sy = -1;
    dy = -dy;
  } else {
    sy = 0;
  }

  //Z
  if ( dz>0 ) {
    sz = 1;
  } else if ( dz<0 ) {
    sz = -1;
    dz = -dz;
  } else {
    sz = 0;
  }

  int ax = 2*dx,
      ay = 2*dy,
      az = 2*dz;

  if ( ( dy <= dx ) && ( dz <= dx ) ) {
    for (int decy=ay-dx, decz=az-dx;
         ;
         x+=sx, decy+=ay, decz+=az) {
      //SetP ( grid,x,y,z,end_pos, atMax, count);
      fun(x, y, z);
      //Bresenham step
      if ( x==end_pos[0] ) break;
      if ( decy>=0 ) {
        decy-=ax;
        y+=sy;
      }
      if ( decz>=0 ) {
        decz-=ax;
        z+=sz;
      }
    }
  } else if ( ( dx <= dy ) && ( dz <= dy ) ) {
    //dy>=dx,dy
    for (int decx=ax-dy,decz=az-dy;
         ;
         y+=sy,decx+=ax,decz+=az ) {
      // SetP ( grid,x,y,z,end_pos, atMax, count);
      fun(x, y, z);
      //Bresenham step
      if ( y==end_pos[1] ) break;
      if ( decx>=0 ) {
        decx-=ay;
        x+=sx;
      }
      if ( decz>=0 ) {
        decz-=ay;
        z+=sz;
      }
    }
  } else if ( ( dx <= dz ) && ( dy <= dz ) ) {
    //dy>=dx,dy
    for (int decx=ax-dz,decy=ay-dz;
         ;
         z+=sz,decx+=ax,decy+=ay ) {
      //SetP ( grid,x,y,z,end_pos, atMax, count);
      fun(x, y, z);
      //Bresenham step
      if ( z==end_pos[2] ) break;
      if ( decx>=0 ) {
        decx-=az;
        x+=sx;
      } if ( decy>=0 ) {
        decy-=az;
        y+=sy;
      }
    }
  }
}

/**
 * Simply increment a counter in densearray3 for each step along the way.
 */
template<class GridScalar, class ArrayScalar>
void bresenham_trace_simple(const Vec3Ix& start_pos,
                            const Vec3Ix& end_pos,
                            const ca::ScrollGrid3<GridScalar>& grid3,
                            ca::DenseArray3<ArrayScalar>& array3
                            ) {
  //int ray_ctr = 0;
  // beware: vec3ix are int64_t
  int x = start_pos[0],
      y = start_pos[1],
      z = start_pos[2];
  int dx = end_pos[0] - start_pos[0],
      dy = end_pos[1] - start_pos[1],
      dz = end_pos[2] - start_pos[2];
  int sx, sy, sz;
  //X
  if ( dx>0 ) {
    sx = 1;
  } else if ( dx<0 ) {
    sx = -1;
    dx = -dx;
  } else {
    sx = 0;
  }

  //Y
  if ( dy>0 ) {
    sy = 1;
  } else if ( dy<0 ) {
    sy = -1;
    dy = -dy;
  } else {
    sy = 0;
  }

  //Z
  if ( dz>0 ) {
    sz = 1;
  } else if ( dz<0 ) {
    sz = -1;
    dz = -dz;
  } else {
    sz = 0;
  }

  int ax = 2*dx,
      ay = 2*dy,
      az = 2*dz;

  if ( ( dy <= dx ) && ( dz <= dx ) ) {
    for (int decy=ay-dx, decz=az-dx;
         ;
         x+=sx, decy+=ay, decz+=az) {
      mem_ix_t mem_ix = grid3.grid_to_mem(x, y, z);
      array3[mem_ix] += 1;
      //array3[mem_ix] = ray_ctr++;
      //Bresenham step
      if ( x==end_pos[0] ) break;
      if ( decy>=0 ) {
        decy-=ax;
        y+=sy;
      }
      if ( decz>=0 ) {
        decz-=ax;
        z+=sz;
      }
    }
  } else if ( ( dx <= dy ) && ( dz <= dy ) ) {
    //dy>=dx,dy
    for (int decx=ax-dy,decz=az-dy;
         ;
         y+=sy,decx+=ax,decz+=az ) {
      mem_ix_t mem_ix = grid3.grid_to_mem(x, y, z);
      array3[mem_ix] += 1;
      //array3[mem_ix] = ray_ctr++;
      //Bresenham step
      if ( y==end_pos[1] ) break;
      if ( decx>=0 ) {
        decx-=ay;
        x+=sx;
      }
      if ( decz>=0 ) {
        decz-=ay;
        z+=sz;
      }
    }
  } else if ( ( dx <= dz ) && ( dy <= dz ) ) {
    //dy>=dx,dy
    for (int decx=ax-dz,decy=ay-dz;
         ;
         z+=sz,decx+=ax,decy+=ay ) {
      grid_ix_t mem_ix = grid3.grid_to_mem(x, y, z);
      array3[mem_ix] += 1;
      //array3[mem_ix] = ray_ctr++;
      //Bresenham step
      if ( z==end_pos[2] ) break;
      if ( decx>=0 ) {
        decx-=az;
        x+=sx;
      } if ( decy>=0 ) {
        decy-=az;
        y+=sy;
      }
    }
  }
}

template<class TraceFunctor>
void bresenham_trace(const Vec2Ix& start_pos,
                     const Vec2Ix& end_pos,
                     const TraceFunctor& fun) {
    int x1 = start_pos[0],
    y1 = start_pos[1],
    x2 = end_pos[0],
    y2 = end_pos[1];

    // Bresenham's line algorithm
    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
    if(steep)
    {
      std::swap(x1, y1);
      std::swap(x2, y2);
    }

    if(x1 > x2)
    {
      std::swap(x1, x2);
      std::swap(y1, y2);
    }

    const float dx = x2 - x1;
    const float dy = fabs(y2 - y1);

    float error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;

    const int maxX = (int)x2;
    bool end_cell=false;

    for(int x=(int)x1; x <= maxX; x++)
    {
      if(steep)
      {
          if(!fun(y,x,end_cell))
              return;
      }
      else
      {
          if(!fun(x,y,end_cell))
              return;
      }

      error -= dy;
      if(error < 0)
      {
          y += ystep;
          error += dx;
      }
    }
}

}

#endif /* end of include guard: RAYCASTING_HPP_OLVFBMND */
