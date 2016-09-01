/**
 * Copyright (c) 2015 Carnegie Mellon University, Daniel Maturana <dimatura@cmu.edu>
 *
 * For License information please see the LICENSE file in the root directory.
 *
 */

#ifndef SPARSE_ARRAY3_HPP_LWA5VI1M
#define SPARSE_ARRAY3_HPP_LWA5VI1M

//#include <unordered_map>
#include <boost/unordered_map.hpp>

#include <boost/shared_ptr.hpp>

#include <pcl_util/point_types.hpp>

#include "scrollgrid/grid_types.hpp"

namespace ca
{

/**
 * Assumes input is a decent hash key to begin with
 * TODO should the hashing be done by scrollgrid?
 * TODO does tr1 use identity as a hash for uint64_t?
 */
struct TrivialHash {
  size_t operator()(uint64_t key) const {
    return static_cast<size_t>(key);
  }
};

template <class CellT>
class SparseArray {
public:
  typedef CellT CellType;

  typedef boost::shared_ptr<SparseArray> Ptr;
  typedef boost::shared_ptr<const SparseArray> ConstPtr;

  typedef typename boost::unordered_map<uint64_t, CellType, TrivialHash> MapType;
  //typedef typename boost::container::flat_map<uint64_t, CellType> MapType;

  typedef typename MapType::iterator iterator;
  typedef typename MapType::const_iterator const_iterator;

public:
  SparseArray() { }

  SparseArray(const SparseArray& other) = delete;

  SparseArray& operator=(const SparseArray& other) = delete;

  virtual ~SparseArray() { }

public:

  CellType& operator[](uint64_t hash_ix) {
    //ROS_INFO_STREAM("hash_map_.load_factor() = " << hash_map_.load_factor());
    return hash_map_[hash_ix];
  }

  const CellType& operator[](uint64_t hash_ix) const {
    return hash_map_[hash_ix];
  }

  iterator find(uint64_t k) {
    return hash_map_.find(k);
  }

public:

  void reset() {
    hash_map_.clear();
  }

  void reset(size_t expected_size) {
    hash_map_.clear();
    hash_map_.resize(expected_size);
  }

  size_t size() const { return hash_map_.size(); }

  void clear_no_resize() {
    // does not recover memory for hashtable buckets, only items
    // saves time when we reuse map
    hash_map_.clear_no_resize();
  }

  void erase( iterator pos ) {
    hash_map_.erase(pos);
  }

  iterator begin() { return hash_map_.begin(); }
  const_iterator begin() const { return hash_map_.cbegin(); }

  iterator end() { return hash_map_.end(); }
  const_iterator end() const { return hash_map_.cend(); }

  // interesting: clear_deleted

private:
  MapType hash_map_;

};

} /* ca */

#endif /* end of include guard: SPARSE_ARRAY3_HPP_LWA5VI1M */
