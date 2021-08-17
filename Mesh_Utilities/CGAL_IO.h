#ifndef CGAL_TYPES_FILE_HANDLING
#define CGAL_TYPES_FILE_HANDLING

#include "CGAL_types.h"
#include "tinycolormap.hpp"

using namespace tinycolormap;
namespace CGAL_IO
{
    //IO methods
    bool read_PLY(const std::string& filepath, Mesh& mesh);
    bool write_PLY(const std::string& filepath, const Mesh& mesh, const bool binary = true);

    //helper methods
    Mesh skel_to_mesh(const Skeleton& skeleton);
    bool color_verts_from_face_property(Mesh& mesh, const Facet_with_id_pmap<double>& face_map, ColormapType colormap = ColormapType::Viridis);

}

#endif