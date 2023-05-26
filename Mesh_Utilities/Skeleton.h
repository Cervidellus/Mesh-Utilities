#ifndef MESHUTILS_SKELETON
#define MESHUTILS_SKELETON

#include "mesh_types.h"

//Meant to interact between a CGAL skeleton and a meshparty skeleton. 
//A meshparty skeleton offers the following:
//vertices,
//edges,
//root = None,
//radius = None,
//mesh_to_skel_map = None,
//mesh_index = None,
//vertex_properties = {},
//node_mask = None,
//voxel_scaling = None,
//remove_zero_length_edges = True,
//skeleton_index = None,
//meta = {},

//meta{} includes:
//root_id: int = None
//soma_pt_x : float = None
//soma_pt_y : float = None
//soma_pt_z : float = None
//soma_radius : float = None
//collapse_soma : bool = None
//collapse_function : str = None
//invalidation_d : float = None
//smooth_vertices : bool = None
//compute_radius : bool = None
//shape_function : str = None
//smooth_iterations : int = None
//smooth_neighborhood : int = None
//smooth_r : float = None
//cc_vertex_thresh : int = None
//remove_zero_length_edges : bool = None
//collapse_params : dict = None
//timestamp : float = None
//skeleton_type : str = None
//meta : object = None


//In CGAL the skeleton is:
//typedef boost::adjacency_list< boost::vecS, boost::vecS, boost::undirectedS, Vmap > 	Skeleton
//The graph type representing the skeleton.
//The vertex property Vmap is a struct with a member point of type Traits::Point_3 and a member vertices of type std::vector<boost::graph_traits<TriangleMesh>::vertex_descriptor>.
//The boost documention should give some idea for how to add vertices and edges.
//I belive vecS is just a std::vector. 
// the first vecS I think is vertex list? 
// the second is the outedgelist?	
//boost::undirectedS just tells it to be an undirected graph.

//What do I want to hold in memory? I think the CGAL Skeleton type is ok. The VMAP doesn't apply to a meshparty skeleton though. Can I just leave it empty?

class Skeleton
{
public:
	Skeleton(std::shared_ptr<Point3Mesh> mesh);//will add params
	Skeleton();

	std::shared_ptr<Point3Mesh> sourceMesh = nullptr;//I don't think I need this as I Skeleton has a map.. but doesn't hurt to have it. 
	std::shared_ptr<Mean_Curvature_Flow_Skeleton> skeletonData;

	//properties
	int edgeCount();
	int vertexCount();
	
	//vertices()
	//edges()

	//IO
	//void toMesh(std::shared_ptr<Point3Mesh> mesh);
	//How do I want this done? Do I want it as points and edges, which would not be supported by most file formats?
	//Do I want to make a cylindar sphere model? 
	//For the latter, I could make some primitives in mesh_utilities for a sphere and cylinder

private:
	//std::shared_ptr<Point3Mesh> mesh_ = nullptr;//I don't think I need this as I Skeleton has a map.. but doesn't hurt to have it. 
	//std::shared_ptr<Mean_Curvature_Flow_Skeleton> skeleton_ = nullptr;

	void generateSkeleton_();
};
#endif 



