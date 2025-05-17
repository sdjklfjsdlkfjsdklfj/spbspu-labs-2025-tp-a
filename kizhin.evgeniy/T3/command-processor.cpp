#include "command-processor.hpp"
#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>

namespace kizhin {
  using CmdContainer = std::map< std::string, std::function< void(void) > >;

  void processArea(const PolygonContainer&, std::istream&, std::ostream&);
  void processMax(const PolygonContainer&, std::istream&, std::ostream&);
  void processMin(const PolygonContainer&, std::istream&, std::ostream&);
  void processCount(const PolygonContainer&, std::istream&, std::ostream&);
  void processPerms(const PolygonContainer&, std::istream&, std::ostream&);
  void processMaxseq(const PolygonContainer&, std::istream&, std::ostream&);

  namespace area {
    void processEven(const PolygonContainer&, std::ostream&);
    void processOdd(const PolygonContainer&, std::ostream&);
    void processMean(const PolygonContainer&, std::ostream&);
    /* TODO: rename */
    void processNumOfVertexes(const PolygonContainer&, std::size_t, std::ostream&);
  }

  namespace max {
    void processArea(const PolygonContainer&, std::ostream&);
    void processVertexes(const PolygonContainer&, std::ostream&);
  }

  namespace min {
    void processArea(const PolygonContainer&, std::ostream&);
    void processVertexes(const PolygonContainer&, std::ostream&);
  }

  namespace count {
    void processEven(const PolygonContainer&, std::ostream&);
    void processOdd(const PolygonContainer&, std::ostream&);
    /* TODO: rename */
    void processNumOfVertexes(const PolygonContainer&, std::size_t, std::ostream&);
  }

  using namespace std::placeholders;
  const auto pointsSize = std::mem_fn(&PointContainer::size);
  const auto polygonSize = std::bind(pointsSize, std::bind(&Polygon::points, _1));
  const auto polygonArea = std::bind(computeArea, _1);

  const auto modTwo = std::bind(std::modulus<>{}, std::bind(polygonSize, _1), 2);
  const auto isEven = std::bind(std::equal_to<>{}, std::bind(modTwo, _1), 0);
  const auto isOdd = std::bind(std::logical_not<>{}, std::bind(isEven, _1));

  const auto areaCompPart = std::bind(std::less<>{}, std::bind(polygonArea, _1), _2);
  const auto areaComp = std::bind(areaCompPart, _1, std::bind(polygonArea, _2));

  const auto vertexCompPart = std::bind(std::less<>{}, std::bind(polygonSize, _1), _2);
  const auto vertexComp = std::bind(vertexCompPart, _1, std::bind(polygonSize, _2));
}

void kizhin::processCommands(PolygonContainer& polygons, std::istream& in,
    std::ostream& out)
{
  /* TODO: StreamGuard */
  out << std::fixed << std::setprecision(1);
  const auto inRef = std::ref(in);
  const auto outRef = std::ref(out);
  const CmdContainer commands = {
    { "AREA", std::bind(processArea, std::cref(polygons), inRef, outRef) },
    { "MAX", std::bind(processMax, std::cref(polygons), inRef, outRef) },
    { "MIN", std::bind(processMin, std::cref(polygons), inRef, outRef) },
    { "COUNT", std::bind(processCount, std::cref(polygons), inRef, outRef) },
    { "PERMS", std::bind(processPerms, std::cref(polygons), inRef, outRef) },
    { "MAXSEQ", std::bind(processMaxseq, std::cref(polygons), inRef, outRef) },
  };
  CmdContainer::key_type currCmd;
  while (in >> currCmd) {
    try {
      if (polygons.empty()) {
        throw std::logic_error("");
      }
      commands.at(currCmd)();
    } catch (const std::logic_error&) {
      out << "<INVALID COMMAND>\n";
      in.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
    }
  }
}

void kizhin::processArea(const PolygonContainer& polygons, std::istream& in,
    std::ostream& out)
{
  const auto outRef = std::ref(out);
  const CmdContainer commands = {
    { "EVEN", std::bind(area::processEven, std::cref(polygons), outRef) },
    { "ODD", std::bind(area::processOdd, std::cref(polygons), outRef) },
    { "MEAN", std::bind(area::processMean, std::cref(polygons), outRef) },
  };
  std::size_t vertexCount = 0;
  if (in >> vertexCount) {
    area::processNumOfVertexes(polygons, vertexCount, out);
    return;
  }
  in.clear();
  CmdContainer::key_type currCmd;
  if (!(in >> currCmd)) {
    throw std::logic_error("Failed to input command");
  }
  commands.at(currCmd)();
}

void kizhin::processMax(const PolygonContainer& polygons, std::istream& in,
    std::ostream& out)
{
  const auto outRef = std::ref(out);
  const CmdContainer commands = {
    { "AREA", std::bind(max::processArea, std::cref(polygons), outRef) },
    { "VERTEXES", std::bind(max::processVertexes, std::cref(polygons), outRef) },
  };
  CmdContainer::key_type currCmd;
  if (!(in >> currCmd)) {
    throw std::logic_error("Failed to input command");
  }
  commands.at(currCmd)();
}

