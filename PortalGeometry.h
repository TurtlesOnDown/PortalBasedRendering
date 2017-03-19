#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Camera.h"

constexpr int TEXTURE_COUNT = 1; //TODO change this to actually keep track of the number of textures!
class Sector;
class XPlane;

struct XVertex {
	// Position
	glm::vec3 Position;
	// TexCoords
	glm::vec2 TexCoords;
};

class  XPlane {
	public:
		//construct a plane from its transform
		XPlane(const glm::mat4& tf, int tx, XPlane* lk, Sector* prnt) : trans(tf), link(lk), parent(prnt) {
			tex = (tx >= TEXTURE_COUNT) ? 0 : tx;
			verts = {
				{ { 1000.f, 1000.f,0 },{ 1,1 } },
				{ { 1000.f,-1000.f,0 },{ 1,0 } },
				{ {-1000.f,-1000.f,0 },{ 0,0 } },
				{ {-1000.f, 1000.f,0 },{ 0,1 } },
			};
		}
		//TODO add a constructor to take in vertex info instead
		//default destructor is fine?
	
		//get the texture
		int getTex() {return tex;}
		//set the texture if it is valid
		void setTex(int tx) {if (tx >= TEXTURE_COUNT) tex = tx;}
		
		//get the parent sector
		Sector* getParent() {return parent;}
		//get the plane corresponding to the other side of the portal
		XPlane* getLink() {return link;}
		//modify the portals in some way
		void setLinkRaw(XPlane* newlink) {link = newlink;}
		void setLinkTwoWayRaw(XPlane* newlink) {link = newlink; newlink->link = this;}
		void setLink(XPlane* newlink) {removePortalFromCycle(link, this); link = newlink;}
		void setLinkTwoWay(XPlane* newlink) {
			setLink(newlink);
			newlink->setLink(this);
		}
		
		//get the transform
		glm::mat4 getTransform() {return trans;}
		//fix the plane to a specific transform in world space
		void setTransform(const glm::mat4& m) {trans = m;}
		//move the plane using a transform in world space 
		void move(const glm::mat4& m) {trans = m * trans;}

		//===========STUFF FROM MESH.h=================
		/*  Functions  */
		// Render the mesh
		void Draw(Shader shader);
		//==============================================

	private:
		//The transform that defines the orientation of the plane.
		//Colums are in formatt (n, b, b_cross_n, p)
		//with an additional bottom row of (0, 0, 0, 1)
		//(i.e. an affine transformation in homogeneous coordinates)
		glm::mat4 trans;

		//the index of the texture to use for this plane
		int tex;
		
		//the other side of the portal (nullptr if not a portal)
		XPlane* link;

		//the sector this face belongs to
		//needed by the camera when crossing through a portal
		Sector* parent;
		
		// suppose a ~> end, end ~> b, b ~> c, c ~> a
		// results in a ~> newlink, end ~> b, b ~> c, c ~> a
		// so long as this is a, b, c, d, or end
		void removePortalFromCycle(XPlane* newlink, XPlane* end) {
			if (this->link == end) {
				link = newlink;
			}
			else {
				this->link->removePortalFromCycle(newlink, end);
			}
		}

		//============STUFF FROM MESH.h====================
		/*  Mesh Data  */
		//TODO convert this into something more sensible
		//e.g. pointer to some substructure of a polygon soup
		vector<XVertex> verts;
		GLuint indices[6] = { 0,1,3,1,2,3 };

		/*  Render data  */
		//vertex array object, vertex buffer object, something buffer object?
		GLuint VAO, VBO, EBO;

		/*  Functions    */
		// Initializes all the buffer objects/arrays
		void setupMesh();
		//=================================================
};

class Sector {
	friend class CamWrapper;
	public:
		//constructor takes in a vector of pointers to XPlanes
		Sector(const vector<XPlane*>& fs) {faces = fs;}
		//destructor...? TODO? Is this what we want?
		~Sector() {
			for (auto p = faces.begin(); p != faces.end(); ++p) {
				delete *p;
			}
		}
		
		//iterates over the planes in the sector, moving them
		void move(const glm::mat4& t) {for (auto p = faces.begin(); p != faces.end(); ++p) {(*p)->move(t);}}
				
	private:
		vector<XPlane*> faces;
};

typedef vector<Sector> World;


/*
class CamWrapper {
	public:
		//obvious constructor
		CamWrapper (Camera& c, Sector& s) : camera(c), currentSector(s) {}
		//default destructor is fine?
		
		//obvious getters
		Camera& getCamera() {return camera;}
		Sector& getCurrentSector() {return currentSector;}
		//check if the camera has passed through a portal and
		//modify its sector if so
		void updateCurrentSector();
		void renderWorld(glm::mat4 projection) {renderWorld(projection, camera.GetViewMatrix(), currentSector);}
	private:
		Camera& camera;
		Sector& currentSector;
		void renderWorld(glm::mat4, glm::mat4, Sector&);
};

void CamWrapper::updateCurrentSector() {
	glm::vec4 cam_pos = glm::vec4(camera.Position, 1);
	for (auto p = currentSector.faces->begin(); p != currentSector.faces->end; ++p) {
		glm::mat4 t = p->getTransform();
		//if behind portal...
		if (glm::dot(glm::column(t, 0),cam_pos - glm::column(t, 3)) < 0) {
			//...go to the sector on the other side 
			currentSector = p->getParent();
			return;
		}
	}
}

void CamWrapper::renderWorld(glm::mat4 currentProjectionMatrix, glm::mat4 currentViewMatrix, Sector& sectorToRender) {
	vector<XPlane&> truePortals;;

	for (auto p = sectorToRender.faces.begin(); p != sectorToRender.faces.end(); ++p) {
		//transform p->getVerts() and add to vertex buffer

		if (p->link != nullptr) {
			truePortals.push_back(*p);
		}
		else {
			//draw it!
		}
	}

	//draw the screen
	for (auto p = truePortals.begin(); p != truePortals.end(); ++p) {
		//draw as a stencil
		//if the area is > 1px
		//	renderWorld(glm::mat4(-1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1) * p->getTransform(), p->link->parent) 
	}
	
	delete onscreenPortalMap;
}
*/


