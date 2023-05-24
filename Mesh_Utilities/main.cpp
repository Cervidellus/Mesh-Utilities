#define CGAL_EIGEN3_ENABLED
//Code is derived primarily from CGAL surface_mesh_skeletonization examples

#include <string>
#include <iostream>
#include <chrono>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
//#include <CGAL/mesh_segmentation.h>
//#include <CGAL/property_map.h>
//#include <CGAL/IO/Color.h>
//#include <nlohmann/json.hpp>
#include <CGAL/subdivision_method_3.h>

#include "mesh_IO.h"
#include "mesh_utilities.h"
//#include "tinycolormap.hpp"

#include "SurfaceMesh.h"

#include <CGAL/Aff_transformation_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Vector_3.h>

using namespace std;
namespace po = boost::program_options;

//next steps: 
//cylinder primitive
    //https://github.com/CGAL/cgal/issues/2618
    // https://doc.cgal.org/latest/Kernel_23/classCGAL_1_1Circle__3.html
//Skeleton to mesh by making balls and cylinders, and using a boolean operation to stick them together. 

//Passing a skeleton in python to a skeleton here
//passing back the stick and ball meshes back to python

int main(int argc, char* argv[])
{
    //std::vector<std::string> argument_log;
    //if (argc > 1) {
    //    argument_log.assign(argv + 1, argv + argc);
    //}
    ////options
    //po::options_description desc("Allowed options");
    //desc.add_options()
    //    ("help,h", "Help screen")
    //    ("source,s", po::value<std::string>()->default_value("E:/OneDrive/Dokumente/Mesh-Utilities/864691136023902009_decimated.ply"), "Source file. Currently supports .off format.")
    //    ("destination,d", po::value<std::string>()->default_value("output"), "Directory for output files. Currently outputs to ply format")
    //    ("max_angle", po::value<double>(), "During the local remeshing step, a triangle will be split if it has an angle larger than max_angle.")
    //    ("min_edge_length", po::value<double>(), "During the local remeshing step, an edge will be collapse if it is length is less than min_edge_length")
    //    ("quality_tradeoff", po::value<double>(), "Quality speed tradeoff. Controls the velocity of movement and approximation quality: decreasing this value makes the mean curvature flow based contraction converge faster, but results in a skeleton of lower quality.")
    //    ("medially_centered", po::value<bool>()->default_value(true), "If true, the meso - skeleton placement will be attracted by an approximation of the medial axis of the mesh during the contraction steps, so will be the result skeleton.")
    //    ("medial_tradeoff", po::value<double>(), "Controls the smoothness of the medial approximation: increasing this value results in a (less smooth) skeleton closer to the medial axis, as well as a lower convergence speed. It is only used if medially_centered==true.")
    //    ("sdf_mesh, sd", "Save mesh with vertices colored according to sdf values.")
    //    ("segmented_mesh, sg", "Save mesh with vertices colored according to segmention on sdf values.")
    //    ;

    //po::variables_map vm;
    //po::store(po::parse_command_line(argc, argv, desc), vm);
    //po::notify(vm);
    //
    //std::map<std::string, std::string> arguments_log;
    //
    //if (vm.count("help")) {
    //    cout << desc << '\n';
    //    return 1;
    //}

    //boost::filesystem::path source{ vm["source"].as<string>() };
    //boost::filesystem::path destination{ vm["destination"].as<string>() };

    //if (!boost::filesystem::exists(source.string())) {
    //    cout << "Source file does:" << source.string() << " does not exist!" << endl;
    //    return 1;
    //}

    //if (!boost::filesystem::exists(destination))
    //{
    //    cout << "Creating destination directory:" << destination.string() << endl;
    //    boost::filesystem::create_directory(vm["destination"].as<string>());
    //}
    ////set up timing
    //std::chrono::steady_clock::time_point start;
    //std::chrono::duration<double> skeletonization_elapsed(0.0);
    //std::chrono::duration<double> sdf_elapsed(0.0);
    //std::chrono::duration<double> segment_elapsed(0.0);
    //std::shared_ptr<Point3Mesh> myMesh = make_shared<Point3Mesh>();
    //bool success = meshutils::IO::read("C:\\Users\\acrob\\OneDrive\\Dokumente\\Mesh-Utilities\\x64\\Release\\monkey.ply", myMesh);
    //myMesh.fromPlyFile("C:\\Users\\acrob\\OneDrive\\Dokumente\\Mesh-Utilities\\x64\\Release\\monkey.ply");

    //if(success) std::cout << "Loaded mesh" << "\n";

    Point center(0, 0, 0);
    //Point3Mesh myMesh = meshutils::icosphere(5, 2, center);

    //Point3Mesh circlemesh = meshutils::primitives::circle(center, 10, 20);
    //meshutils::IO::write(std::make_shared<Point3Mesh>(circlemesh), "circlemesh.obj");

    auto sphere_5_2 = meshutils::primitives::icosphere(5, 2, center);
    auto sphere_10_2 = meshutils::primitives::icosphere(10, 2, center);
    auto sphere_5_4 = meshutils::primitives::icosphere(5, 4, center);
    meshutils::IO::write(sphere_5_2, "sphere_5_2.obj");
    meshutils::IO::write(sphere_10_2, "sphere_10_2.obj");
    meshutils::IO::write(sphere_5_4, "sphere_5_4.obj");

    ////Default cylinder
    //auto defaultCylinder = meshutils::primitives::cylinder(center, 3, 10, 20);
    //meshutils::IO::write(defaultCylinder, "cylindermesh_default.obj");

    ////Rotated
    //Point target(10, 10, 10);
    //Segment segment(center, target);
    //auto cylinder = meshutils::primitives::cylinder(segment, 3, 20);
    //meshutils::IO::write(cylinder, "cylindermesh_rotated.obj");


    ////Rotated, but not starting on origin
    //Point newcenter (-10, 5, 5);
    //Point newtarget(-15, 0, 25);
    //Segment newsegment(newcenter, newtarget);
    //auto newCylinder = meshutils::primitives::cylinder(newsegment, 3, 20);
    //meshutils::IO::write(newCylinder, "cylindermesh_notorigin.obj");
    


    //typedef CGAL::Aff_transformation_3<Kernel> Aff_transformation;
    //Vector direction_vector(0, 5, 1);
    //double angle = 45.0;
    //Vector z_vector(0, 0, 1);
    ////Aff_transformation rotation_matrix = Aff_transformation::
    ////direction_vector.direction().
    //auto myAngle = CGAL::angle(direction_vector, z_vector);
    //std::cout << myAngle;

    //std::shared_ptr<Point3Mesh> myMesh = std::make_shared<Point3Mesh>(meshutils::primitives::icosphere(5, 4, center));
    
    //for (vertex_descriptor vd : myMesh->vertices())
    //{
    //    std::cout << "vertex:" << myMesh->point(vd) << " squared distance from center: " << CGAL::squared_distance(center, myMesh->point(vd)) << "\n";//returns about half of what it should? 
    //}

    //Point3Mesh icos;
    //CGAL::make_icosahedron(icos, center, 5);

    //meshutils::IO::write(std::make_shared<Point3Mesh>(icos), "make_icosahedron5.obj");

    //CGAL::Subdivision_method_3::Loop_subdivision<Point3Mesh>(icos, CGAL::parameters::number_of_iterations(1));

    //meshutils::IO::write(std::make_shared<Point3Mesh>(icos), "Loop_subdivision_1_subs_5.obj");



    //for (vertex_descriptor vd : icos.vertices()) {
    //    //this gives correct squared distance of 25
    //    std::cout << "icos without subdivision:\n";
    //    std::cout << "vertex:" << icos.point(vd) << " squared distance from center: " << CGAL::squared_distance(center, icos.point(vd)) << "\n"; 
    //}

    //how do I get the number of vertices?
    //std::cout << myMesh->number_of_vertices();

    //meshutils::IO::write(myMesh, "icosahedronSphereMesh5.obj");
    

    //CGAL::Sphere_3<Point> sphere(Point(0, 0, 0), 5);
    

   // //Load mesh
   // Point3Mesh mesh;
   // //meshutils::IO::read_PLY(vm["source"].as<string>(), mesh);
   // std::ifstream inputStream(vm["source"].as<string>());
   // cout << "vertices: " << mesh.number_of_vertices() << endl;
   // cout << "faces: " << mesh.number_of_faces() << endl;

   // if (!CGAL::is_triangle_mesh(mesh))
   // {
   //     std::cout << "input geometry is not triangulated." << std::endl;
   //     return EXIT_FAILURE;
   // }

   // //generate skeleton
   //start = std::chrono::high_resolution_clock::now();
    //Mean_Curvature_Flow_Skeleton skeleton;
    //Skeletonization skeletonization(*myMesh);
    //if (vm.count("max_angle")) {
    //    skeletonization.set_max_triangle_angle(vm["max_angle"].as<double>());
    //}
    //if (vm.count("min_edge_length")) {
    //    skeletonization.set_min_edge_length(vm["min_edge_length"].as<double>());
    //}
    //if (vm.count("quality_tradeoff")) {
    //    skeletonization.set_quality_speed_tradeoff(vm["quality_tradeoff"].as<double>());
    //}
    //if (vm.count("medially_centered")) {
    //    skeletonization.set_is_medially_centered(vm["medially_centered"].as<bool>());
    //}
    //if (vm.count("medial_tradeoff")) {
    //    skeletonization.set_medially_centered_speed_tradeoff(vm["medial_tradeoff"].as<double>());
    //}

    //skeletonization.contract_until_convergence();
    //skeletonization.convert_to_skeleton(skeleton);
    //std::cout << "Number of vertices of the skeleton: " << boost::num_vertices(skeleton) << "\n";
    //std::cout << "Number of edges of the skeleton: " << boost::num_edges(skeleton) << "\n";
    //auto verts = boost::vertices(skeleton);
    //auto edges = boost::edges(skeleton);
    //std::cout << typeid(verts).name() << "\n";
    //std::cout << typeid(edges).name();

    //meshutils::IO::write(myMesh, "newMonkeyMesh.obj");


    //CGAL::make_range(vertices(skeleton))
   // skeletonization_elapsed = std::chrono::high_resolution_clock::now() - start;

   // //sdf and segmentation
   // std::vector<double> sdf_values(num_faces(mesh));
   // std::vector<double> segment_ids(num_faces(mesh));
   // if (vm.count("sdf_mesh") || vm.count("segmented_mesh"))
   // {
   //     Facet_with_id_pmap<double> sdf_property_map(sdf_values);
   //     CGAL::sdf_values(mesh, sdf_property_map);
   //     CGAL::sdf_values_postprocessing(mesh, sdf_property_map);
   //    
   //     if (vm.count("sdf_mesh"))
   //     {
   //         start = std::chrono::high_resolution_clock::now();
   //         //mesh
   //         meshutils::color_verts_from_face_property(mesh, sdf_property_map);
   //         boost::filesystem::path outputMeshFilePath(destination);
   //         outputMeshFilePath.append(source.stem().string() + "_sdf_mesh.ply");
   //         meshutils::IO::write_PLY(outputMeshFilePath.string(), mesh);

   //         //skeleton
   //         Point3Mesh skelmesh = meshutils::color_skel_to_mesh(skeleton, mesh);
   //         skelmesh.add_property_map<Point3Mesh::edge_index, boost::int64_t>("e:unused");//required to force writing of edges. Would also work if I color edges.

   //         boost::filesystem::path outputSkeletonFilePath(destination);
   //         outputSkeletonFilePath.append(source.stem().string() + "_sdf_skeleton.ply");
   //         meshutils::IO::write_PLY(outputSkeletonFilePath.string(), skelmesh);
   //         sdf_elapsed = std::chrono::high_resolution_clock::now() - start;
   //     }
   //     if (vm.count("segmented_mesh"))
   //     {
   //         start = std::chrono::high_resolution_clock::now();
   //         // segment the mesh using default parameters. There are many paramaters that can be changed here.
   //         Facet_with_id_pmap<double> segment_property_map(segment_ids);

   //         std::cout << "Number of segments: "
   //             << CGAL::segmentation_from_sdf_values(mesh, sdf_property_map, segment_property_map) << "\n";
   //         CGAL::sdf_values_postprocessing(mesh, segment_property_map);
   //         meshutils::color_verts_from_face_property(mesh, segment_property_map, tinycolormap::ColormapType::Jet);
   //         boost::filesystem::path outputMeshFilePath(destination);
   //         outputMeshFilePath.append(source.stem().string() + "_seg_mesh.ply");
   //         meshutils::IO::write_PLY(outputMeshFilePath.string(), mesh);

   //         Point3Mesh skelmesh = meshutils::color_skel_to_mesh(skeleton, mesh);
   //         boost::filesystem::path outputSkeletonFilePath(destination);
   //         outputSkeletonFilePath.append(source.stem().string() + "_seg_skeleton.ply");
   //         meshutils::IO::write_PLY(outputSkeletonFilePath.string(), skelmesh);
   //         segment_elapsed = std::chrono::high_resolution_clock::now() - start;
   //     }
   // }

   // //write values to json
   // nlohmann::json j;
 
   // j["arguments"] = argument_log;

   // j["skeletonization_elapsed_seconds"] = skeletonization_elapsed.count();
   // if (vm.count("sdf_mesh"))
   // {
   //     j["shape_diameter_function_elapsed_seconds"] = sdf_elapsed.count();
   //     j["shape_diameter_function_values"] = sdf_values;
   // }
   // if (vm.count("segmented_mesh"))
   // {
   //     j["shape_diameter_function_elapsed_seconds"] = segment_elapsed.count();
   //     j["segmentation_ids"] = segment_ids;
   // }

   // boost::filesystem::path outputResultsFilePath(destination);
   // outputResultsFilePath.append(source.stem().string() + "_results.json");
   // std::ofstream jsonStream(outputResultsFilePath.string());
   // jsonStream << std::setw(4) << j << std::endl;
   // jsonStream.close();


    return EXIT_SUCCESS;
}