void kizhin::processMin(const PolygonContainer& polygons, std::istream& in,
    std::ostream& out)
{
  const auto outRef = std::ref(out);
  const CmdContainer commands = {
    { "AREA", std::bind(min::processArea, std::cref(polygons), outRef) },
    { "VERTEXES", std::bind(min::processVertexes, std::cref(polygons), outRef) },
  };
  CmdContainer::key_type currCmd;
  if (!(in >> currCmd)) {
    throw std::logic_error("Failed to input command");
  }
  commands.at(currCmd)();
}

void kizhin::processCount(const PolygonContainer& polygons, std::istream& in,
    std::ostream& out)
{
  const auto outRef = std::ref(out);
  const CmdContainer commands = {
    { "EVEN", std::bind(count::processEven, std::cref(polygons), outRef) },
    { "ODD", std::bind(count::processOdd, std::cref(polygons), outRef) },
  };
  std::size_t vertexCount = 0;
  if (in >> vertexCount) {
    count::processNumOfVertexes(polygons, vertexCount, out);
    return;
  }
  in.clear();
  CmdContainer::key_type currCmd;
  if (!(in >> currCmd)) {
    throw std::logic_error("Failed to input command");
  }
  commands.at(currCmd)();
}

void kizhin::processPerms(const PolygonContainer& /*polygons*/, std::istream& /*in*/,
    std::ostream& /*out*/)
{
  return;
}

void kizhin::processMaxseq(const PolygonContainer& /*polygons*/
    ,
    std::istream& /*in*/, std::ostream& /*out*/)
{
  return;
}

void kizhin::area::processEven(const PolygonContainer& polygons, std::ostream& out)
{
  PolygonContainer copy;
  copy.reserve(std::count_if(polygons.begin(), polygons.end(), isEven));
  std::copy_if(polygons.begin(), polygons.end(), std::back_inserter(copy), isEven);
  const auto summator = std::bind(std::plus<>{}, _1, std::bind(polygonArea, _2));
  out << std::accumulate(copy.begin(), copy.end(), 0.0, summator) << '\n';
}

void kizhin::area::processOdd(const PolygonContainer& polygons, std::ostream& out)
{
  PolygonContainer copy;
  copy.reserve(std::count_if(polygons.begin(), polygons.end(), isOdd));
  std::copy_if(polygons.begin(), polygons.end(), std::back_inserter(copy), isOdd);
  const auto summator = std::bind(std::plus<>{}, _1, std::bind(polygonArea, _2));
  out << std::accumulate(copy.begin(), copy.end(), 0.0, summator) << '\n';
}

void kizhin::area::processMean(const PolygonContainer& polygons, std::ostream& out)
{
  const auto summator = std::bind(std::plus<>{}, _1, std::bind(polygonArea, _2));
  const double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0, summator);
  out << sum / polygons.size() << '\n';
}

void kizhin::area::processNumOfVertexes(const PolygonContainer& polygons,
    std::size_t count, std::ostream& out)
{
  const auto equalTo = std::equal_to<>{};
  const auto isNeededSize = std::bind(equalTo, std::bind(polygonSize, _1), count);
  PolygonContainer copy;
  copy.reserve(std::count_if(polygons.begin(), polygons.end(), isNeededSize));
  std::copy_if(polygons.begin(), polygons.end(), std::back_inserter(copy), isNeededSize);
  const auto summator = std::bind(std::plus<>{}, _1, std::bind(polygonArea, _2));
  out << std::accumulate(copy.begin(), copy.end(), 0.0, summator) << '\n';
}

void kizhin::max::processArea(const PolygonContainer& polygons, std::ostream& out)
{
  const auto res = std::max_element(polygons.begin(), polygons.end(), areaComp);
  out << computeArea(*res) << '\n';
}

void kizhin::max::processVertexes(const PolygonContainer& polygons, std::ostream& out)
{
  const auto res = std::max_element(polygons.begin(), polygons.end(), vertexComp);
  out << res->points.size() << '\n';
}

void kizhin::min::processArea(const PolygonContainer& polygons, std::ostream& out)
{
  const auto res = std::min_element(polygons.begin(), polygons.end(), areaComp);
  out << computeArea(*res) << '\n';
}

void kizhin::min::processVertexes(const PolygonContainer& polygons, std::ostream& out)
{
  const auto res = std::min_element(polygons.begin(), polygons.end(), vertexComp);
  out << res->points.size() << '\n';
}

void kizhin::count::processEven(const PolygonContainer& polygons, std::ostream& out)
{
  out << std::count_if(polygons.begin(), polygons.end(), isEven) << '\n';
}

void kizhin::count::processOdd(const PolygonContainer& polygons, std::ostream& out)
{
  out << std::count_if(polygons.begin(), polygons.end(), isOdd) << '\n';
}

void kizhin::count::processNumOfVertexes(const PolygonContainer& polygons,
    std::size_t count, std::ostream& out)
{
  const auto equalTo = std::equal_to<>{};
  const auto isNeededSize = std::bind(equalTo, count, std::bind(polygonSize, _1));
  out << std::count_if(polygons.begin(), polygons.end(), isNeededSize) << '\n';
}

