#pragma once

#include <vector>
#include <array>
#include <math_src/vec_maker.h>

template <typename T, size_t N>
class marray {
public:
  static const size_t dimensions = N;
  typedef marray<T, N> my_type;
  typedef std::vector<T> container_type;
  typedef typename container_type::pointer pointer;
  typedef typename container_type::const_pointer const_pointer;
  typedef typename container_type::difference_type difference_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::reference reference;
  typedef typename container_type::const_reference const_reference;
  typedef typename container_type::value_type value_type;
  typedef typename container_type::iterator iterator;
  typedef typename container_type::const_iterator const_iterator;
  typedef typename container_type::reverse_iterator reverse_iterator;
  typedef typename container_type::const_reverse_iterator const_reverse_iterator;
  
  marray() : width_mul_height_(0) { dims_.fill(0); }

  marray(size_t w, size_t h = 1, size_t d = 1) {
    set_size(w, h, d);
  }

  void set_size(size_t w, size_t h = 1, size_t d = 1) {
    dims_[0] = w;
    dims_[1] = h;
    dims_[2] = d;
    width_mul_height_ = width() * height();
    data_.resize(w*h*d);
  }

  // Emplace data
  void set_data(size_t w, container_type new_data) {
    RASSERT(w == new_data.size());
    data_ = std::move(new_data);
    set_size(w);
  }
  void set_data(size_t w, size_t h, container_type new_data) {
    RASSERT(w*h == new_data.size());
    data_ = std::move(new_data);
    set_size(w,h);
  }
  void set_data(size_t w, size_t h, size_t d, container_type new_data) {
    RASSERT(w*h*d == new_data.size());
    data_ = std::move(new_data);
    set_size(w,h,d);
  }

  // Size
  size_t width() const { return dims_[0]; }
  size_t height() const { return dims_[1]; }
  size_t depth() const { return dims_[2]; }
  size_t size() const { return width_mul_height_ * depth(); }
  math::Vec3i voxels() const { return math::Vec3i((int)dims_[0], (int)dims_[1], (int)dims_[2]); }
  bool empty() const { return size() == 0; }

  // Data access by iterator
  iterator begin() { return data_.begin(); }
  const_iterator begin() const { return data_.begin(); }
  iterator end() { return data_.end(); }
  const_iterator end() const { return data_.end(); }
  reverse_iterator rend() { return data_.rend(); }
  const_reverse_iterator rend() const { return data_.rend(); }
  reverse_iterator rbegin() { return data_.rbegin(); }
  const_reverse_iterator rbegin() const { return data_.rbegin(); }

  value_type& front() { return data_.front(); }
  const value_type& front() const { return data_.front(); }
  value_type& back() { return data_.back(); }
  const value_type& back() const { return data_.back(); }

  value_type& operator[](size_t idx) { return data_[idx]; }
  const value_type& operator[](size_t idx) const { return data_[idx]; }

  // Element access - 1D
  value_type& at(size_t x) { 
    STATIC_ASSERT(N == 1);
    DASSERT(x < width()); 
    return data_[x]; 
  }
  const value_type& at(size_t x) const { 
    STATIC_ASSERT(N == 1);
    DASSERT(x < width()); 
    return data_[x]; 
  }
  // Element access - 2D
  value_type& at(size_t x, size_t y) { 
    STATIC_ASSERT(N == 2);
    DASSERT(x < width() && y < height()); 
    return data_[y*width()+x]; 
  }
  const value_type& at(size_t x, size_t y) const { 
    STATIC_ASSERT(N == 2);
    DASSERT(x < width() && y < height()); 
    return data_[y*width()+x]; 
  }
  // Element access - 3D
  template <typename X, typename Y, typename Z>
  value_type& at(X x, Y y, Z z) {
    static_assert(dimensions == 3, "");
    static_assert(std::is_integral<X>::value && sizeof(X) > 1, "");
    static_assert(std::is_integral<Y>::value && sizeof(Y) > 1, "");
    static_assert(std::is_integral<Z>::value && sizeof(Z) > 1, "");
    DASSERT(x < width() && y < height() && z < depth());
    return data_[size_t(z)*width_mul_height_ + size_t(y)*width() + size_t(x)];
  }
  template <typename X, typename Y, typename Z>
  const value_type& at(X x, Y y, Z z) const {
    static_assert(dimensions == 3, "");
    static_assert(std::is_integral<X>::value && sizeof(X) > 1, "");
    static_assert(std::is_integral<Y>::value && sizeof(Y) > 1, "");
    static_assert(std::is_integral<Z>::value && sizeof(Z) > 1, "");
    DASSERT(x < width() && y < height() && z < depth());
    return data_[size_t(z)*width_mul_height_ + size_t(y)*width() + size_t(x)];
  }

  // Fill
  my_type& fill(const value_type& val) {
    ::std::fill(data_.begin(), data_.end(), val);
    return *this;
  }

  // Linear interpolation
  value_type atli(float x, float y) const {
    auto lt = at(floor(x), floor(y));
    auto rt = at(ceil(x), floor(y));
    auto lb = at(floor(x), ceil(y));
    auto rb = at(ceil(x), ceil(y));
    auto xt = fmod(x, dims_[0]);
    auto yt = fmod(y, dims_[1]);
    auto ti = interpolate(lt, rt, xt);
    auto bi = interpolate(lb, rb, xt);
    return interpolate(ti, bi, yt);
  }

