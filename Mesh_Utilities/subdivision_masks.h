#ifndef SUBDIVISION_MASKS
#define SUBDIVISION_MASKS

#include "mesh_types.h"

#include <cmath>

class Spherical_Loop_mask {

public:
    typedef Point3Mesh PolygonMesh;
    typedef typename boost::property_map<Point3Mesh, CGAL::vertex_point_t>::type VertexPointMap;
    typedef typename boost::property_traits<VertexPointMap>::value_type Point_value;
    typedef typename boost::property_traits<VertexPointMap>::reference Point_ref;
    typedef CGAL::Halfedge_around_target_circulator<Point3Mesh> Halfedge_around_vertex_circulator;
    typedef typename Kernel::FT FT;

    Point3Mesh& mesh;
    Point center;
    VertexPointMap vpm;
    double squared_radius;

    //I think I can use CGAL::Origin as the center
    Spherical_Loop_mask(Point3Mesh& mesh, const Point center, const double radius) :mesh(mesh), center(center), squared_radius(radius*radius), vpm(CGAL::get(boost::vertex_point_t::vertex_point, mesh)) {};

    void edge_node(halfedge_descriptor halfedge, Point& pt) {
        Point_ref p1 = boost::get(vpm, CGAL::target(halfedge, mesh));
        Point_ref p2 = boost::get(vpm, CGAL::target(opposite(halfedge, mesh), mesh));
        Point_ref f1 = boost::get(vpm, CGAL::target(next(halfedge, mesh), mesh));
        Point_ref f2 = boost::get(vpm, CGAL::target(next(opposite(halfedge, mesh), mesh), mesh));
        
        pt = Point((3 * (p1[0] + p2[0]) + f1[0] + f2[0]) * 0.125,
            (3 * (p1[1] + p2[1]) + f1[1] + f2[1]) * 0.125,
            (3 * (p1[2] + p2[2]) + f1[2] + f2[2]) * 0.125);

        //project points to sphere
        double scalar = sqrt(abs(squared_radius / CGAL::to_double(CGAL::squared_distance(center, pt))));
        pt = Point(((pt[0]-center[0]) * scalar) + center[0], 
            ((pt[1]-center[1]) * scalar) + center[1], 
            ((pt[2] - center[2]) * scalar) + center[2]);
    }

    void vertex_node(vertex_descriptor vertex, Point& pt) {
        Halfedge_around_vertex_circulator vcir(vertex, mesh);
        size_t n = circulator_size(vcir);
        CGAL_assume(n > 0);

        FT R[] = { 0.0, 0.0, 0.0 };
        Point_ref S = get(vpm, vertex);

        for (size_t i = 0; i < n; i++, ++vcir) {
            Point_ref p = get(vpm, CGAL::target(opposite(*vcir, mesh), mesh));
            R[0] += p[0];         R[1] += p[1];         R[2] += p[2];
        }
        if (n == 6) {
            pt = Point((10 * S[0] + R[0]) * 0.0625, (10 * S[1] + R[1]) * 0.0625, (10 * S[2] + R[2]) * .0625);
        }
        else {
            const FT Cn = (FT)(5.0 / 8.0 - CGAL::square(3 + 2 * std::cos(2 * CGAL_PI / (double)n)) / 64.0);

            const FT Sw = CGAL::approximate_division((double)n * (1 - Cn),  Cn);
            const FT W = CGAL::approximate_division((double)n, Cn);
            //pt = Point(CGAL::approximate_division(Sw * S[0] + R[0]) / W, (Sw * S[1] + R[1]) / W, (Sw * S[2] + R[2]) * 0.015625);
            pt =Point(CGAL::approximate_division(Sw * S[0] + R[0], W),
                CGAL::approximate_division(Sw * S[1] + R[1], W),
                (Sw * S[2] + R[2]) * 0.015625);
        }

        //project points to sphere
        CGAL::MP_Float scalar = CGAL::approximate_sqrt(abs(CGAL::approximate_division(squared_radius, CGAL::squared_distance(center, pt))));
        pt = Point(((pt[0] - center[0]) * scalar) + center[0],
            ((pt[1] - center[1]) * scalar) + center[1],
            ((pt[2] - center[2]) * scalar) + center[2]);
    }

    void border_node(halfedge_descriptor hd, Point& ept, Point& vpt) {
        Point_ref ep1 = get(vpm, CGAL::target(hd, mesh));
        Point_ref ep2 = get(vpm, CGAL::target(opposite(hd, mesh), mesh));
        ept = Point((ep1[0] + ep2[0]) * 0.5, 
            (ep1[1] + ep2[1]) * 0.5, 
            (ep1[2] + ep2[2]) * 0.5);
        CGAL::Halfedge_around_target_circulator<Point3Mesh> vcir(hd, mesh);
        Point_ref vp1 = get(vpm, CGAL::target(opposite(*vcir, mesh), mesh));
        Point_ref vp0 = get(vpm, CGAL::target(*vcir, mesh));
        --vcir;
        Point_ref vp_1 = get(vpm, CGAL::target(opposite(*vcir, mesh), mesh));
        vpt = Point((vp_1[0] + 6 * vp0[0] + vp1[0]) * .125,
            (vp_1[1] + 6 * vp0[1] + vp1[1]) * .125,
            (vp_1[2] + 6 * vp0[2] + vp1[2]) * .125);
    }
};


#endif
