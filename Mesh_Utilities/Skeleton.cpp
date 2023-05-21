#include "Skeleton.h"

Skeleton::Skeleton(std::shared_ptr<Point3Mesh> mesh) :
    mesh_(mesh) {}

void Skeleton::generateSkeleton_() {
    Mean_Curvature_Flow_Skeleton skeleton;
    Skeletonization skeletonization(*mesh_);
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
    skeletonization.convert_to_skeleton(*skeleton_);
}