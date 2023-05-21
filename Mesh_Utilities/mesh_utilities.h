#ifndef MESHUTILITIES_UTILITIES
#define MESHUTILITIES_UTILITIES

#include "mesh_types.h"
#include "tinycolormap.hpp"

using namespace tinycolormap;

namespace meshutils {
	namespace primitives {
		Point3Mesh icosphere(const double radius, const int subdivisions, const Point center);
		//circle
		
		Point3Mesh circle(const Point center, const double radius, const int num_vertices);
		//cylinder

		Point3Mesh cylinder(const Point center, const double radius, const double height, const int num_vertices);
		Point3Mesh cylinder(const Segment segment, const double radius, const int num_vertices);

		//I want to provide overloads for circle and cylinder that will take a vector and produce an oriented circle and cylinder between the souce and target of the vector, respectively

	}

	namespace subdivision {
		void loop_subdivision(Point3Mesh& mesh, int subdivisions);
	}
	
	//helper methods
	Point3Mesh skel_to_mesh(const Mean_Curvature_Flow_Skeleton& skeleton);//this is going to change
	//coloring
	Point3Mesh color_skel_to_mesh(Mean_Curvature_Flow_Skeleton& skeleton, Point3Mesh& mesh);
	bool color_verts_from_face_property(Point3Mesh& mesh, const Facet_with_id_pmap<double>& face_map, ColormapType colormap = ColormapType::Viridis);
}

#endif