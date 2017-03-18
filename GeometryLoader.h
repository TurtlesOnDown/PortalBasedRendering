#ifndef _GEOMETRYLOADER_H_
#define _GEOMETRYLOADER_H_

#include <vector>
#include "picojson.h"

class GeometryLoader {
 private:
  struct vertex {
    double x, y, z;
  };
  struct plane {
    vertex v1, v2, v3;
    vertex up;
  };
  typedef std::vector<plane> sector;
  std::vector<sector> sectors;
  std::string lasterror = "no error";
 public:
  GeometryLoader();
  const std::string& getLastError() const {return lasterror;}
  bool loadFromFile(xstd::string path);
  bool loadFromString(std::string json);
};

#endif
