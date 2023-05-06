#ifndef MESHUTILS_SKELETON
#define MESHUTILS_SKELETON

#include "mesh_types.h"

class Skeleton
{
public:
	Skeleton(std::shared_ptr<Point3Mesh> mesh);//will add params

	//properties
	//vertices()
	//edges()

	//IO
	void toMesh(std::shared_ptr<Point3Mesh> mesh);
	//How do I want this done? Do I want it as points and edges, which would not be supported by most file formats?
	//Do I want to make a cylindar sphere model? 
	//For the latter, I could make some primitives in mesh_utilities for a sphere and cylinder

private:
	std::shared_ptr<Point3Mesh> mesh_;
	std::shared_ptr<Mean_Curvature_Flow_Skeleton> skeleton_ = nullptr;

	void generateSkeleton_();
};
#endif 



