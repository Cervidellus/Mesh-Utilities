#ifndef MESHUTILS_PYTHON
#define MESHUTILS_PYTHON

#include "SurfaceMesh.h"
#include "mesh_utilities.h"

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

PYBIND11_MODULE(meshutils, m) {
    py::class_<SurfaceMesh>(m, "Mesh")
        .def(py::init<>())
        .def("vertexCount", &SurfaceMesh::vertexCount, "return an int representing the number of vertices in the object")
        .def("faceCount", &SurfaceMesh::faceCount, "return an int representing the number of faces in the object")
        .def("edgeCount", &SurfaceMesh::edgeCount, "return an int representing the number of edges in the object");

    auto mPrimitives = m.def_submodule("primitives");

    mPrimitives.def("Icosphere", [](int radius, int subdivisions, py::array_t<double> center_point) {
        double* pointData = (double*)center_point.request().ptr;
        return SurfaceMesh(meshutils::primitives::icosphere(radius, 3, Point(pointData[0], pointData[1], pointData[3])));
        },
        "Returns a sphere constructed similarly to a Blender icosphere. It is constructed by first subdividing an icosahedron, then projecting the vertices onto a sphere of the correct radius.",
        py::arg("radius") = 5,
        py::arg("subdivisions") = 3,
        py::arg("center_point") = py::array_t<double>(3,std::vector<double>{4,5,6}.data()));
}

#endif