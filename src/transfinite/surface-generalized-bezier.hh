#pragma once

#include "surface.hh"

namespace Transfinite {

class SurfaceGeneralizedBezier : public Surface {
public:
  SurfaceGeneralizedBezier();
  virtual ~SurfaceGeneralizedBezier();
  virtual Point3D eval(const Point2D &uv) const;
  size_t degree() const;
  size_t layers() const;
  void initNetwork(size_t degree);
  Point3D centralControlPoint() const;
  void setCentralControlPoint(const Point3D &p);
  Point3D controlPoint(size_t i, size_t j, size_t k) const;
  void setControlPoint(size_t i, size_t j, size_t k, const Point3D &p);

protected:
  virtual std::shared_ptr<Ribbon> newRibbon() const;

private:
  using ControlNet = std::vector<PointVector>;

  static void bernstein(size_t n, double u, DoubleVector &coeff);

  size_t degree_, layers_;
  Point3D central_cp_;
  std::vector<ControlNet> nets_;
};

} // namespace Transfinite