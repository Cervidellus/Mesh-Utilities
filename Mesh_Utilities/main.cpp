#define CGAL_EIGEN3_ENABLED
//#define CGAL_USE_BASIC_VIEWER
//#define QT_DISABLE_DEPRECATED_BEFORE = 4
//Temporary notes:
//I think what I need to read in the ply file is Polyhedron/polyhedron_prog_incr_builder.cpp
//This is some good example code: https://unclejimbo.github.io/2017/06/14/Mesh-Processing-with-CGAL/

//Code is coming primarily from CGAL surface_mesh_skeletonization examples

//#include <CGAL/mesh_segmentation.h>
//#include <tinyply.h>
#include <string>
#include <iostream>
#include <boost/program_options.hpp>
//#include <CGAL/boost/graph/IO/OFF.h>
//#include <CGAL/boost/graph/IO/PLY.h>
#include <CGAL/Surface_mesh/IO/PLY.h>
#include <CGAL/mesh_segmentation.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/property_map.h>

#include <nlohmann/json.hpp>
#include "CGAL_types.h"

//#include <CGAL/draw_surface_mesh.h>


//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>


//using namespace CGAL_IO;
using namespace std;
namespace po = boost::program_options;
int main(int argc, char* argv[])
{
    //options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Help screen")
        ("input,i", po::value<std::string>()->default_value("E:/OneDrive/Dokumente/Mesh-Utilities/monkey3.ply"), "Input file. Currently supports .off format.")
        ("output,o", po::value<std::string>()->default_value("E:/OneDrive/Dokumente/Mesh-Utilities/output.ply"), "Directory for output files. Currently outputs to ply format")
        ("max_angle", po::value<double>(), "During the local remeshing step, a triangle will be split if it has an angle larger than max_angle.")
        ("min_edge_length", po::value<double>(), "During the local remeshing step, an edge will be collapse if it is length is less than min_edge_length")
        ("quality_tradeoff", po::value<double>(), "Quality speed tradeoff. Controls the velocity of movement and approximation quality: decreasing this value makes the mean curvature flow based contraction converge faster, but results in a skeleton of lower quality.")
        ("medially_centered", po::value<bool>()->default_value(true), "If true, the meso - skeleton placement will be attracted by an approximation of the medial axis of the mesh during the contraction steps, so will be the result skeleton.")
        ("medial_tradeoff", po::value<double>(), "Controls the smoothness of the medial approximation: increasing this value results in a (less smooth) skeleton closer to the medial axis, as well as a lower convergence speed. It is only used if medially_centered==true.")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << '\n';
        return 1;
    }

    //Load mesh
    std::ifstream inputStream(vm["input"].as<string>());
    Mesh mesh;
    std::string ply_comments;
    bool file_read = CGAL::IO::read_PLY(inputStream, mesh, ply_comments);//Does not read a binary PLY file correctly? When reading a binary file, the flag std::ios::binary flag must be set during the creation of the ifstream
    cout << "vertices: " << mesh.number_of_vertices() << endl;
    cout << "faces: " << mesh.number_of_faces() << endl;
    //TEMP::just trying to figure out property maps.
    auto propertymaps = mesh.properties<vertex_descriptor>();

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

    // init the polyhedron simplex indices. Maybe you don't need for surface mesh?
    //CGAL::set_halfedgeds_items_id(mesh);
    
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
    //This is a shape distance function, and with a centerline function it should be the same as a shape diameter function. 
    //libigl has a proper sdf algorithm 
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

    // post-process the sdf values. Filters them and normalizes them to between 0 and 1. Maybe we don't want them normalized?
    CGAL::sdf_values_postprocessing(mesh, sdf_property_map);

    // create a property-map for segment-ids (it is an adaptor for this case)
    std::vector<std::size_t> segment_ids(mesh.number_of_faces());
    Facet_with_id_pmap<std::size_t> segment_property_map(segment_ids);

    // segment the mesh using default parameters
    std::cout << "Number of segments: "
        << CGAL::segmentation_from_sdf_values(mesh, sdf_property_map, segment_property_map) << "\n";

    //TODO:: color faces according to segment_property_map or sdf_property_map



    //outputs that need to be written to the mesh file:
    // polyhedron tmesh
    //sdf_property_map
    //segment_property_map which is the resulting segmentation. 
    //I probably also want to output a map od the correspondance between surface points and the skeleton vertices?

    //maybe pass as a facet property map vector or map of a name and a vector
    //but then I have to get into type erasure and I don't want to do that right now. 

    //Can also pass the property maps.. look at PLY.h to see how to pass.
    std::filebuf outfile_buffer;
    outfile_buffer.open(vm["output"].as<string>(), std::ios::out);
    std::ostream outstream(&outfile_buffer);
    CGAL::IO::write_PLY(outstream, mesh);
    
    //Write skeleton




    //Output files. Need to fix so that it removes the suffix of input, and then adds... 
    /*write_skeleton_ply(skeleton, vm["output"].as<string>() + "_skel.ply", false);

    write_polyhedron_ply(tmesh,
        vm["output"].as<string>() + "_mesh.ply",
        sdf_values,
        segment_ids,
        false);*/

    //write the clustering and sdf values to json

    nlohmann::json j;

    j["sdf"] = sdf_values;
    j["segments"] = segment_ids;

    std::ofstream o("E:/OneDrive/Dokumente/Mesh-Utilities//results.json");
    o << std::setw(4) << j << std::endl;
    o.close();

    ////// Output all the edges of the skeleton.
    ////std::ofstream output("skel-poly.cgal");
    ////Display_polylines display(skeleton, output);
    ////CGAL::split_graph_into_polylines(skeleton, display);
    ////output.close();


    ////// Output skeleton points and the corresponding surface points
    ////output.open("correspondance-poly.cgal");
    ////for (Skeleton_vertex v : CGAL::make_range(vertices(skeleton)))
    ////    for (vertex_descriptor vd : skeleton[v].vertices)
    ////        output << "2 " << skeleton[v].point << " "
    ////        << get(CGAL::vertex_point, tmesh, vd) << "\n";


    //return EXIT_SUCCESS;
}

//Up Next:

//Get binary file reading working
//Get a better use of paths to specify the output file path.  Look at my other code to figure out how. 
// Write skeleton to file.
// 
//Color the faces according to the SDF values. 

//Write the maps to the PLY file. I'll have to find some other way to read them. I might also separately write them to text. 

//Have it so that it can accept a list. 






//TODO:: try again with old code it see if it loads the file...

//Interesting:
//https://stackoverflow.com/questions/52160757/cgal-hole-filling-with-color
