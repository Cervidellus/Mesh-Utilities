#ifndef MESHUTILITIES_UTILITIES
#define MESHUTILITIES_UTILITIES

#include "mesh_types.h"
#include "tinycolormap.hpp"

using namespace tinycolormap;

namespace meshutils {
	//helper methods
	Point3Mesh skel_to_mesh(const Skeleton& skeleton);
	//coloring
	Point3Mesh color_skel_to_mesh(Skeleton& skeleton, Point3Mesh& mesh);
	bool color_verts_from_face_property(Point3Mesh& mesh, const Facet_with_id_pmap<double>& face_map, ColormapType colormap = ColormapType::Viridis);
}

#endif