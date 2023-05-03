#ifndef MESHUTILS_MESH
#define MESHUTILS_MESH

#include "mesh_types.h"
//Will offer class methods to operate on that mesh, such as returning a skeleton (and holding it internally as a member)

class Mesh
{
public:
	Mesh();
	void testPrint(int i);//just to test the python interface

	//Properties
	//vertices
	// setVertices
	// vertexCount
	//faces
	// setFaces
	// faceCount
	//what else?

	//IO
	//void fromPlyFile(std::string fileName);
	//void toPlyFile(std::string fileName, bool binary = true);

	//Skeletonization
	//For this I will want it to return a skeleton object. It will store as a memeber shared pointer. If it does not exist, it will create it. 
	//This skeleton object should be able to return verts/edges and have it's own IO methods as I don't think the ones I have will work. 


private:
	std::shared_ptr<Point3Mesh> mesh_ = nullptr;
	std::shared_ptr<Mean_Curvature_Flow_Skeleton> skeleton_ = nullptr;
};
#endif

