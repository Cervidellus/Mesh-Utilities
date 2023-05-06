#include "mesh_IO.h"
#include "helper_utils.h"

#include <filesystem>
#include <CGAL/boost/graph/named_params_helper.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>

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