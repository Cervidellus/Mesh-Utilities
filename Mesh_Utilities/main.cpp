#define CGAL_EIGEN3_ENABLED
//Temporary notes:
//HDS is short for halfedge data structure
//I think what I need to read in the ply file is Polyhedron/polyhedron_prog_incr_builder.cpp
//This is some good example code: https://unclejimbo.github.io/2017/06/14/Mesh-Processing-with-CGAL/

//Code is coming primarily from CGAL surface_mesh_skeletonization examples

#include <CGAL/mesh_segmentation.h>
//#include <tinyply.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include "CGAL_types.h"
#include "CGAL_IO.h"

using namespace CGAL_IO;

int main(int argc, char* argv[])
{
    std::ifstream input((argc > 1) ? argv[1] : "E:/OneDrive/Dokumente/Analysis/AxonCarryingDendrites/exampleocclusion.off");
    Polyhedron tmesh;
    input >> tmesh;//>> is an operator defined for Polyhedron for .off input
    if (!CGAL::is_triangle_mesh(tmesh))
    {
        std::cout << "Input geometry is not triangulated." << std::endl;
        return EXIT_FAILURE;
    }
    Skeleton skeleton;

    //TODO: there are advanced options that could be messed with
    CGAL::extract_mean_curvature_flow_skeleton(tmesh, skeleton);
    std::cout << "Number of vertices of the skeleton: " << boost::num_vertices(skeleton) << "\n";
    std::cout << "Number of edges of the skeleton: " << boost::num_edges(skeleton) << "\n";

    // init the polyhedron simplex indices
    CGAL::set_halfedgeds_items_id(tmesh);

    //for each input vertex compute its distance to the skeleton
    std::vector<double> distances(num_vertices(tmesh));
    for (Skeleton_vertex v : CGAL::make_range(vertices(skeleton)))
    {
        const Point& skel_pt = skeleton[v].point;
        for (vertex_descriptor mesh_v : skeleton[v].vertices)
        {
            const Point& mesh_pt = mesh_v->point();
            distances[mesh_v->id()] = std::sqrt(CGAL::squared_distance(skel_pt, mesh_pt));
        }
    }

    // create a property-map for sdf values
    std::vector<double> sdf_values(num_faces(tmesh));
    Facet_with_id_pmap<double> sdf_property_map(sdf_values);

    // compute sdf values with skeleton. Takes the average distance for each of the 3 vertices in a face. 
    for (face_descriptor f : faces(tmesh))
    {
        double dist = 0;
        for (halfedge_descriptor hd : halfedges_around_face(halfedge(f, tmesh), tmesh))
            dist += distances[target(hd, tmesh)->id()];
        sdf_property_map[f] = dist / 3.;
    }

    // post-process the sdf values. Filters them and normalizes them to between 0 and 1. Maybe we don't want them normalized?
    CGAL::sdf_values_postprocessing(tmesh, sdf_property_map);

    // create a property-map for segment-ids (it is an adaptor for this case)
    std::vector<std::size_t> segment_ids(num_faces(tmesh));
    Facet_with_id_pmap<std::size_t> segment_property_map(segment_ids);

    // segment the mesh using default parameters
    std::cout << "Number of segments: "
        << CGAL::segmentation_from_sdf_values(tmesh, sdf_property_map, segment_property_map) << "\n";


    //outputs that need to be written to the mesh file:
    // polyhedron tmesh
    //sdf_property_map
    //segment_property_map which is the resulting segmentation. 
    //I probably also want to output a map od the correspondance between surface points and the skeleton vertices?

    //maybe pass as a facet property map vector or map of a name and a vector
    //but then I have to get into type erasure and I don't want to do that right now. 

    write_skeleton_ply(skeleton, "E:/OneDrive/Dokumente/Analysis/AxonCarryingDendrites/exampleskeleton.ply", false);

    write_polyhedron_ply(tmesh,
        "E:/OneDrive/Dokumente/Analysis/AxonCarryingDendrites/exampletmesh.ply",
        sdf_values,
        segment_ids,
        false);

    //write the clustering and sdf values to json

    nlohmann::json j;

    j["sdf"] = sdf_values;
    j["segments"] = segment_ids;

    std::ofstream o("E:/OneDrive/Dokumente/Analysis/AxonCarryingDendrites/results.json");
    o << std::setw(4) << j << std::endl;
    o.close();

    //// Output all the edges of the skeleton.
    //std::ofstream output("skel-poly.cgal");
    //Display_polylines display(skeleton, output);
    //CGAL::split_graph_into_polylines(skeleton, display);
    //output.close();


    //// Output skeleton points and the corresponding surface points
    //output.open("correspondance-poly.cgal");
    //for (Skeleton_vertex v : CGAL::make_range(vertices(skeleton)))
    //    for (vertex_descriptor vd : skeleton[v].vertices)
    //        output << "2 " << skeleton[v].point << " "
    //        << get(CGAL::vertex_point, tmesh, vd) << "\n";


    return EXIT_SUCCESS;
}

