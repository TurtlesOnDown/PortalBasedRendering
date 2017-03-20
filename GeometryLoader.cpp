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
  }
  else {
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
  auto prePlanes_i = obj.find("planes");
  auto preSectors_i = obj.find("sectors");
  if (verts_i == obj.end()) {
    lasterror = "json does not contain a \"verts\" property";
    return false;
  }
  if (prePlanes_i == obj.end()) {
    lasterror = "json does not contain a \"prePlanes\" property";
    return false;
  }
  if (preSectors_i == obj.end()) {
    lasterror = "json does not contain a \"preSectors\" property";
    return false;
  }
  if (!verts_i->second.is<value::array>()) {
    lasterror = "\"verts\" is not an array";
    return false;
  }
  if (!prePlanes_i->second.is<value::array>()) {
    lasterror = "\"planes\" is not an array";
    return false;
  }
  if (!preSectors_i->second.is<value::array>()) {
    lasterror = "\"sectors\" is not an array";
    return false;
  }

  value::array verts = verts_i->second.get<value::array>();
  value::array prePlanes = prePlanes_i->second.get<value::array>();
  value::array secs = preSectors_i->second.get<value::array>();

  vector<preVertex> vertices;
  for (int i = 0; i < verts.size(); i += 3) {
    if (i + 2 >= verts.size()) {
      lasterror = "invalid preVertex definition";
      return false;
    }
    if (!verts[i].is<double>()) {
      lasterror = "invalid preVertex coordinate definition: \"" + verts[i].to_str() + "\"";
      return false;
    }
    if (!verts[i + 1].is<double>()) {
      lasterror = "invalid preVertex coordinate definition: \"" + verts[i + 1].to_str() + "\"";
      return false;
    }
    if (!verts[i + 2].is<double>()) {
      lasterror = "invalid preVertex coordinate definition: \"" + verts[i + 2].to_str() + "\"";
      return false;
    }
    preVertex v;
    v.x = verts[i].get<double>();
    v.y = verts[i + 1].get<double>();
    v.z = verts[i + 2].get<double>();
    vertices.push_back(v);
  }

  vector<prePlane> prePlanes_real;
  for (int i = 0; i < prePlanes.size(); i += 3) {
    if (i + 2 >= prePlanes.size()) {
      lasterror = "invalid prePlane definition";
      return false;
    }

    if (!prePlanes[i].is<double>()) {
      lasterror = "invalid preVertex index definition: \"" + prePlanes[i].to_str() + "\"";
      return false;
    }
    if (!prePlanes[i + 1].is<double>()) {
      lasterror = "invalid preVertex index definition: \"" + prePlanes[i + 1].to_str() + "\"";
      return false;
    }
    if (!prePlanes[i + 2].is<double>()) {
      lasterror = "invalid preVertex index definition: \"" + prePlanes[i + 2].to_str() + "\"";
      return false;
    }

    prePlane p;
    int v1_id = prePlanes[i].get<double>();
    int v2_id = prePlanes[i + 1].get<double>();
    int v3_id = prePlanes[i + 2].get<double>();
    if (v1_id < 0 || v1_id >= vertices.size() ||
      v2_id < 0 || v2_id >= vertices.size() ||
      v3_id < 0 || v3_id >= vertices.size()) {
      lasterror = "preVertex index out of bounds";
      return false;
    }

    p.v1 = vertices[v1_id];
    p.v2 = vertices[v2_id];
    p.v3 = vertices[v3_id];
    prePlanes_real.push_back(p);
  }

  preSectors.clear();
  for (auto i = secs.begin(); i != secs.end(); i++) {
    if (!i->is<value::array>()) {
      lasterror = "invalid preSector definition";
      return false;
    }
    preSector preSector_real;
    const value::array& preSector_v = i->get<value::array>();
    for (auto j = preSector_v.begin(); j != preSector_v.end(); j++) {
      if (!j->is<value::object>()) {
        lasterror = "invalid preSector-prePlane definition";
        return false;
      }
      const value::object& prePlane_obj = j->get<value::object>();
      auto prePlane_i = prePlane_obj.find("plane");
      auto up_i = prePlane_obj.find("up");
      if (prePlane_i == prePlane_obj.end()) {
        lasterror = "missing \"prePlane\" property";
        return false;
      }
      if (up_i == prePlane_obj.end()) {
        lasterror = "missing \"up\" property";
        return false;
      }
      if (!prePlane_i->second.is<double>()) {
        lasterror = "invalid prePlane index definition \"" + prePlane_i->second.to_str() + "\"";
        return false;
      }
      if (!up_i->second.is<value::object>()) {
        lasterror = "invalid up vector definition \"" + up_i->second.to_str() + "\"";
        return false;
      }

      int p_id = prePlane_i->second.get<double>();
      if (p_id < 0 || p_id >= prePlanes_real.size()) {
        lasterror = "prePlane index out of bounds";
        return false;
      }

      prePlane p = prePlanes_real[p_id];

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

      if (!x_i->second.is<double>()) {
        lasterror = "invalid x magnitude definition \"" + x_i->second.to_str() + "\"";
        return false;
      }
      if (!y_i->second.is<double>()) {
        lasterror = "invalid y magnitude definition \"" + y_i->second.to_str() + "\"";
        return false;
      }
      if (!z_i->second.is<double>()) {
        lasterror = "invalid z magnitude definition \"" + z_i->second.to_str() + "\"";
        return false;
      }

      p.up.x = x_i->second.get<double>();
      p.up.y = y_i->second.get<double>();
      p.up.z = z_i->second.get<double>();
      preSector_real.push_back(p);
    }
    preSectors.push_back(preSector_real);
  }

  return true;
}

World GeometryLoader::loadGeometryFinal() {
  World world = {};
  for (preSector pre_s : preSectors) {
    vector<XPlane> planes;
    Sector s(planes);
    for (const prePlane& pre_p : pre_s) {
      XPlane p(vector<XVertex>{XVertex{ glm::vec3(pre_p.v1.x, pre_p.v1.y, pre_p.v1.z), glm::vec2(0, 0) },
        XVertex{ glm::vec3(pre_p.v2.x, pre_p.v2.y, pre_p.v2.z), glm::vec2(0, 0) },
        XVertex{ glm::vec3(pre_p.v3.x, pre_p.v3.y, pre_p.v3.z), glm::vec2(0, 0) }},
        glm::vec3(pre_p.up.x, pre_p.up.y, pre_p.up.z),
        1, nullptr, nullptr);
      planes.push_back(p);
    }
    world.push_back(planes);
  }
  return world;
}
