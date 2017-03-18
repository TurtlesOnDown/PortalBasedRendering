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
  bool loadFromFile(std::string path);
  bool loadFromString(std::string json);
  //nate you implement this
  //read all sector data from sectors
  //sectors is a vector of vectors of planes,
  //each plane has 3 vertices and an up vector (represented as a vertex)
  //vertices are already seperated at this point
  //you will need to compute the normal from the given vertices. The second part
  //of the binormal should be given by the up vector.
  void loadGeometryFinal();
};

#endif
