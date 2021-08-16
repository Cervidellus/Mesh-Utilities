#ifndef MY_CGAL_TYPES
#define MY_CGAL_TYPES

#include <CGAL/Surface_mesh/Surface_mesh.h>
#include <CGAL/Surface_mesh_traits_generator_3.h>
#include <CGAL/extract_mean_curvature_flow_skeleton.h>
#include <CGAL/boost/graph/split_graph_into_polylines.h>

typedef CGAL::Simple_cartesian<double>                                              Kernel;
typedef Kernel::Point_3                                                             Point;
typedef CGAL::Surface_mesh <Point>                                                  Mesh;
typedef boost::graph_traits<Mesh>::vertex_descriptor                                vertex_descriptor;
typedef boost::graph_traits<Mesh>::halfedge_descriptor                              halfedge_descriptor;
typedef boost::graph_traits<Mesh>::face_descriptor                                  face_descriptor;

typedef CGAL::Mean_curvature_flow_skeletonization<Mesh>                             Skeletonization;
typedef Skeletonization::Skeleton                                                   Skeleton;
typedef Skeleton::vertex_descriptor                                                 Skeleton_vertex;
typedef Skeleton::edge_descriptor                                                   Skeleton_edge;
typedef Skeleton::edge_iterator                                                     Skeleton_edge_iterator;

typedef std::map<face_descriptor, double> Facet_double_map;

// Property map associating a facet with an integer as id to an
// element in a vector stored internally
template<class ValueType>
struct Facet_with_id_pmap
    : public boost::put_get_helper<ValueType&,
    Facet_with_id_pmap<ValueType> >
{
    typedef face_descriptor key_type;
    typedef ValueType value_type;
    typedef value_type& reference;
    typedef boost::lvalue_property_map_tag category;
    Facet_with_id_pmap(
        std::vector<ValueType>& internal_vector
    ) : internal_vector(internal_vector) { }

    reference operator[](key_type key) const
    {
        return internal_vector[key.idx()];
    }
private:
    std::vector<ValueType>& internal_vector;
};
#endif