  // Nearest neighbour interpolation
  value_type& atnn(float x, float y, float z) {
    return at(
      math::clamp<int>((int)x, 0, (int)dims_[0] - 1),
      math::clamp<int>((int)y, 0, (int)dims_[1] - 1),
      math::clamp<int>((int)z, 0, (int)dims_[2] - 1)
    );
  }
  const value_type& atnn(float x, float y, float z) const {
    return at(
      math::clamp<int>((int)x, 0, (int)dims_[0] - 1),
      math::clamp<int>((int)y, 0, (int)dims_[1] - 1),
      math::clamp<int>((int)z, 0, (int)dims_[2] - 1)
    );
  }
  value_type& atnn(float x, float y) {
    return at(
      math::clamp<int>((int)x, 0, (int)dims_[0] - 1),
      math::clamp<int>((int)y, 0, (int)dims_[1] - 1)
    );
  }
  const value_type& atnn(float x, float y) const {
    return at(
      math::clamp<int>((int)x, 0, (int)dims_[0] - 1),
      math::clamp<int>((int)y, 0, (int)dims_[1] - 1)
    );
  }

  
  my_type smooth() const {
    auto ret = *this;
    for (size_t x = 1; x + 1 < width(); ++x)
      for (size_t y = 1; y + 1 < height(); ++y)
        for (size_t z = 1; z + 1 < depth(); ++z) {
          ret.at(x, y, z) = 
            at(x+1, y  , z  ) + 
            at(x  , y+1, z  ) + 
            at(x  , y  , z+1) + 
            at(x-1, y  , z  ) + 
            at(x  , y-1, z  ) + 
            at(x  , y  , z-1) + 
            at(x  , y  , z  );
          ret.at(x, y, z) /= 7;
        }
    return ret;
  }

  my_type smooth2() const {
    auto ret = *this;

    pfor(size_t x, 1, width()-1) {
      for (size_t y = 1; y + 1 < height(); ++y)
        ret.at(x, y) = (
          at(x-1, y) + 
          at(x+1, y) + 
          at(x, y-1) + 
          at(x, y+1)
          ) / 4;
    };
    return ret;
  }


  std::vector<value_type*> sub_range(int x, int y, int z, int w) {
    std::vector<value_type*> ret;
    for (auto xt = x - w; xt < x + w; ++x)
      for (auto yt = y - w; yt < y + w; ++y)
        for (auto zt = z - w; zt < z + w; ++z) 
          ret.push_back(&at(x,y,z));
    return ret;
  }

  my_type sub_marray(size_t xstart, size_t ystart, size_t w, size_t h) const {
    STATIC_ASSERT(N==2);
    my_type ret(w,h);
    loopm((auto x=w)(auto y=h))
      ret.at(x,y) = this->at(x+xstart, y+ystart);
    return ret;
  }

  /*
  my_type sub_marray(size_t x, size_t y, size_t z, size_t w, size_t h, size_t d) const {
    STATIC_ASSERT(N==3);
    my_type ret(w,h,d);
  }
  */

  math::Vec3<value_type> gradient(size_t x, size_t y, size_t z) const {
    static_assert(N == 3 && !std::is_unsigned<value_type>::value, "");
    static const auto MINUS_TWO = value_type(-2);
    value_type X = at(x+1, y  , z  ) - at(x-1, y  , z  );
    value_type Y = at(x  , y+1, z  ) - at(x  , y-1, z  );
    value_type Z = at(x  , y  , z+1) - at(x  , y  , z-1);
    X /= MINUS_TWO;
    Y /= MINUS_TWO;
    Z /= MINUS_TWO;
    return math::Vec3<value_type>(X, Y, Z);
  }

  value_type curvature(size_t x, size_t y, size_t z) const {
    STATIC_ASSERT(N == 3);
    const auto my_gradient = gradient(x, y, z);
    value_type sum = 0;
    sum += (my_gradient - gradient(x+1, y  , z  )).length();
    sum += (my_gradient - gradient(x  , y+1, z  )).length();
    sum += (my_gradient - gradient(x  , y  , z+1)).length();
    sum += (my_gradient - gradient(x-1, y  , z  )).length();
    sum += (my_gradient - gradient(x  , y-1, z  )).length();
    sum += (my_gradient - gradient(x  , y  , z-1)).length();
    return sum;
  }

  my_type resized_nn(const math::Vec3i& newSize) const {
    STATIC_ASSERT(N == 3);
    auto ret = my_type(newSize.x, newSize.y, newSize.z);
    for (size_t x = 0; x < ret.width(); ++x) {
      for (size_t y = 0; y < ret.height(); ++y) {
        for (size_t z = 0; z < ret.depth(); ++z) {
          float xf = math::reinterval<float>((float)x, 0, (float)ret.width(), 0, (float)width());
          float yf = math::reinterval<float>((float)y, 0, (float)ret.height(), 0, (float)height());
          float zf = math::reinterval<float>((float)z, 0, (float)ret.depth(), 0, (float)depth());
          ret.at(x, y, z) = atnn(xf, yf, zf);
        }
      }
    }
    return ret;
  }

  value_type* data() { return data_.data(); }
  const value_type* data() const { return data_.data(); }
  const std::vector<value_type>& data_vector() const { return data_; }

  std::vector<value_type> data_;

private:
  size_t width_mul_height_;
  std::array <size_t, 3> dims_;
  IMPLEMENTS_ALL((data_)(dims_)(width_mul_height_));
};


typedef marray<float, 2> marray2f;
typedef marray<float, 3> marray3f;
typedef marray<double, 2> marray2d;
typedef marray<double, 3> marray3d;