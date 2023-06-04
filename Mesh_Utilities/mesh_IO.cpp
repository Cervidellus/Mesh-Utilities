#include "mesh_IO.h"
#include "helper_utils.h"
#include "SurfaceMesh.h"
#include "Skeleton.h"

#include <filesystem>
#include <CGAL/boost/graph/named_params_helper.h>
#include <CGAL/Named_function_parameters.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

using namespace std;
//error handling.. here I essentially do that with the bool, but better would be returning for example a CGAL_IO_ERROR type or something. 
//to me this is "c-like" in that I pass in what I expect to get out, rather than having it returned to me. 

//alternatively, I could return the mesh instead of a void, and throw a standard error.. requiring a try catch block in the more "scripting" part of the code. 

bool meshutils::IO::read(const std::string& filepath, std::shared_ptr<Point3Mesh> mesh) {
    //verify valid filepath, with supported type. 
    std::filesystem::path filePath = filepath;
    std::string lowerCaseExtension = helpers::str_tolower(filePath.extension().string());

    if (!(lowerCaseExtension == ".obj" || lowerCaseExtension == ".ply" || lowerCaseExtension == ".off")) // Heed the dot.
    {
        std::cout << lowerCaseExtension << " is an invalid file extension."; 
        return false;
    }

    if (!std::filesystem::exists(filePath)) {
        std::cout << "File does not exist.\n";
        return false;
    }

    //read 
    bool success = CGAL::IO::read_polygon_mesh<Point3Mesh>(filepath, *mesh);

    if (!success)
    {
        std::cout << "Failed to read " << filepath;
        return false;
    }
    return true;
}

bool meshutils::IO::write(std::shared_ptr<Point3Mesh> mesh, const std::string& filepath) {
    if (!mesh) return 0;

    //verify valid filepath, with supported type. 
    std::filesystem::path filePath = filepath;
    std::string lowerCaseExtension = helpers::str_tolower(filePath.extension().string());

    if (!(lowerCaseExtension == ".obj" || lowerCaseExtension == ".ply" || lowerCaseExtension == ".off")) 
    {
        std::cout << lowerCaseExtension << " is an invalid file extension.";
        return false;
    }

    if (filePath.parent_path() == "") filePath = std::filesystem::current_path() /= filepath;

    if (!std::filesystem::exists(filePath.parent_path())) {
        std::cout << "Invalid output directory.\n";
        return false;
    }

    //write
    return CGAL::IO::write_polygon_mesh(
        filepath, 
        *mesh, 
        CGAL::parameters::stream_precision(12));
}

SurfaceMesh meshutils::IO::surfaceMeshFromMeshpartyMesh(const py::object& meshparty_mesh)
{
    SurfaceMesh mesh = SurfaceMesh();
    std::shared_ptr<Point3Mesh> meshData = mesh.meshData();

    auto vertices = meshparty_mesh.attr("vertices").cast<py::array_t<double>>();
    auto faces = meshparty_mesh.attr("faces").cast<py::array_t<uint32_t>>();

    std::vector<Point3Mesh::Vertex_index> vertex_indices;
    vertex_indices.reserve(vertices.shape(0));
    for (int i = 0; i < vertices.shape(0); ++i) {
        auto point = Point(vertices.at(i, 0), vertices.at(i, 1), vertices.at(i, 2));
        vertex_indices.push_back(meshData->add_vertex(point));
    }
    for (int i = 0; i < faces.shape(0); ++i) {
        std::vector<Point3Mesh::Vertex_index> face_vertices;
        face_vertices.reserve(faces.shape(1));
        for (int j = 0; j < faces.shape(1); ++j) {
            face_vertices.push_back(vertex_indices[faces.at(i, j)]);
        }
        meshData->add_face(face_vertices);
    }
    return mesh;
}

Skeleton meshutils::IO::skeletonFromMeshpartySkeleton(const py::object& meshparty_skeleton)
{
    Skeleton skeleton = Skeleton();
    auto vertices = meshparty_skeleton.attr("vertices").cast<py::array_t<double>>();
    auto edges = meshparty_skeleton.attr("edges").cast<py::array_t<int>>();

    for (int i = 0; i < vertices.shape(0); ++i) {
        auto point = Point(vertices.at(i, 0), vertices.at(i, 1), vertices.at(i, 2));
        Skeleton_vertex v = boost::add_vertex(*skeleton.skeletonData);
        skeleton.skeletonData->operator[](v).point = point;
    }

    for (int i = 0; i < edges.shape(0); ++i) {
        Skeleton_vertex v = edges.at(i,0);
        Skeleton_vertex w = edges.at(i,1);
        auto e = boost::add_edge(v, w, *skeleton.skeletonData);
    }

    return skeleton;
}
 