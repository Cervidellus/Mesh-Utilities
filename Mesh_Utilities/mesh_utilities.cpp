#include "mesh_utilities.h"

//Converts a skeleton object to a mesh object. Do I need this? Can I just support IO of the skeleton types?
//Should I just have a separate skeletonIO namespace? Or should I have this in IO, overload the read/write with a skeleton version that calls this?
//The problem is, then I can't make it private... 
//Maybe I should properly subclass the CGAL types so I can have this as a private method. 

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