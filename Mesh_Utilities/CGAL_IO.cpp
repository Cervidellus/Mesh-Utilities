#include "CGAL_IO.h"

//#include <CGAL/Surface_mesh/IO/PLY.h>
//#include <CGAL/boost/graph/IO/PLY.h>
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

bool CGAL_IO::write_PLY(const std::string& filepath, Mesh& mesh, bool binary)
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

//bool CGAL_IO::write_PLY(const std::string& filepath, Skeleton& skeleton, bool binary)
//{
//    std::string unused_comment = "NO COMMENT!";
//    if (binary) {
//        
//        /*os, sm, unused_comment, parameters::all_default()*/
//        ofstream outputStream(filepath, ios::binary);
//        //CGAL::IO::write_PLY(outputStream, skeleton, unused_comment, CGAL::parameters::all_default());
//    }
//    else
//    {
//        ofstream outputStream(filepath);
//        //CGAL::IO::write_PLY(outputStream, skeleton);
//    }
//    return true;
//}

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

//bool CGAL_IO::write_skeleton_PLY(
//        const std::string& filepath,
//        const Skeleton& skeleton,
//        const bool binary)
//{
//    std::filebuf file_buffer;
//
//    if (binary) {
//        file_buffer.open(filepath, std::ios::out | std::ios::binary);
//    }
//    else
//    {
//        file_buffer.open(filepath, std::ios::out);
//    }
//    std::ostream outstream(&file_buffer);
//    if (outstream.fail()) throw std::runtime_error("failed to open " + filepath);
//
//    CGAL::IO::write_PLY(outstream, skeleton);
//
//
//
////
////    //tinyply::PlyFile mesh_file;
////    int vertex_number = boost::num_vertices(skeleton);
////    int edge_number = boost::num_edges(skeleton);
////
////    //skeleton vertices
////    std::vector<double> vertex_array(vertex_number * 3);
////    for (Skeleton_vertex v : CGAL::make_range(vertices(skeleton)))
////    {
////        vertex_array[v * 3] = skeleton[v].point.x();
////        vertex_array[v * 3 + 1] = skeleton[v].point.y();
////        vertex_array[v * 3 + 2] = skeleton[v].point.z();
////    }
////    mesh_file.add_properties_to_element(
////        "vertex",
////        { "x", "y", "z" },
////        tinyply::Type::FLOAT64,
////        vertex_number,
////        reinterpret_cast<uint8_t*>(vertex_array.data()),
////        tinyply::Type::INVALID,
////        0);
////
////
////    //// Output all the edges of the skeleton.
//////https://stackoverflow.com/questions/47365731/how-to-draw-lines-in-ply-files-using-meshlab shows .ply format for edges, which is based on vertex indices
////    std::vector<unsigned int> edge_array;
////
////    for (Skeleton_edge e : CGAL::make_range(edges(skeleton)))
////    {
////        edge_array.push_back(source(e, skeleton));
////        edge_array.push_back(target(e, skeleton));
////    }
////
////    mesh_file.add_properties_to_element(
////        "edge",
////        { "vertex1", "vertex2" },
////        tinyply::Type::INT32,
////        edge_number,
////        reinterpret_cast<uint8_t*>(edge_array.data()),
////        tinyply::Type::INVALID,
////        0);
////
////
////
////    //cube_file.add_properties_to_element("vertex", { "nx", "ny", "nz" },
////    //    Type::FLOAT32, cube.normals.size(), reinterpret_cast<uint8_t*>(cube.normals.data()), Type::INVALID, 0);
////
////    //cube_file.add_properties_to_element("vertex", { "u", "v" },
////    //    Type::FLOAT32, cube.texcoords.size(), reinterpret_cast<uint8_t*>(cube.texcoords.data()), Type::INVALID, 0);
////
////    //cube_file.add_properties_to_element("face", { "vertex_indices" },
////    //    Type::UINT32, cube.triangles.size(), reinterpret_cast<uint8_t*>(cube.triangles.data()), Type::UINT8, 3);
////
////    mesh_file.get_comments().push_back("generated by tinyply 2.3");
////
////    if (binary)
////    {
////        mesh_file.write(outstream, true);
////    }
////    else
////    {
////        mesh_file.write(outstream, false);
////    }
//    return true;
//}



