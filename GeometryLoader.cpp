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
  
  
  return true;
}
