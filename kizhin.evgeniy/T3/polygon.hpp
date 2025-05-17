#ifndef SPBSPU_LABS_2025_TP_A_KIZHIN_EVGENIY_T3_POLYGON_HPP
#define SPBSPU_LABS_2025_TP_A_KIZHIN_EVGENIY_T3_POLYGON_HPP

#include <iosfwd>
#include <vector>

namespace kizhin {
  struct Point;
  struct Polygon;

  using PointContainer = std::vector< Point >;
  using PolygonContainer = std::vector< Polygon >;

  double computeArea(const Polygon&);
  std::istream& operator>>(std::istream&, Polygon&);
}

struct kizhin::Polygon
{
  PointContainer points;
};

struct kizhin::Point
{
  int x = 0;
  int y = 0;
};

#endif

