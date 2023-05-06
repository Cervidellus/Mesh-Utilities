#ifndef MESHUTILITIES_FILE_IO
#define MESHUTILITIES_FILE_IO

#include "mesh_types.h"

//change it to a class with static methods for read and write, and private methods for reading and writing. 

namespace meshutils {
    namespace IO
    {
        //IO methods
        bool read(const std::string& filepath, std::shared_ptr<Point3Mesh> mesh);
        bool write(std::shared_ptr<Point3Mesh> mesh, const std::string& filepath);
    }
}

#endif