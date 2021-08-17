#include "CGAL_IO.h"

#include <CGAL/boost/graph/Named_function_parameters.h>
#include <CGAL/boost/graph/named_params_helper.h>


using namespace std;
bool CGAL_IO::read_PLY(const std::string& filepath, Mesh& mesh)
{
    ifstream inputStream(filepath);
    std::string ply_comments;
    string current_line;
    getline(inputStream, current_line);
    if (current_line != "ply")
    {
        cout << "Error reading file." << endl;
        return false;
    };
    getline(inputStream, current_line);
    if (current_line.find("binary") != std::string::npos) {
        //close inputstream, open again as binary, read file.
        inputStream.close();
        inputStream.open(filepath, ios::binary);
        if (CGAL::IO::read_PLY(inputStream, mesh, ply_comments)) return true;
    }
    else
    {
        //reset to beginning of file, and read.
        inputStream.clear();
        inputStream.seekg(0);
        if (CGAL::IO::read_PLY(inputStream, mesh, ply_comments)) return true;
    }
    cout << "Error reading file." << endl;
    return false;
}

bool CGAL_IO::write_PLY(const std::string& filepath, const Mesh& mesh, bool binary)
{
    if (binary) {
        ofstream outputStream(filepath, ios::binary);
        CGAL::IO::write_PLY(outputStream, mesh);
        //CGAL::IO::write_OBJ(outputStream, mesh);
    }
    else
    {
        ofstream outputStream(filepath);
        CGAL::IO::write_PLY(outputStream, mesh);
        //CGAL::IO::write_OBJ(outputStream, mesh);
    }
    return true;
}

Mesh CGAL_IO::skel_to_mesh(const Skeleton& skeleton)
{
    Mesh mesh;
    for (const Skeleton_vertex v : CGAL::make_range(vertices(skeleton)))
    {
        mesh.add_vertex(skeleton[v].point);
    }
    for (const Skeleton_edge e : CGAL::make_range(edges(skeleton)))
    {
        mesh.add_edge(Mesh::vertex_index(source(e, skeleton)), Mesh::vertex_index(target(e, skeleton)));
    }
    return mesh;
}

bool CGAL_IO::color_verts_from_face_property(Mesh& mesh, const Facet_with_id_pmap<double>& face_map, ColormapType colormap)
{
    Mesh::Property_map<Mesh::Vertex_index, CGAL::Color> vertex_colors = mesh.property_map<Mesh::Vertex_index, CGAL::Color >("v:color").first;
    Mesh::Property_map<Mesh::Face_index, CGAL::Color> face_colors = mesh.property_map<Mesh::Face_index, CGAL::Color>("f:color").first;
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
