#pragma once

#include "marray.h"


namespace misc {


// Get trilinear interpolated value according to floating point pos, in 3d texture
template <typename ValueType, typename InterpolationType>
ValueType trilinear_interpolate(const marray<ValueType, 3>& texture, const math::Vec3<InterpolationType>& pos) {
  typedef math::Vec3<InterpolationType> PositionType;
  RayAssertDesc(pos.x >= 0 && pos.y >= 0 && pos.z >= 0, "TrilinearInterpolate error; pos below 0");
  RayAssertDesc(pos.x + 1 <= (InterpolationType) texture.dims().x, "TrilinearInterpolate error (x); " << pos.x << " " << texture.dims().x);
  RayAssertDesc(pos.y + 1 <= (InterpolationType) texture.dims().y, "TrilinearInterpolate error (y); " << pos.y << " " << texture.dims().y);
  RayAssertDesc(pos.z + 1 <= (InterpolationType) texture.dims().z, "TrilinearInterpolate error (z); " << pos.z << " " << texture.dims().z);
  // Get positions
  math::Vec3<int> poslow;
  poslow.x = (int)(floor(pos.x));
  poslow.y = (int)(floor(pos.y));
  poslow.z = (int)(floor(pos.z));
  auto poshigh = poslow + LinAlgPoint3<int>(1,1,1);
  if (poshigh.x >= texture.dims().x) poshigh.x = poslow.x;
  if (poshigh.y >= texture.dims().y) poshigh.y = poslow.y;
  if (poshigh.z >= texture.dims().z) poshigh.z = poslow.z;
  static const InterpolationType ONE = 1;
  PositionType posfrac(fmod(pos.x, ONE), fmod(pos.y, ONE), fmod(pos.z, ONE));
  // Grab values at corners
  auto left_bottom_back =   texture.at(poslow.x , poslow.y , poslow.z  );
  auto left_bottom_front =  texture.at(poslow.x , poslow.y , poshigh.z);
  auto left_top_back =      texture.at(poslow.x , poshigh.y, poslow.z  );
  auto left_top_front =     texture.at(poslow.x , poshigh.y, poshigh.z);
  auto right_bottom_back =  texture.at(poshigh.x, poslow.y , poslow.z  );
  auto right_bottom_front = texture.at(poshigh.x, poslow.y , poshigh.z);
  auto right_top_back =     texture.at(poshigh.x, poshigh.y, poslow.z  );
  auto right_top_front =    texture.at(poshigh.x, poshigh.y, poshigh.z);
  // Mix
  auto Mix = [&](ValueType a, ValueType b, InterpolationType t) { return a * (ONE-t) + b * t; };
  auto left_top = Mix(left_top_back, left_top_front, posfrac.z);
  auto right_top = Mix(right_top_back, right_top_front, posfrac.z);
  auto left_bottom = Mix(left_bottom_back, left_bottom_front, posfrac.z);
  auto right_bottom = Mix(right_bottom_back, right_bottom_front, posfrac.z);
  auto top = Mix(left_top, right_top, posfrac.x);
  auto bottom = Mix(left_bottom, right_bottom, posfrac.x);
  auto result = Mix(bottom, top, posfrac.y);
  return result;
}

template <typename ValueType>
marray<ValueType, 3> gradient_length_volume(const marray<ValueType, 3>& spatial) {
  const auto voxels = spatial.voxels();
  marray<ValueType, 3> gradient(voxels.x, voxels.y, voxels.z);
  gradient.fill(ValueType());
  pfor(size_t x, 1, voxels.x - 1) {
    for(size_t y = 1, y_end = voxels.y - 1; y < y_end; ++y) {
      for(size_t z = 1, z_end = voxels.z - 1; z < z_end; ++z) {
        gradient.at(x,y,z) = spatial.gradient(x,y,z).length();
      }
    }
  };
  return gradient;
}

template <typename ValueType>
marray<ValueType, 3> curvature_length_volume(const marray<ValueType, 3>& spatial) {
  const auto voxels = spatial.voxels();
  marray<ValueType, 3> curvature(voxels.x, voxels.y, voxels.z);
  curvature.fill(ValueType());
  pfor(size_t x, 2, voxels.x - 2) {
    for(size_t y = 2, y_end = voxels.y - 2; y < y_end; ++y) {
      for(size_t z = 2, z_end = voxels.z - 2; z < z_end; ++z) {
        curvature.at(x,y,z) = spatial.curvature(x,y,z);
      }
    }
  };
  return curvature;
}


} // namespace misc