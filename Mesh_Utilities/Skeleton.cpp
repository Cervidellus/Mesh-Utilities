#include "Skeleton.h"

Skeleton::Skeleton(std::shared_ptr<Point3Mesh> mesh) :
    sourceMesh(mesh),
    skeletonData(std::make_shared<Mean_Curvature_Flow_Skeleton>(Mean_Curvature_Flow_Skeleton()))
{
}

//std::shared_ptr<Mean_Curvature_Flow_Skeleton> skeletonData
Skeleton::Skeleton(): 
    skeletonData(std::make_shared<Mean_Curvature_Flow_Skeleton>(Mean_Curvature_Flow_Skeleton()))
{
}

int Skeleton::edgeCount()
{
    return int(boost::num_edges(*skeletonData));
}

int Skeleton::vertexCount()
{
    return boost::num_vertices(*skeletonData);
}

std::vector<std::array<double, 3>> Skeleton::vertices()
{
    std::vector<std::array<double, 3>> vertices;
    
    for (int i = 0; i < boost::num_vertices(*skeletonData); i++) {
        Point point = skeletonData->operator[](i).point;
        vertices.push_back(std::array<double, 3>{point.x(), point.y(), point.z()});
    }

    return vertices;
}

std::vector<std::pair<int, int>> Skeleton::edges()
{
    std::vector<std::pair<int, int>> edges;

    for (const Skeleton_edge edge : CGAL::make_range(boost::edges(*skeletonData))) {
        int source = boost::source(edge, *skeletonData);
        int target = boost::target(edge, *skeletonData);
        edges.push_back(std::pair<int, int>(source, target));
    }

    return edges;
}

void Skeleton::generateSkeleton_() {
    Mean_Curvature_Flow_Skeleton skeleton;
    Skeletonization skeletonization(*sourceMesh);
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
    
    skeletonization.contract_until_convergence();
    skeletonization.convert_to_skeleton(*skeletonData);
}
