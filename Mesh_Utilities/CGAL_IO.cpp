#include "CGAL_IO.h"

//#include <CGAL/boost/graph/Named_function_parameters.h>
#include <CGAL/boost/graph/named_params_helper.h>

using namespace std;
//error handling.. here I essentially do that with the bool, but better would be returning for example a CGAL_IO_ERROR type or something. 
//to me this is "c-like" in that I pass in what I expect to get out, rather than having it returned to me. 

//alternatively, I could return the mesh instead of a void, and throw a standard error.. requiring a try catch block in the more "scripting" part of the code. 

bool meshutils::IO::read_PLY(const std::string& filepath, Mesh& mesh)
{
    ifstream inputStream(filepath);
    std::string ply_comments;
    string current_line;
    getline(inputStream, current_line);
    if (current_line != "ply")
    {
        cout << "Error reading file." << endl;
        return false;
    };
    getline(inputStream, current_line);
    if (current_line.find("binary") != std::string::npos) {
        inputStream.close();
        inputStream.open(filepath, ios::binary);
        if (CGAL::IO::read_PLY(inputStream, mesh, ply_comments)) return true;
    }
    else
    {
        inputStream.clear();
        inputStream.seekg(0);
        if (CGAL::IO::read_PLY(inputStream, mesh, ply_comments)) return true;
    }
    cout << "Error reading file." << endl;
    return false;
}

bool meshutils::IO::write_PLY(const std::string& filepath, const Mesh& mesh, bool binary)
{
    if (binary) {
        ofstream outputStream(filepath, ios::binary);
        CGAL::IO::write_PLY(outputStream, mesh);
    }
    else
    {
        ofstream outputStream(filepath);
        CGAL::IO::write_PLY(outputStream, mesh);
    }
    return true;
}
