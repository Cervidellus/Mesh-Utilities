#include "Mesh.h"

#include "mesh_IO.h"

//need to make the mesh
Mesh::Mesh():mesh_(std::make_shared<Point3Mesh>()) {

}

void Mesh::fromPlyFile(std::string fileName) {
	meshutils::IO::read_PLY(fileName, mesh_);
}

void Mesh::toPlyFile(std::string fileName, bool binary) {
	meshutils::IO::write_PLY(fileName, mesh_, binary);
}

void Mesh::testPrint(int i) {
	std::cout << "Here is the int:" << i;
}