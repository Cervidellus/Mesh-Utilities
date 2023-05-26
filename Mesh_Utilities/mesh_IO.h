#ifndef MESHUTILITIES_FILE_IO
#define MESHUTILITIES_FILE_IO

#include "mesh_types.h"
#include "pybind11/pybind11.h"


//change it to a class with static methods for read and write, and private methods for reading and writing. 
namespace py = pybind11;

class SurfaceMesh;
class Skeleton;

namespace meshutils {
    namespace IO
    {
        //IO methods
        bool read(const std::string& filepath, std::shared_ptr<Point3Mesh> mesh);
        bool write(std::shared_ptr<Point3Mesh> mesh, const std::string& filepath);

        SurfaceMesh surfaceMeshFromMeshpartyMesh(const py::object &meshparty_mesh);
        SurfaceMesh surfaceMeshFromSkeleton(std::shared_ptr<Point3Mesh> mesh, double vertexRadius, double edgeRadius);

        Skeleton skeletonFromMeshpartySkeleton(const py::object& meshparty_skeleton);

        //TODO
        //py::object meshpartySkeletonFromSkeleton(const Skeleton& skeleton);
    }
}

#endif