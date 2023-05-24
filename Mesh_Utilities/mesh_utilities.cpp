#include "mesh_utilities.h"
#include "subdivision_masks.h"

#include <numbers>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Aff_transformation_3.h>
#include <CGAL/aff_transformation_tags.h>
#include <CGAL/boost/graph/generators.h>
#include <CGAL/Polygon_mesh_processing/extrude.h>
#include <CGAL/Polygon_mesh_processing/transform.h>
#include <CGAL/subdivision_method_3.h>
#include <CGAL/Subdivision_method_3/subdivision_masks_3.h>
#include <Eigen/Geometry>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Vector_3.h>

std::shared_ptr<Point3Mesh> meshutils::primitives::icosphere(const double radius, const int subdivisions, const Point center)
{
    Point3Mesh sphere_mesh;
    CGAL::make_icosahedron(sphere_mesh, center, radius);
    Spherical_Loop_mask mask(sphere_mesh, center, radius);
    CGAL::Subdivision_method_3::PTQ(sphere_mesh, mask, subdivisions);

    return std::make_unique<Point3Mesh>(sphere_mesh);
}

std::shared_ptr<Point3Mesh> meshutils::primitives::circle(const Point center, const double radius, const int num_vertices)//should I add an axis?
{
    //Produces a triangular circle mesh  on the XY plane centered around a Point 'center', with edge vertices 'num_vertices'

    Point3Mesh circle;
    double angleStep = 2.0f * std::numbers::pi / num_vertices;
    circle.add_vertex(center);

    //add edge verts
    for (int i = 0; i < num_vertices; i++) {
        double angle = i * angleStep;
        double x = radius * cos(angle) + center[0];
        double y = radius * sin(angle) + center[1];
        circle.add_vertex(Point(x, y, center[2]));
    }

    circle.point(CGAL::SM_Vertex_index(0));
    vertex_descriptor center_vd(0);
    for (vertex_descriptor vd : circle.vertices()) {
        if (vd.idx() != 0) {
            //First point makes a face with the last point. 
            if (vd.idx() == 1) {
                circle.add_face(center_vd, vertex_descriptor(num_vertices), vd);
            }
            else {
                circle.add_face(center_vd, vertex_descriptor(vd-1), vd);
            }
        }
    }
    return std::make_unique<Point3Mesh>(circle);
}

std::shared_ptr<Point3Mesh> meshutils::primitives::cylinder(const Point source, const double radius, const double height, const int num_vertices)
{
    Point3Mesh cylinder;
    Point target(source[0], source[1], source[2] + height);

    //create a circle and extrude along z axis
    auto circle = meshutils::primitives::circle(source, radius, num_vertices);
    Vector vector(target, source);
    CGAL::Polygon_mesh_processing::extrude_mesh(*circle, cylinder, vector);

    return std::make_unique<Point3Mesh>(cylinder);
}

std::shared_ptr<Point3Mesh> meshutils::primitives::cylinder(const Segment segment, const double radius, const int num_vertices)
{
    //TODO::Should I move these typedefs to mesh_types?
    typedef CGAL::Aff_transformation_3<Kernel> Affine;
    typedef Kernel::Direction_3 Direction;

    //Construct a cylinder that starts at segemnt.source() and extends to to length of the segment in z. 
    Point source = segment.source();//TODO this should be a unique pointer
    auto cylinder = meshutils::primitives::cylinder(
        source,
        radius,
        sqrt(segment.squared_length()),
        num_vertices);

    Eigen::Vector3d initial(source[0],source[1],source[2] + sqrt(segment.squared_length()));
    Eigen::Vector3d final(segment.to_vector()[0], segment.to_vector()[1], segment.to_vector()[2]);
    Eigen::Quaternion<double> quaternion = Eigen::Quaternion<double>::FromTwoVectors(initial, final);
    auto rotationMatrix = quaternion.toRotationMatrix();

    Affine rotation({
        rotationMatrix(0,0),
        rotationMatrix(0,1),
        rotationMatrix(0,2),
        rotationMatrix(1,0),
        rotationMatrix(1,1),
        rotationMatrix(1,2),
        rotationMatrix(2,0),
        rotationMatrix(2,1),
        rotationMatrix(2,2)
        });
    
    CGAL::Polygon_mesh_processing::transform(rotation, *cylinder);
    
    return cylinder;
}

void meshutils::subdivision::loop_subdivision(Point3Mesh& mesh, int subdivisions) {
    CGAL::Subdivision_method_3::Loop_subdivision(mesh, CGAL::parameters::number_of_iterations(subdivisions));
}

Point3Mesh meshutils::skel_to_mesh(const Mean_Curvature_Flow_Skeleton& skeleton)
{
    Point3Mesh mesh;
    for (const Skeleton_vertex v : CGAL::make_range(vertices(skeleton)))
    {
        mesh.add_vertex(skeleton[v].point);
    }
    for (const Skeleton_edge e : CGAL::make_range(edges(skeleton)))
    {
        mesh.add_edge(Point3Mesh::vertex_index(source(e, skeleton)), Point3Mesh::vertex_index(target(e, skeleton)));
    }
    return mesh;
}

