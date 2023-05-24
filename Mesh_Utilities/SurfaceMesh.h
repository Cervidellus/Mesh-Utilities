#ifndef MESHUTILS_MESH
#define MESHUTILS_MESH

#include "mesh_types.h"
//Will offer class methods to operate on that mesh, such as returning a skeleton (and holding it internally as a member)
//It's main purpose is to wrap CGAL's Surface_Mesh into an interface suitable for presenting to a Python module.

class SurfaceMesh
{
public:
	SurfaceMesh();
	SurfaceMesh(std::shared_ptr<Point3Mesh> mesh);

	void setMeshData(std::shared_ptr<Point3Mesh> mesh);
	std::shared_ptr<Point3Mesh> meshData();

	//add a constructor overload that takes a filepath

	//Properties
	//vertices
	// setVertices

	//just for testing
	int vertexCount();
	int faceCount();
	int edgeCount();
	//faces
	// setFaces
	// faceCount
	//what else?


	//Should I move the IO methods here? 
	//fromFile
	//toFile
	//fromMeshParty
	//fromTrimesh

	//Skeletonization
	//For this I will want it to return a skeleton object. It will store as a memeber shared pointer. If it does not exist, it will create it. 
	//This skeleton object should be able to return verts/edges and have it's own IO methods as I don't think the ones I have will work. 


private:
	std::shared_ptr<Point3Mesh> mesh_ = nullptr;
	std::shared_ptr<Mean_Curvature_Flow_Skeleton> skeleton_ = nullptr;
};
#endif

