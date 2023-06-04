#ifndef MESHUTILITIES_CGAL_TYPES
#define MESHUTILITIES_CGAL_TYPES

#include <CGAL/Surface_mesh_deformation.h>
#include <CGAL/Surface_mesh/Surface_mesh.h>
#include <CGAL/Surface_mesh_traits_generator_3.h>
#include <CGAL/extract_mean_curvature_flow_skeleton.h>
#include <CGAL/boost/graph/split_graph_into_polylines.h>
#include <CGAL/MP_Float.h>

typedef CGAL::Simple_cartesian<CGAL::MP_Float>                                              Kernel;
typedef Kernel::Point_3                                                             Point;
typedef Kernel::Vector_3                                                            Vector;
typedef Kernel::Segment_3                                                           Segment;
typedef CGAL::Surface_mesh <Point>                                                  Point3Mesh;
typedef boost::graph_traits<Point3Mesh>::vertex_descriptor                          vertex_descriptor;
typedef boost::graph_traits<Point3Mesh>::halfedge_descriptor                        halfedge_descriptor;
typedef boost::graph_traits<Point3Mesh>::face_descriptor                            face_descriptor;

typedef CGAL::Mean_curvature_flow_skeletonization<Point3Mesh>                       Skeletonization;
typedef Skeletonization::Skeleton                                                   Mean_Curvature_Flow_Skeleton;
typedef Mean_Curvature_Flow_Skeleton::vertex_descriptor                             Skeleton_vertex;
typedef Mean_Curvature_Flow_Skeleton::edge_descriptor                               Skeleton_edge;
typedef Mean_Curvature_Flow_Skeleton::edge_iterator                                 Skeleton_edge_iterator;
typedef Mean_Curvature_Flow_Skeleton::vertex_iterator                               Skeleton_vertex_iterator; 


typedef std::map<face_descriptor, double> Facet_double_map;

typedef CGAL::Surface_mesh_deformation<Point3Mesh>                                  Point3Mesh_deformation;
////typedef boost::property_map<Point3Mesh, CGAL::vertex_point_t>::type  VertexPointMap;
//typedef boost::property_map<Point3Mesh, CGAL::vertex_point_t>::type  Vertex_Property_Map;
//typedef Point3Mesh::template Property_map<vertex_descriptor, Vector> Vertex_Normal_Map;
//
//struct Bottom
//{
//    Bottom(Vertex_Property_Map pmap, Vertex_Normal_Map nmap, double vlen)
//        : pmap(pmap), nmap(nmap), vlen(vlen)
//    {}
//    void operator()(const vertex_descriptor& vin, const vertex_descriptor vout) const
//    {
//        boost::put(pmap, vout, boost::get(pmap, vout) - vlen * boost::get(nmap, vin));
//    }
//    Vertex_Property_Map pmap;
//    Vertex_Normal_Map nmap;
//    double vlen;
//};
//
//struct Top
//{
//    Top(VPMap pmap, VNMap nmap, double vlen)
//        : pmap(pmap), nmap(nmap), vlen(vlen)
//    {}
//    void operator()(const vertex_descriptor& vin, const vertex_descriptor vout) const
//    {
//        put(pmap, vout, get(pmap, vout) + vlen * get(nmap, vin));
//    }
//    VPMap pmap;
//    VNMap nmap;
//    double vlen;
//};






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