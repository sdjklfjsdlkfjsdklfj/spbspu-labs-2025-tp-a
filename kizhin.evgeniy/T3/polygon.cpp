#include "polygon.hpp"
#include <algorithm>
#include <istream>
#include <iterator>

namespace kizhin {
  struct Delimiter;

  std::istream& operator>>(std::istream&, Delimiter&&);
  std::istream& operator>>(std::istream&, Point&);
}

double kizhin::computeArea(const Polygon& polygon)
{
  double area = 0.0;
  const auto& points = polygon.points;
  for (auto i = points.begin(), end = points.end(); i != end; ++i) {
    const Point& current = *i;
    const Point& next = (i + 1 == end) ? points.front() : *(i + 1);
    area += current.x * next.y - next.x * current.y;
  }
  return std::abs(area) / 2.0;
}

std::istream& kizhin::operator>>(std::istream& in, Polygon& dest)
{
  std::istream::sentry sentry(in);
  if (!sentry) {
    return in;
  }
  size_t size = 0;
  if (!(in >> size)) {
    return in;
  }
  using InIt = std::istream_iterator< Point >;
#if 1
  PointContainer points(size);
  std::copy_n(InIt{ in }, size, points.begin()); // TODO: udefined
#else
  PointContainer points(InIt{ in }, InIt{});
#endif
  if (points.size() == size) {
    dest.points = std::move(points);
    in.clear();
  }
  return in;
}

struct kizhin::Delimiter
{
  char val;
};

std::istream& kizhin::operator>>(std::istream& in, Point& dest)
{
  std::istream::sentry sentry(in);
  if (!sentry) {
    return in;
  }
  Point input;
  in >> Delimiter{ '(' } >> input.x >> Delimiter{ ';' };
  if (in >> input.y >> Delimiter{ ')' }) {
    dest = input;
  }
  return in;
}

std::istream& kizhin::operator>>(std::istream& in, Delimiter&& dest)
{
  std::istream::sentry sentry(in);
  if (!sentry) {
    return in;
  }
  char c;
  if ((in >> c) && (c != dest.val)) {
    in.setstate(std::ios::failbit);
  }
  return in;
}

