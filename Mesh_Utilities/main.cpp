#define CGAL_EIGEN3_ENABLED
//Code is derived primarily from CGAL surface_mesh_skeletonization examples

#include <string>
#include <iostream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <CGAL/mesh_segmentation.h>
#include <CGAL/property_map.h>
#include <CGAL/IO/Color.h>
#include <nlohmann/json.hpp>

#include "CGAL_types.h"
#include "CGAL_IO.h"
#include "tinycolormap.hpp"


using namespace CGAL_IO;
using namespace std;
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
    //options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Help screen")
        ("source,s", po::value<std::string>()->default_value("E:/OneDrive/Dokumente/Mesh-Utilities/monkey3.ply"), "Source file. Currently supports .off format.")
        ("destination,d", po::value<std::string>()->default_value("output"), "Directory for output files. Currently outputs to ply format")
        ("max_angle", po::value<double>(), "During the local remeshing step, a triangle will be split if it has an angle larger than max_angle.")
        ("min_edge_length", po::value<double>(), "During the local remeshing step, an edge will be collapse if it is length is less than min_edge_length")
        ("quality_tradeoff", po::value<double>(), "Quality speed tradeoff. Controls the velocity of movement and approximation quality: decreasing this value makes the mean curvature flow based contraction converge faster, but results in a skeleton of lower quality.")
        ("medially_centered", po::value<bool>()->default_value(true), "If true, the meso - skeleton placement will be attracted by an approximation of the medial axis of the mesh during the contraction steps, so will be the result skeleton.")
        ("medial_tradeoff", po::value<double>(), "Controls the smoothness of the medial approximation: increasing this value results in a (less smooth) skeleton closer to the medial axis, as well as a lower convergence speed. It is only used if medially_centered==true.")
        ("face_color_sdf, fsf", "Color mesh faces according to sdf values.")
        ("face_color_segment, fst", "Color mesh faces according to segmentation values. Overrides vertex_color_sdf.")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << '\n';
        return 1;
    }

    boost::filesystem::path source{ vm["source"].as<string>() };
    boost::filesystem::path destination{ vm["destination"].as<string>() };

    if (!boost::filesystem::exists(source.string())) {
        cout << "Source file does:" << source.string() << " does not exist!" << endl;
        return 1;
    }

    if (!boost::filesystem::exists(destination))
    {
        cout << "Creating destination directory:" << destination.string() << endl;
        boost::filesystem::create_directory(vm["destination"].as<string>());
    }

    //Load mesh
    Mesh mesh;
    read_PLY(vm["source"].as<string>(), mesh);
    std::ifstream inputStream(vm["source"].as<string>());
    cout << "vertices: " << mesh.number_of_vertices() << endl;
    cout << "faces: " << mesh.number_of_faces() << endl;

    if (!CGAL::is_triangle_mesh(mesh))
    {
        std::cout << "input geometry is not triangulated." << std::endl;
        return EXIT_FAILURE;
    }

    //generate skeleton
    Skeleton skeleton;
    Skeletonization skeletonization(mesh);
    if (vm.count("max_angle")) {
        skeletonization.set_max_triangle_angle(vm["max_angle"].as<double>());
    }
    if (vm.count("min_edge_length")) {
        skeletonization.set_min_edge_length(vm["min_edge_length"].as<double>());
    }
    if (vm.count("quality_tradeoff")) {
        skeletonization.set_quality_speed_tradeoff(vm["quality_tradeoff"].as<double>());
    }
    if (vm.count("medially_centered")) {
        skeletonization.set_is_medially_centered(vm["medially_centered"].as<bool>());
    }
    if (vm.count("medial_tradeoff")) {
        skeletonization.set_medially_centered_speed_tradeoff(vm["medial_tradeoff"].as<double>());
    }

    skeletonization.contract_until_convergence();
    skeletonization.convert_to_skeleton(skeleton);
    std::cout << "Number of vertices of the skeleton: " << boost::num_vertices(skeleton) << "\n";
    std::cout << "Number of edges of the skeleton: " << boost::num_edges(skeleton) << "\n";
    
    //for each input vertex compute its distance to the skeleton point it collapsed to.
    std::vector<double> distances(mesh.number_of_vertices());
    for (Skeleton_vertex v : CGAL::make_range(vertices(skeleton)))
    {
        cout << "Skeleton index:" << v << endl;
        const Point& skel_pt = skeleton[v].point;
        for (vertex_descriptor mesh_v : skeleton[v].vertices)
        {
            cout << "Mesh index:" << mesh_v.idx() << endl;
            const Point& mesh_pt = mesh.point(mesh_v);
            distances[mesh_v.idx()] = std::sqrt(CGAL::squared_distance(skel_pt, mesh_pt));
            cout << distances[mesh_v.idx()] << endl;
        }
    }
    // create a property-map for sdf values
    std::vector<double> sdf_values(num_faces(mesh));
    Facet_with_id_pmap<double> sdf_property_map(sdf_values);
    
    // compute sdf values with skeleton. Takes the average distance for each of the 3 vertices in a face. 
    //This is a shape distance function, and with a centerline function it should be similar to a shape diameter function. 
    //libigl has a proper sdf algorithm. I believe CGAL does also: https://doc.cgal.org/latest/Surface_mesh_segmentation/group__PkgSurfaceMeshSegmentationRef.html#ga8a429857a748922d0e8460619db69764
    for (face_descriptor f : faces(mesh))
    {
        double dist = 0;

        for (halfedge_descriptor hd : halfedges_around_face(halfedge(f, mesh), mesh))//I think I need a different circulator for surface mesh?
            dist += distances[target(hd, mesh).idx()];
        sdf_property_map[f] = dist / 3.;

        //May Need to change to one of hte two ways... 
        //{
        //    std::cout << "vertices around face " << f << std::endl;
        //    CGAL::Vertex_around_face_iterator<Mesh> vbegin, vend;
        //    for (boost::tie(vbegin, vend) = vertices_around_face(m.halfedge(f), m);
        //        vbegin != vend;
        //        ++vbegin) {
        //        std::cout << *vbegin << std::endl;
        //    }
        //}
        //// or the same again, but directly with a range based loop
        //for (vertex_descriptor vd : vertices_around_face(m.halfedge(f), m)) {
        //    std::cout << vd << std::endl;
        //}
    }

    // post-process the sdf values. Filters them and normalizes them to between 0 and 1. 
    CGAL::sdf_values_postprocessing(mesh, sdf_property_map);

    // create a property-map for segment-ids (it is an adaptor for this case)
    std::vector<std::size_t> segment_ids(mesh.number_of_faces());
    Facet_with_id_pmap<std::size_t> segment_property_map(segment_ids);

    // segment the mesh using default parameters. There are many paramaters that can be changed here.
    std::cout << "Number of segments: "
        << CGAL::segmentation_from_sdf_values(mesh, sdf_property_map, segment_property_map) << "\n";




    

    //Color mesh faces according to segment_property_map or sdf_property_map 
    if (vm.count("face_color_sdf") && !vm.count("face_color_segment"))
    {
        Mesh::Property_map<Mesh::Face_index, CGAL::Color> face_colors = mesh.property_map<Mesh::Face_index, CGAL::Color >("f:color").first;
        for (face_descriptor f : faces(mesh))
        {
            tinycolormap::Color c = tinycolormap::GetColor(sdf_property_map[f]);
            face_colors[f].set_rgb(c.r()*255, c.g()*255, c.b()*255);
        }
    }


    //Mesh::Property_map<Mesh::Vertex_index, CGAL::Color> vcolors = mesh.property_map<Mesh::Vertex_index, CGAL::Color >("v:color").first;










    //Write mesh to file
    boost::filesystem::path outputMeshFilePath(destination);
    outputMeshFilePath.append(source.stem().string() + "_mesh.ply");
    CGAL_IO::write_PLY(outputMeshFilePath.string(), mesh);

    //Write skeleton to file
    boost::filesystem::path outputSkeletonFilePath(destination);
    Mesh skelmesh = CGAL_IO::skel_to_mesh(skeleton);
    outputSkeletonFilePath.append(source.stem().string() + "_skeleton.ply");
    
    skelmesh.add_property_map<Mesh::edge_index, boost::int64_t>("e:weight");//required to force writing of edges. Would also work once I color.
    //To read, change v0 in header to vertex1, and v1 to vertex2
    CGAL_IO::write_PLY(outputSkeletonFilePath.string(), skelmesh, false);

    //write the clustering and sdf values to json
    nlohmann::json j;
    j["sdf"] = sdf_values;
    j["segments"] = segment_ids;
    boost::filesystem::path outputResultsFilePath(destination);
    outputResultsFilePath.append(source.stem().string() + "_results.json");
    std::ofstream jsonStream(outputResultsFilePath.string());
    jsonStream << std::setw(4) << j << std::endl;
    jsonStream.close();

    return EXIT_SUCCESS;
}

//Up Next:

// Write skeleton to file.
// 
//Color the skeleton vertices according to face SDF values, and edges according to source vertex.
//Color the faces according to the SDF values. 


//Have it so that it can accept a list. 


//TODO:: try again with old code it see if it loads the file...

//Interesting:
//https://stackoverflow.com/questions/52160757/cgal-hole-filling-with-color