//}
// 
// 
// 
//namespace CGAL_IO
//{
//    void write_polyhedron_ply(
//        const Polyhedron& polyhedron,
//        const std::string& filepath,
//        std::vector<double> sdf_values,
//        std::vector<std::size_t> segment_ids,
//        const bool binary)
//    {
//        std::filebuf file_buffer;
//        int i;
//
//        if (binary) {
//            file_buffer.open(filepath, std::ios::out | std::ios::binary);
//        }
//        else
//        {
//            file_buffer.open(filepath, std::ios::out);
//        }
//        std::ostream outstream(&file_buffer);
//        if (outstream.fail()) throw std::runtime_error("failed to open " + filepath);
//
//        tinyply::PlyFile mesh_file;
//        int vertex_number = boost::num_vertices(polyhedron);
//        //int edge_number = boost::num_edges(polyhedron);
//
//        //skeleton vertices
//        std::vector<double> vertex_array(vertex_number * 3);
//        i = 0;
//        for (auto it = polyhedron.vertices_begin(); it != polyhedron.vertices_end(); it++, i++)
//        {
//            auto v = it->point();
//            vertex_array[i * 3] = v.x();
//            vertex_array[i * 3 + 1] = v.y();
//            vertex_array[i * 3 + 2] = v.z();
//        }
//
//        mesh_file.add_properties_to_element(
//            "vertex",
//            { "x", "y", "z" },
//            tinyply::Type::FLOAT64,
//            vertex_number,
//            reinterpret_cast<uint8_t*>(vertex_array.data()),
//            tinyply::Type::INVALID,
//            0);
//
//        //faces
//        std::vector<unsigned int> facet_array(polyhedron.size_of_facets() * 3);
//        for (face_descriptor f : faces(polyhedron))
//        {
//            int face_index = f->id();
//            if (f->is_triangle())
//            {
//                i = 0;
//                for (halfedge_descriptor halfedge : halfedges_around_face(halfedge(f, polyhedron), polyhedron))
//                {
//
//                    facet_array[face_index * 3 + i] = halfedge->vertex()->id();
//                    i++;
//                }
//
//            }
//        }
//
//        mesh_file.add_properties_to_element(
//            "face",
//            { "vertex_indices" },
//            tinyply::Type::UINT32,
//            polyhedron.size_of_facets(),
//            reinterpret_cast<uint8_t*>(facet_array.data()),
//            tinyply::Type::UINT8, 3);
//
//        mesh_file.add_properties_to_element(
//            "face",
//            { "signed_distance_function" },
//            tinyply::Type::FLOAT64,
//            sdf_values.size(),
//            reinterpret_cast<uint8_t*>(sdf_values.data()),
//            tinyply::Type::INVALID,
//            0);
//
//        //mesh_file.add_properties_to_element(
//        //    "face",
//        //    { "segement_id" },
//        //    tinyply::Type::INT32,
//        //    segment_ids.size(),
//        //    reinterpret_cast<uint8_t*>(segment_ids.data()),
//        //    tinyply::Type::INVALID,
//        //    0);
//
//
//        //)
//        ////segemntation values
//        //
//
//
//        //cube_file.add_properties_to_element("vertex", { "nx", "ny", "nz" },
//        //    Type::FLOAT32, cube.normals.size(), reinterpret_cast<uint8_t*>(cube.normals.data()), Type::INVALID, 0);
//
//        //cube_file.add_properties_to_element("vertex", { "u", "v" },
//        //    Type::FLOAT32, cube.texcoords.size(), reinterpret_cast<uint8_t*>(cube.texcoords.data()), Type::INVALID, 0);
//
//        //cube_file.add_properties_to_element("face", { "vertex_indices" },
//        //    Type::UINT32, cube.triangles.size(), reinterpret_cast<uint8_t*>(cube.triangles.data()), Type::UINT8, 3);
//
//        mesh_file.get_comments().push_back("generated by tinyply 2.3");
//
//        if (binary)
//        {
//            mesh_file.write(outstream, true);
//        }
//        else
//        {
//            mesh_file.write(outstream, false);
//        }
//
//    }


//
//
//
////python trimesh to polyhedron
//
////ply reader
////ply writer
//
////obj reader
////obj writer