#ifndef MESHUTILITIES_UTILITIES
#define MESHUTILITIES_UTILITIES

#include "mesh_types.h"
#include "tinycolormap.hpp"

using namespace tinycolormap;

namespace meshutils {
	namespace primitives {
		std::shared_ptr<Point3Mesh> icosphere(const double radius, const int subdivisions, const Point center);
		
		std::shared_ptr<Point3Mesh> circle(const Point center, const double radius, const int num_vertices);

		std::shared_ptr<Point3Mesh> cylinder(const Point center, const double radius, const double height, const int num_vertices);
		std::shared_ptr<Point3Mesh> cylinder(const Segment segment, const double radius, const int num_vertices);
		//need an overload where it uses two points for hte segment
		//std::shared_ptr<Point3Mesh> cylinder(const Point source, const Point target, const double radius, const int num_vertices);
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