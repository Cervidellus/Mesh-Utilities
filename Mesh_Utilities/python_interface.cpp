#ifndef MESHUTILS_PYTHON
#define MESHUTILS_PYTHON

#include "Mesh.h"
#include "pybind11/pybind11.h"

namespace py = pybind11;

//PYBIND11_MODULE(meshy, m) {
//    py::class_<Mesh>(m, "Mesh")
//        .def(py::init<>())
//        .def("getInt", &Mesh::getInt, "A function that returns an int")
//        .def("setInt", &Mesh::setInt, "Hey, set the int!")
//        .def("multiplyInt", &Mesh::multiplyInt, "MULTIPLY");
//}

void tesfunction() {
    Mesh mesh;//valid class.. 
}

PYBIND11_MODULE(meshutils, m) {
    py::class_<Mesh>(m, "Mesh")
        .def(py::init<>())
        .def("printInt", &Mesh::testPrint, "junk")//works?
        .def("fromPlyFile", &Mesh::fromPlyFile, "read a mesh from a .ply file")
        .def("toPlyFile", &Mesh::toPlyFile, "write a mesh to a .ply file");
}

#endif