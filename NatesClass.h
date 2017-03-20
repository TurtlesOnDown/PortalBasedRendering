#pragma once
class Plane {
public:
  //construct a plane from a position and a normal 
  Plane(Vect pos, Vect norm);
  //construct a plane from a list of vertices
  Plane(Vector<Vect> points);
  ~Plane();

  //get the texture
  Tex& getTex();
  //set the texture if it is valid
  void setTex(Tex&);

  //get the plane corresponding to the other side of the portal
  //if this isn't a portal, it is null
  Plane& getLink();
  //set a new portal location for this plane
  //if this was already a portal, update the other side of the portal
  //so that it is now just a normal plane
  void setLink(Plane&);
  //move the plane (maybe...?)
  void move(Rot, Vect);
  //continuous version
  boost::coroutines::coroutine<void(Rot, Vect, Double)> moveOverTime;

private:
  //which form of the data is fresh.
  //if we move geometry around, it may be preferable to update one
  //representation or another depending on what we are doing.
  bool freshVerts;
  bool freshPosNorm;

  //previous state is needed for updates
  Vect prev_normal;
  Vect normal;
  Vect prev_point_on_plane;
  Vect point_on_plane;
  Vector<Vect> prev_verts
    Vector<Vect> verts;

  //the texture
  Tex& tex;
  //the other side of the portal (null if not a portal)
  Plane& link;
  //the sector this face belongs to
  //needed by the camera when crossing through a portal
  const Sector& parent;

  //calculate position-normal representation from vertices representation
  //done as an example. I don't know if we would actually inline that.
  void calcPosNorm() {
    if (!freshPosNorm) {
      prev_point_on_plane = point_on_plane;
      point_on_plane = verts[0];
      prev_normal = normal;
      normal = (verts[1] - verts[0]).cross(verts[-1] - verts[0]);
      freshPosNorm = True;
    }
  }

  //calculate vertices representation from position-normal representation
  //this is potentially really hard.
  void calcVerts();
}

class CamWrapper {
public:
  //obvious constructor
  CamWrapper(Camera&, Sector&);
  //obvious getters
  Camera& getCamera();
  Sector& getCurrentSector();
  //check if the camera has passed through a portal and
  //modify its sector if so
  void updateCurrentSector();

private:
  Camera& camera;
  Sector& currentSector;
}

class Sector {
public:
  //constructors...?
  //destructor...?

  //just map of the plane move method
  void move(Rot r, Vect v) { faces.map([](Plane p) {p.move(r, v)}) };
  //if it goes outward, it shouldn't be that hard to do.
  //inward is a different story.
  void extrude(&World world, Int index, Vect amount) { return bevel(world, index, amount, 0) };
  //same as above.
  void bevel(&World, Int, Vect, Double);
  //continuous versions
  boost::coroutines::coroutine<void(&World, Int, Vect, Double)> extrudeOverTime;
  boost::coroutines::coroutine<void(&World, Int, Vect, Double, Double)> bevelOverTime;

private:
  Vector<Plane> faces;
}

typedef World = Vector<Sector>;