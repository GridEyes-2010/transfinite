#pragma once

#include "parameterization.hh"

class ParameterizationBilinear : public Parameterization {
public:
  virtual ~ParameterizationBilinear();
  virtual Point2D mapToRibbon(size_t i, const Point2D &uv) const;
};
