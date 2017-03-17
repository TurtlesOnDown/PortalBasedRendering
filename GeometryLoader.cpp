#include "GeometryLoader.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace picojson;

void GeometryLoader::test() {
  value v;
  string json = "";
  string line;
  ifstream testfile("sectors.json");
  if (testfile.is_open()) {
    while (getline(testfile, line)) {
      json += line + "\n";
    }
    testfile.close();
  } else {
    cout << "oh shit it broke" << endl;
    return;
  }

  cout << "-----json begin-----\n"
       << json
       << "\n------json end------" << endl;

  string err = parse(v, json);
  if (!err.empty()) {
    cout << err << endl;
    return;
  }

  if (!v.is<object>()) {
    cout << "not an object!" << endl;
    return;
  }

  const object& obj = v.get<object>();
  for (auto i = obj.begin(); i != obj.end(); i++) {
    cout << i->first << ": " << i->second.to_str() << endl;
  }
}
