#include "Mesh.h"

#include "mesh_IO.h"

//need to make the mesh
Mesh::Mesh():mesh_(std::make_shared<Point3Mesh>()) {
}

void Mesh::testPrint(int i) {
	std::cout << "Here is the int:" << i;
}