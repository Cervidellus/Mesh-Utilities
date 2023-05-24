#include "SurfaceMesh.h"

#include "mesh_IO.h"

//need to make the mesh
//SurfaceMesh::SurfaceMesh() {};

SurfaceMesh::SurfaceMesh():mesh_(std::make_shared<Point3Mesh>()) {
}

SurfaceMesh::SurfaceMesh(std::shared_ptr<Point3Mesh> mesh)
{
	mesh_ = mesh;
}

int SurfaceMesh::vertexCount()
{
	return mesh_->number_of_vertices();
}

int SurfaceMesh::faceCount()
{
	return mesh_->number_of_faces();
}

int SurfaceMesh::edgeCount()
{
	return mesh_->number_of_edges();
}
