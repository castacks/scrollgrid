#ifndef GRID_TYPES_HPP_HJ46RUAT
#define GRID_TYPES_HPP_HJ46RUAT

#include <stdint.h>

#include <Eigen/Core>

namespace ca
{

// a compact bitwise representation of ijk coordinates
typedef uint64_t hash_ix_t;
typedef Eigen::Matrix<hash_ix_t, Eigen::Dynamic, 1> HashIxVector;

// indexes dense grid arrays in ram. we use signed to avoid arithmetic bugs.
typedef int64_t mem_ix_t;
typedef Eigen::Matrix<mem_ix_t, Eigen::Dynamic, 1> MemIxVector;

typedef int32_t grid_ix_t;
typedef Eigen::Matrix<grid_ix_t, 2, 1> Vec2Ix;
typedef Eigen::Matrix<grid_ix_t, 3, 1> Vec3Ix;
typedef Eigen::Matrix<grid_ix_t, 4, 1> Vec4Ix;
// note that we are using an eigen-friendly representation
typedef Eigen::Matrix<grid_ix_t, 2, Eigen::Dynamic> Mat2Ix;
typedef Eigen::Matrix<grid_ix_t, 3, Eigen::Dynamic> Mat3Ix;
typedef Eigen::Matrix<grid_ix_t, 4, Eigen::Dynamic> Mat4Ix;

} /* ca */

#endif /* end of include guard: GRID_TYPES_HPP_HJ46RUAT */
