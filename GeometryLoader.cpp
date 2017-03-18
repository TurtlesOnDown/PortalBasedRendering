#include "GeometryLoader.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace picojson;

GeometryLoader::GeometryLoader() {}

bool GeometryLoader::loadFromFile(string path) {
  string json = "";
  string line;
  ifstream testfile("sectors.json");
  if (testfile.is_open()) {
    while (getline(testfile, line)) {
      json += line + "\n";
    }
    testfile.close();
    return loadFromString(json);
  } else {
    lasterror = string() + "failed to open file \"" + path + "\"";
    return false;
  }
}

//behold, my big, beautiful, ball of mud
bool GeometryLoader::loadFromString(string json) {
  value v;
  string err = parse(v, json);
  if (!err.empty()) {
    lasterror = err;
    return false;
  }

  if (!v.is<object>()) {
    lasterror = "provided json is not an object";
    return false;
  }

  const object& obj = v.get<object>();
  auto verts_i = obj.find("verts");
  auto planes_i = obj.find("planes");
  auto sectors_i = obj.find("sectors");
  if (verts_i == obj.end()) {
    lasterror = "json does not contain a \"verts\" property";
    return false;
  }
  if (planes_i == obj.end()) {
    lasterror = "json does not contain a \"planes\" property";
    return false;
  }
  if (sectors_i == obj.end()) {
    lasterror = "json does not contain a \"sectors\" property";
    return false;
  }
  if (! verts_i->second.is<value::array>()) {
    lasterror = "\"verts\" is not an array";
    return false;
  }
  if (! planes_i->second.is<value::array>()) {
    lasterror = "\"planes\" is not an array";
    return false;
  }
  if (! sectors_i->second.is<value::array>()) {
    lasterror = "\"sectors\" is not an array";
    return false;
  }

  value::array verts = verts_i->second.get<value::array>();
  value::array planes = planes_i->second.get<value::array>();
  value::array secs = sectors_i->second.get<value::array>();

  vector<vertex> vertices;
  for (int i = 0; i < verts.size(); i += 3) {
    if (i+2 >= verts.size()) {
      lasterror = "invalid vertex definition";
      return false;
    }
    if (!verts[i].is<double>()) {
      lasterror = "invalid vertex coordinate definition: \"" + verts[i].to_str() + "\"";
      return false;
    }
    if (!verts[i+1].is<double>()) {
      lasterror = "invalid vertex coordinate definition: \"" + verts[i+1].to_str() + "\"";
      return false;
    }
    if (!verts[i+2].is<double>()) {
      lasterror = "invalid vertex coordinate definition: \"" + verts[i+2].to_str() + "\"";
      return false;
    }
    vertex v;
    v.x = verts[i].get<double>();
    v.y = verts[i+1].get<double>();
    v.z = verts[i+2].get<double>();
    vertices.push_back(v);
  }

  vector<plane> planes_real;
  for (int i = 0; i < planes.size(); i += 3) {
    if (i+2 >= planes.size()) {
      lasterror = "invalid plane definition";
      return false;
    }

    if (!planes[i].is<double>()) {
      lasterror = "invalid vertex index definition: \"" + planes[i].to_str() + "\"";
      return false;
    }
    if (!planes[i+1].is<double>()) {
      lasterror = "invalid vertex index definition: \"" + planes[i+1].to_str() + "\"";
      return false;
    }
    if (!planes[i+2].is<double>()) {
      lasterror = "invalid vertex index definition: \"" + planes[i+2].to_str() + "\"";
      return false;
    }

    plane p;
    int v1_id = planes[i].get<double>();
    int v2_id = planes[i+1].get<double>();
    int v3_id = planes[i+2].get<double>();
    if (v1_id < 0 || v1_id >= vertices.size() ||
	v2_id < 0 || v2_id >= vertices.size() ||
	v3_id < 0 || v3_id >= vertices.size()) {
      lasterror = "vertex index out of bounds";
      return false;
    }
    
    p.v1 = vertices[v1_id];
    p.v2 = vertices[v2_id];
    p.v3 = vertices[v3_id];
    planes_real.push_back(p);
  }

  sectors.clear();
  for (auto i = secs.begin(); i != secs.end(); i++) {
    if (! i->is<value::array>()) {
      lasterror = "invalid sector definition";
      return false;
    }
    sector sector_real;
    const value::array& sector_v = i->get<value::array>();
    for (auto j = sector_v.begin(); j != sector_v.end(); j++) {
      if (! j->is<value::object>()) {
	lasterror = "invalid sector-plane definition";
	return false;
      }
      const value::object& plane_obj = j->get<value::object>();
      auto plane_i = plane_obj.find("plane");
      auto up_i = plane_obj.find("up");
      if (plane_i == plane_obj.end()) {
	lasterror = "missing \"plane\" property";
	return false;
      }
      if (up_i == plane_obj.end()) {
	lasterror = "missing \"up\" property";
	return false;
      }
      if (! plane_i->second.is<double>()) {
	lasterror = "invalid plane index definition \"" + plane_i->second.to_str() + "\"";
	return false;
      }
      if (! up_i->second.is<value::object>()) {
	lasterror = "invalid up vector definition \"" + up_i->second.to_str() + "\"";
	return false;
      }

      int p_id = plane_i->second.get<double>();
      if (p_id < 0 || p_id >= planes_real.size()) {
	lasterror = "plane index out of bounds";
	return false;
      }

      plane p = planes_real[p_id];

      const value::object& up_obj = up_i->second.get<value::object>();
      auto x_i = up_obj.find("x");
      auto y_i = up_obj.find("y");
      auto z_i = up_obj.find("z");

      if (x_i == up_obj.end()) {
	lasterror = "up vector missing property \"x\"";
	return false;
      }
      if (y_i == up_obj.end()) {
	lasterror = "up vector missing property \"y\"";
	return false;
      }
      if (z_i == up_obj.end()) {
	lasterror = "up vector missing property \"z\"";
	return false;
      }

      if (! x_i->second.is<double>()) {
	lasterror = "invalid x magnitude definition \"" + x_i->second.to_str() + "\"";
	return false;
      }
      if (! y_i->second.is<double>()) {
	lasterror = "invalid y magnitude definition \"" + y_i->second.to_str() + "\"";
	return false;
      }
      if (! z_i->second.is<double>()) {
	lasterror = "invalid z magnitude definition \"" + z_i->second.to_str() + "\"";
	return false;
      }

      p.up.x = x_i->second.get<double>();
      p.up.y = y_i->second.get<double>();
      p.up.z = z_i->second.get<double>();
      sector_real.push_back(p);
    }
    sectors.push_back(sector_real);
  }
  
  return true;
}