//Here I am passing in a mesh and returning a mesh. I think I should pass in a mesh and return an error. 
Point3Mesh meshutils::color_skel_to_mesh(Mean_Curvature_Flow_Skeleton& skeleton, Point3Mesh& mesh)
{
    Point3Mesh::Property_map<Point3Mesh::Vertex_index, CGAL::Color> mesh_vertex_colors = mesh.property_map<Point3Mesh::Vertex_index, CGAL::Color >("v:color").first;
    Point3Mesh skelmesh;
    Point3Mesh::Property_map<Point3Mesh::Edge_index, boost::int64_t> skel_edge_red = (skelmesh.add_property_map<Point3Mesh::edge_index, boost::int64_t>("e:red")).first;
    Point3Mesh::Property_map<Point3Mesh::Edge_index, boost::int64_t> skel_edge_green = (skelmesh.add_property_map<Point3Mesh::edge_index, boost::int64_t>("e:green")).first;
    Point3Mesh::Property_map<Point3Mesh::Edge_index, boost::int64_t> skel_edge_blue = (skelmesh.add_property_map<Point3Mesh::edge_index, boost::int64_t>("e:blue")).first;
    Point3Mesh::Property_map<Point3Mesh::Vertex_index, CGAL::Color> skelmesh_vertex_colors = (skelmesh.add_property_map<Point3Mesh::Vertex_index, CGAL::Color>("v:color")).first;
    for (const Skeleton_vertex sv : CGAL::make_range(vertices(skeleton)))
    {
        int vertex_index = skelmesh.add_vertex(skeleton[sv].point);
        auto skelmesh_vcolor_iterator = skelmesh_vertex_colors.end();
        skelmesh_vcolor_iterator--;

        int count = skeleton[sv].vertices.size();
        int r = 0;
        int g = 0;
        int b = 0;
        if (count > 0)
        {
            for (vertex_descriptor mv : skeleton[sv].vertices)
            {
                r += mesh_vertex_colors[mv].r();
                g += mesh_vertex_colors[mv].g();
                b += mesh_vertex_colors[mv].b();
            }
            skelmesh_vcolor_iterator->set_rgb(r / count, g / count, b / count);
        }
    }
    for (const Skeleton_edge e : CGAL::make_range(edges(skeleton)))
    {
        skelmesh.add_edge(Point3Mesh::vertex_index(source(e, skeleton)), Point3Mesh::vertex_index(target(e, skeleton)));
        int sourceindex = Point3Mesh::vertex_index(source(e, skeleton));
        CGAL::Color c = skelmesh_vertex_colors[Point3Mesh::vertex_index(source(e, skeleton))];
        auto red_iterator = skel_edge_red.end();
        auto green_iterator = skel_edge_green.end();
        auto blue_iterator = skel_edge_blue.end();
        red_iterator--;
        green_iterator--;
        blue_iterator--;
        *red_iterator = c.r();
        *green_iterator = c.g();
        *blue_iterator = c.b();
    }
    return skelmesh;
}

bool meshutils::color_verts_from_face_property(Point3Mesh& mesh, const Facet_with_id_pmap<double>& face_map, ColormapType colormap)
{
    Point3Mesh::Property_map<Point3Mesh::Vertex_index, CGAL::Color> vertex_colors = mesh.property_map<Point3Mesh::Vertex_index, CGAL::Color >("v:color").first;
    Point3Mesh::Property_map<Point3Mesh::Face_index, CGAL::Color> face_colors = mesh.property_map<Point3Mesh::Face_index, CGAL::Color>("f:color").first;
    std::vector<std::vector<CGAL::Color>> temp_vertex_colors(mesh.number_of_vertices());
    for (face_descriptor f : faces(mesh))
    {
        tinycolormap::Color c = tinycolormap::GetColor(face_map[f], colormap);
        CGAL::Color face_color(c.r() * 255, c.g() * 255, c.b() * 255);
        face_colors[f] = face_color;
        for (halfedge_descriptor hd : halfedges_around_face(halfedge(f, mesh), mesh))
        {
            temp_vertex_colors[target(hd, mesh).idx()].push_back(face_color);
        }
    }
    for (vertex_descriptor v : vertices(mesh))
    {
        //average the colors from temp_vertex_colors
        int count = temp_vertex_colors[v.idx()].size();
        double r = 0;
        double g = 0;
        double b = 0;

        //iterate over temp_vertex_colors[v.idx()] and add values to r g and b. 
        for (CGAL::Color color : temp_vertex_colors[v.idx()])
        {
            r += color.red();
            g += color.green();
            b += color.blue();
        }
        vertex_colors[v].set_rgb(r / count, g / count, b / count);
    }
    return true;
}