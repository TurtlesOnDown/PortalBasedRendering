#ifndef _GEOMETRYLOADER_H_
#define _GEOMETRYLOADER_H_

#include <vector>
#include "picojson.h"
#include "XPlane.h"

class GeometryLoader {
private:
  struct preVertex {
    double x, y, z;
  };
  struct prePlane {
    preVertex v1, v2, v3;
    preVertex up;
  };
  typedef std::vector<prePlane> preSector;
  std::vector<preSector> preSectors;
  std::string lasterror = "no error";
public:
  GeometryLoader();
  const std::string& getLastError() const { return lasterror; }
  bool loadFromFile(std::string path);
  bool loadFromString(std::string json);
  //nate you implement this
  //read all preSector data from preSectors
  //preSectors is a vector of vectors of prePlanes,
  //each prePlane has 3 vertices and an up vector (represented as a preVertex)
  //vertices are already seperated at this point
  //you will need to compute the normal from the given vertices. The second part
  //of the binormal should be given by the up vector.
  World loadGeometryFinal();
};

#endif 