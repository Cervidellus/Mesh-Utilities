#ifndef MESHUTILS_PYTHON
#define MESHUTILS_PYTHON

#include "Skeleton.h"
#include "SurfaceMesh.h"
#include "mesh_utilities.h"
#include "mesh_IO.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

PYBIND11_MODULE(meshutils, m) {
    py::class_<SurfaceMesh>(m, "Mesh")
        .def(py::init<>())
        .def("vertexCount", &SurfaceMesh::vertexCount, "return an int representing the number of vertices in the object")
        .def("faceCount", &SurfaceMesh::faceCount, "return an int representing the number of faces in the object")
        .def("edgeCount", &SurfaceMesh::edgeCount, "return an int representing the number of edges in the object");

    py::class_<Skeleton>(m, "Skeleton")
        .def(py::init<>())
        .def("edgeCount", &Skeleton::edgeCount, "return an int representing the number of edges in the object")
        .def("vertexCount", &Skeleton::vertexCount, "return an int representing the number of vertices in the object")
        .def("vertices", 
            [](Skeleton& skeleton){
                //We first make an array, then we shape it to represent 3 dimensional vertices.
                auto vertices = skeleton.vertices();
                py::print(vertices.size(), " size of vertices.");
                auto pyvertices = py::array_t<double>(vertices.size() * vertices[0].size());
                auto pyvertices_buffer = pyvertices.mutable_data();
                for (auto& point : vertices) {
                    std::memcpy(pyvertices_buffer, point.data(), point.size() * sizeof(double));
                    pyvertices_buffer += point.size();
                }
                std::vector<py::ssize_t> shape = { static_cast<py::ssize_t>(vertices.size()), static_cast<py::ssize_t>(vertices[0].size()) };
                return pyvertices.reshape(shape).attr("copy")();
            },
            "return an array containing the vertices in the skeleton.")
        .def("edges",
            [](Skeleton& skeleton) {
                auto edges = skeleton.edges();
                py::print(edges.size(), " size of edges.");
                auto pyedges = py::array_t<int>(edges.size() * 2);
                auto pyedgesUnchecked = pyedges.mutable_unchecked<1>();

                for (int i = 0; i < edges.size(); i++) {
                    
                    pyedgesUnchecked[i * 2] = edges[i].first;
                    pyedgesUnchecked[i * 2 + 1] = edges[i].second;
                }
                std::vector<py::ssize_t> shape = { static_cast<py::ssize_t>(edges.size()),2 };
                return pyedges.reshape(shape).attr("copy")();
            },
            "return an array containing vertex index pairs representing the edges in the skeleton.")
        .def("toSurfaceMesh",
            &Skeleton::toSurfaceMesh,
            "Generates spheres for vertices and cylinders for edges in the skeleton, and returns them a SurfaceMesh.");

    auto submoduleIO = m.def_submodule("IO");

    submoduleIO.def("read", 
        [](std::string filepath) {
        std::shared_ptr<Point3Mesh> mesh = std::make_shared<Point3Mesh>();
        meshutils::IO::read(filepath, mesh);
        return SurfaceMesh(mesh);},
        "read a surface mesh object from file.",
        py::arg("filepath"));

    submoduleIO.def("write", 
        [](std::string filepath, SurfaceMesh mesh) {
        meshutils::IO::write(mesh.meshData(), filepath);},
        "read a surface mesh object from file.",
        py::arg("filepath"),
        py::arg("mesh"));

    submoduleIO.def("SurfaceMeshFromTrimesh",
        &meshutils::IO::surfaceMeshFromMeshpartyMesh,
        "Surface mesh from a trimesh or meshparty mesh.",
        py::arg("mesh"));

    submoduleIO.def("SkeletonFromMeshpartySkeleton",
        &meshutils::IO::skeletonFromMeshpartySkeleton,
        "Skeleton from a Meshparty skeleton.",
        py::arg("skeleton"));

    auto submodulePrimitives = m.def_submodule("primitives");

    submodulePrimitives.def("icosphere", [](int radius, int subdivisions, py::array_t<long double> center_point) {
        long double* pointData = (long double*)center_point.request().ptr;
        return SurfaceMesh(meshutils::primitives::icosphere(radius, subdivisions, Point(pointData[0], pointData[1], pointData[2])));
        },
        "Returns a sphere constructed similarly to a Blender icosphere. It is constructed by first subdividing an icosahedron, then projecting the vertices onto a sphere of the correct radius.",
        py::arg("radius") = 5,
        py::arg("subdivisions") = int(3),
        py::arg("center_point") = py::array_t<long double>(3,std::vector<long double>{0,0,0}.data()));
}

#endif