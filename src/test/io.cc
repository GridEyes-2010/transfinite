#include <fstream>
#include <iostream>
#include <sstream>

#include "io.hh"

#include "domain.hh"

CurveVector readLOP(std::string filename) {
  std::ifstream f(filename);
  if (!f.is_open()) {
    std::cerr << "Unable to open file: " << filename << std::endl;
    return CurveVector();
  }

  size_t n, deg, nk, nc;
  DoubleVector knots;
  PointVector cpts;
  CurveVector result;

  f >> n;
  result.reserve(n);
  for (size_t i = 0; i < n; ++i) {
    f >> deg;
    f >> nk;
    knots.resize(nk);
    for (size_t j = 0; j < nk; ++j)
      f >> knots[j];
    f >> nc;
    cpts.resize(nc);
    for (size_t j = 0; j < nc; ++j)
      f >> cpts[j][0] >> cpts[j][1] >> cpts[j][2];
    result.push_back(std::make_shared<BSCurve>(deg, knots, cpts));
  }

  return result;
}

TriMesh readOBJ(const std::string &filename) {
  TriMesh result;
  std::ifstream f(filename);
  if (!f.is_open()) {
    std::cerr << "Unable to open file: " << filename << std::endl;
    return result;
  }
  bool points_set = false;
  std::string line;
  std::istringstream ss;
  Point3D p;
  TriMesh::Triangle t;
  PointVector pv;
  while (!f.eof()) {
    std::getline(f, line);
    if (line.empty())
      continue;
    switch (line[0]) {
    case 'v':
      ss.str(line);
      ss.seekg(2); // skip the first two characters
      ss >> p[0] >> p[1] >> p[2];
      pv.push_back(p);
      break;
    case 'f':
      if (!points_set) {
        result.setPoints(pv);
        points_set = true;
      }
      ss.str(line);
      ss.seekg(2); // skip the first two characters
      ss >> t[0] >> t[1] >> t[2];
      result.addTriangle(t[0], t[1], t[2]);
      break;
    default:
      break;
    }
  }
  return result;
}

SurfaceGeneralizedBezier loadBezier(const std::string &filename) {
  std::ifstream f(filename);
  if (!f.is_open()) {
    std::cerr << "Unable to open file: " << filename << std::endl;
    return SurfaceGeneralizedBezier();
  }

  size_t n, d;
  f >> n >> d;
  size_t l = (d + 1) / 2;
  size_t cp = 1 + d / 2;
  cp = n * cp * l + 1;          // # of control points

  SurfaceGeneralizedBezier surf;
  surf.initNetwork(n, d);

  Point3D p;
  f >> p[0] >> p[1] >> p[2];
  surf.setCentralControlPoint(p);

  for (size_t i = 1, side = 0, col = 0, row = 0; i < cp; ++i, ++col) {
    if (col >= d - row) {
      if (++side >= n) {
        side = 0;
        ++row;
      }
      col = row;
    }
    f >> p[0] >> p[1] >> p[2];
    surf.setControlPoint(side, col, row, p);
  }
  f.close();

  surf.setupLoop();

  return surf;
}

void saveBezier(const SurfaceGeneralizedBezier &surf, const std::string &filename) {
  std::ofstream f(filename);
  if (!f.is_open()) {
    std::cerr << "Unable to open file: " << filename << std::endl;
    return;
  }

  size_t n = surf.domain()->vertices().size();
  size_t d = surf.degree();
  f << n << ' ' << d << std::endl;
  size_t l = (d + 1) / 2;
  size_t cp = 1 + d / 2;
  cp = n * cp * l + 1;          // # of control points

  Point3D p = surf.centralControlPoint();
  f << p[0] << ' ' << p[1] << ' ' << p[2] << std::endl;

  for (size_t i = 1, side = 0, col = 0, row = 0; i < cp; ++i, ++col) {
    if (col >= d - row) {
      if (++side >= n) {
        side = 0;
        ++row;
      }
      col = row;
    }
    p = surf.controlPoint(side, col, row);
    f << p[0] << ' ' << p[1] << ' ' << p[2] << std::endl;
  }
  f.close();
}

void writeBezierControlPoints(const SurfaceGeneralizedBezier &surf, const std::string &filename) {
  // Slow but simple implementation creating a nice mesh
  size_t n = surf.domain()->vertices().size();
  size_t d = surf.degree();
  size_t l = surf.layers();
  size_t cp = 1 + d / 2;
  cp = n * cp * l + 1;

  auto findControlPoint = [n,d,cp](size_t i, size_t j, size_t k) -> size_t {
    for (size_t c = 1, side = 0, col = 0, row = 0; c < cp; ++c, ++col) {
      if (col >= d - row) {
        if (++side >= n) {
          side = 0;
          ++row;
        }
        col = row;
      }
      size_t side_m = (side + n - 1) % n, side_p = (side + 1) % n;
      if ((i == side && j == col && k == row) ||
          (i == side_m && j == d - row && k == col) ||
          (i == side_p && j == row && k == d - col))
        return c + 1;
    }
    return 0;
  };

  std::ofstream f(filename);
  Point3D p = surf.centralControlPoint();
  f << "v " << p[0] << " " << p[1] << " " << p[2] << std::endl;
  for (size_t i = 1, side = 0, col = 0, row = 0; i < cp; ++i, ++col) {
    if (col >= d - row) {
      if (++side >= n) {
        side = 0;
        ++row;
      }
      col = row;
    }
    p = surf.controlPoint(side, col, row);
    f << "v " << p[0] << " " << p[1] << " " << p[2] << std::endl;
  }

  for (size_t i = 0; i < n; ++i)
    for (size_t j = 0; j <= d / 2; ++j)
      for (size_t k = 0; k < l - 1; ++k) {
        size_t a = findControlPoint(i, j, k);
        size_t b = findControlPoint(i, j + 1, k);
        size_t c = findControlPoint(i, j + 1, k + 1);
        size_t d = findControlPoint(i, j, k + 1);
        f << "f " << a << " " << b << " " << c << " " << d << std::endl;
      }
  if (d % 2 == 0)
    for (size_t i = 0; i < n; ++i) {
      size_t im = (i + n - 1) % n;
      size_t a = findControlPoint(i, l - 1, l - 1);
      size_t b = findControlPoint(i, l, l - 1);
      size_t c = 1;
      size_t d = findControlPoint(im, l, l - 1);
      f << "f " << a << " " << b << " " << c << " " << d << std::endl;
    }
  else
    for (size_t i = 0; i < n; ++i) {
      size_t a = findControlPoint(i, l - 1, l - 1);
      size_t b = findControlPoint(i, l, l - 1);
      size_t c = 1;
      f << "f " << a << " " << b << " " << c << std::endl;
    }

  f.close();
}