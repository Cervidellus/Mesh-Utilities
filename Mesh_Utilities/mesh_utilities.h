#ifndef MESHUTILITIES_UTILITIES
#define MESHUTILITIES_UTILITIES

#include "CGAL_types.h"
#include "tinycolormap.hpp"

using namespace tinycolormap;

namespace meshutils {
	//helper methods
	Mesh skel_to_mesh(const Skeleton& skeleton);
	//coloring
	Mesh color_skel_to_mesh(Skeleton& skeleton, Mesh& mesh);
	bool color_verts_from_face_property(Mesh& mesh, const Facet_with_id_pmap<double>& face_map, ColormapType colormap = ColormapType::Viridis);
}

#endif