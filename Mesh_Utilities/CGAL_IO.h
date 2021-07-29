#ifndef CGAL_TYPES_FILE_HANDLING
#define CGAL_TYPES_FILE_HANDLING

#include "CGAL_types.h"

namespace CGAL_IO
{
    //void read_off(const std::string& filepath);
    bool read_PLY(const std::string& filepath, Mesh& mesh);
    bool write_PLY(const std::string& filepath, Mesh& mesh, bool binary = true);
    bool write_PLY(const std::string& filepath, Skeleton& skeleton, bool binary = true);

    Mesh skel_to_mesh(const Skeleton& skeleton);

    //bool write_skeleton_PLY(
    //    const std::string& filepath,
    //    const Skeleton& skeleton,
    //    const bool binary = true);

    //void write_polyhedron_ply(
    //    const Polyhedron& polyhedron,
    //    const std::string& filepath,
    //    std::vector<double> sdf_values,
    //    std::vector<std::size_t> segment_ids,
    //    const bool binary = true);
}

#endif