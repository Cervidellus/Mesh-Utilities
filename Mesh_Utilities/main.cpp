#define CGAL_EIGEN3_ENABLED
//Code is derived primarily from CGAL surface_mesh_skeletonization examples

#include <string>
#include <iostream>
#include <chrono>

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
        ("source,s", po::value<std::string>()->default_value("E:/OneDrive/Dokumente/Mesh-Utilities/864691136023902009_decimated.ply"), "Source file. Currently supports .off format.")
        ("destination,d", po::value<std::string>()->default_value("output"), "Directory for output files. Currently outputs to ply format")
        ("max_angle", po::value<double>(), "During the local remeshing step, a triangle will be split if it has an angle larger than max_angle.")
        ("min_edge_length", po::value<double>(), "During the local remeshing step, an edge will be collapse if it is length is less than min_edge_length")
        ("quality_tradeoff", po::value<double>(), "Quality speed tradeoff. Controls the velocity of movement and approximation quality: decreasing this value makes the mean curvature flow based contraction converge faster, but results in a skeleton of lower quality.")
        ("medially_centered", po::value<bool>()->default_value(true), "If true, the meso - skeleton placement will be attracted by an approximation of the medial axis of the mesh during the contraction steps, so will be the result skeleton.")
        ("medial_tradeoff", po::value<double>(), "Controls the smoothness of the medial approximation: increasing this value results in a (less smooth) skeleton closer to the medial axis, as well as a lower convergence speed. It is only used if medially_centered==true.")
        ("sdf_mesh, sd", "Save mesh with vertices colored according to sdf values.")
        ("segmented_mesh, sg", "Save mesh with vertices colored according to segmention on sdf values.")
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

    //Calculate shape diameter function values and segmentation, save meshes
    std::vector<double> sdf_values(num_faces(mesh));
    if (vm.count("sdf_mesh") || vm.count("segmented_mesh"))
    {
        Facet_with_id_pmap<double> sdf_property_map(sdf_values);
        CGAL::sdf_values(mesh, sdf_property_map);
        CGAL::sdf_values_postprocessing(mesh, sdf_property_map);

        if (vm.count("sdf_mesh"))
        {
            CGAL_IO::color_verts_from_face_property(mesh, sdf_property_map);
            boost::filesystem::path outputMeshFilePath(destination);
            outputMeshFilePath.append(source.stem().string() + "_sdf_mesh.ply");
            CGAL_IO::write_PLY(outputMeshFilePath.string(), mesh);
        }
        if (vm.count("segmented_mesh"))
        {
            // segment the mesh using default parameters. There are many paramaters that can be changed here.
            std::vector<double> segment_ids(mesh.number_of_faces());
            Facet_with_id_pmap<double> segment_property_map(segment_ids);

            std::cout << "Number of segments: "
                << CGAL::segmentation_from_sdf_values(mesh, sdf_property_map, segment_property_map) << "\n";
            CGAL::sdf_values_postprocessing(mesh, segment_property_map);
            CGAL_IO::color_verts_from_face_property(mesh, segment_property_map, tinycolormap::ColormapType::Jet);
            boost::filesystem::path outputMeshFilePath(destination);
            outputMeshFilePath.append(source.stem().string() + "_seg_mesh.ply");
            CGAL_IO::write_PLY(outputMeshFilePath.string(), mesh);
        }
    }


    //generate skeleton
    auto start = std::chrono::high_resolution_clock::now();
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
    
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> skeletonization_elapsed = finish - start;

    //convert skeleton to a mesh and color vertices
    Mesh skelmesh = CGAL_IO::skel_to_mesh(skeleton);
    skelmesh.add_property_map<Mesh::edge_index, boost::int64_t>("e:unused");//required to force writing of edges. Would also work if I color edges.
    Mesh::Property_map<Mesh::Vertex_index, CGAL::Color> skel_vertex_colors = (skelmesh.add_property_map<Mesh::Vertex_index, CGAL::Color>("v:color")).first;


    //Mesh::Property_map<Mesh::Vertex_index, CGAL::Color> skel_vertex_colors = skelmesh.property_map<Mesh::Vertex_index, CGAL::Color >("v:color").first;//size is null.. should be 833?
    //for (vertex_descriptor v : vertices(skelmesh))
    //{
    //    //TODO::Color vertex according to mean sdf value of associated faces. 
    //    tinycolormap::Color c = tinycolormap::GetColor(mean_mesh_vertex_distance[v] / max_skeleton_vertex_distance);
    //    skel_vertex_colors[v].set_rgb(c.r() * 255, c.g() * 255, c.b() * 255);
    //}



    //Write skeleton to file
    boost::filesystem::path outputSkeletonFilePath(destination);
    outputSkeletonFilePath.append(source.stem().string() + "_skeleton.ply");
    //To read, change v0 in header to vertex1, and v1 to vertex2. Will be fixed in later versions of cgal.
    CGAL_IO::write_PLY(outputSkeletonFilePath.string(), skelmesh, false);

    //write the clustering and sdf values to json
    nlohmann::json j;
    j["skeletonization_time"] = skeletonization_elapsed.count();
    j["shape_diameter_function"] = sdf_values;
    //j["sdf"] = skeleton_face_distances;
    //j["segments"] = segment_ids;
    boost::filesystem::path outputResultsFilePath(destination);
    outputResultsFilePath.append(source.stem().string() + "_results.json");
    std::ofstream jsonStream(outputResultsFilePath.string());
    jsonStream << std::setw(4) << j << std::endl;
    jsonStream.close();

    return EXIT_SUCCESS;
}

//Up Next:

//Maybe save a second mesh that has the segmentation? 
// 
//Have it so that it can accept a list of meshes to process.


//optimize parameters for preserving spines. skeleton quality
// 
//Color the faces according to the segmentation. 

//add "fur" to skeleton by making a mesh with lines pointing between skeleton vertex and mesh vertex.





//TODO:: try again with old code it see if it loads the file...

//Interesting:
//https://stackoverflow.com/questions/52160757/cgal-hole-filling-with-color
