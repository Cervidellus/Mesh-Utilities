#include "mesh_IO.h"
#include "helper_utils.h"
#include "SurfaceMesh.h"

#include <filesystem>
#include <CGAL/boost/graph/named_params_helper.h>
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

    if (!(lowerCaseExtension == ".obj" || lowerCaseExtension == ".ply" || lowerCaseExtension == ".off")) // Heed the dot.
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
    return CGAL::IO::write_polygon_mesh(filepath, *mesh);
}

SurfaceMesh meshutils::IO::fromMeshpartyMesh(const py::object& meshparty_mesh)
{
    //py::array_t<double> meshpartyVerts = meshparty_mesh.attr("vertices").cast<py::array_t<double>>();
    //py::array_t<int> meshpartyFaces = meshparty_mesh.attr("faces").cast<py::array_t<int>>();
    //pybind offers faster access when we make assumptions about the shape using the unchecked method.
    //auto uncheckedVerts = meshpartyVerts.unchecked<3>();
    //auto uncheckedFaces = meshpartyFaces.unchecked<3>();

    SurfaceMesh mesh = SurfaceMesh();
    std::shared_ptr<Point3Mesh> meshData = mesh.meshData();
    
    //for (int i = 0; i < uncheckedVerts.shape(0); i++)
    //    for(int j = 0; j < uncheckedVerts.shape(1); j++)
    //        for (int k = 0; k < uncheckedVerts.shape(2); k++) {
    //            //below causes a failure to compile with 'Invalid number of indices for unchecked array reference
    //            auto x = uncheckedVerts(3.25);
    //            meshData->add_vertex(Point(uncheckedVerts(i), uncheckedVerts(j), uncheckedVerts(k)));
    //        }

//Mesh trimesh_to_surface_mesh(py::object & trimesh) {
    //auto vertices = meshparty_mesh.attr("vertices").cast<py::array_t<float>>();
    //auto faces = meshparty_mesh.attr("faces").cast<py::array_t<uint32_t>>();

    //Mesh mesh;

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




    //somehow unchecked is giving me an issue.
    //std::vector<Point3Mesh::vertex_index> vertex_indices;
    //vertex_indices.reserve(meshpartyVerts.shape(0));
    //auto vertices_unchecked = meshpartyVerts.unchecked<3>();
    //for (int i = 0; i < meshpartyVerts.shape(0); ++i) {
    //    auto point = Point(vertices_unchecked(i, 0), vertices_unchecked(i, 1), vertices_unchecked(i, 2));
    //    vertex_indices.push_back(meshData->add_vertex(point));
    //}

    //auto faces_unchecked = meshpartyFaces.unchecked<2>();
    //for (int i = 0; i < meshpartyFaces.shape(0); ++i) {
    //    std::vector<Point3Mesh::Vertex_index> face_vertices;
    //    face_vertices.reserve(meshpartyFaces.shape(1));
    //    for (int j = 0; j < meshpartyFaces.shape(1); ++j) {
    //        face_vertices.push_back(vertex_indices[faces_unchecked(i, j)]);
    //    }
    //    meshData->add_face(face_vertices);
    //}
    return mesh;
}
 