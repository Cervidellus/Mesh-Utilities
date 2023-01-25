#ifndef MESHUTILITIES_FILE_IO
#define MESHUTILITIES_FILE_IO

#include "CGAL_types.h"
#include "tinycolormap.hpp"

using namespace tinycolormap;

namespace meshutils {
    namespace IO
    {
        //IO methods
        bool read_PLY(const std::string& filepath, Mesh& mesh);
        bool write_PLY(const std::string& filepath, const Mesh& mesh, const bool binary = true);
    }
}

#endif