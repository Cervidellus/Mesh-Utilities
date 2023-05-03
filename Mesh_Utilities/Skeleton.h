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
	//toPly

private:
	std::shared_ptr<Point3Mesh> mesh_;
	std::shared_ptr<Mean_Curvature_Flow_Skeleton> skeleton_ = nullptr;

	void generateSkeleton_();
};
#endif 



