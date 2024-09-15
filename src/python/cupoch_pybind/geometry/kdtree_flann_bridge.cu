#include "cupoch/geometry/geometry_utils.h"
#include "cupoch/knn/kdtree_flann.h"

using namespace cupoch;

extern "C" {

bool cupoch_kdtreeflann_set_geometry(knn::KDTreeFlann *tree,
                                     const geometry::Geometry *geometry) {
    return tree->SetRawData(geometry::ConvertVector3fVectorRef(*geometry));
}

int cupoch_kdtreeflann_search(const knn::KDTreeFlann *tree,
                              const Eigen::Vector3f *query,
                              const knn::KDTreeSearchParam *param,
                              std::vector<int> *indices,
                              std::vector<float> *distance2) {
    thrust::host_vector<int> host_indices;
    thrust::host_vector<float> host_distance2;
    int k = tree->Search(*query, *param, host_indices, host_distance2);
    indices->assign(host_indices.begin(), host_indices.end());
    distance2->assign(host_distance2.begin(), host_distance2.end());
    return k;
}

int cupoch_kdtreeflann_search_knn(const knn::KDTreeFlann *tree,
                                  const Eigen::Vector3f *query,
                                  int knn_value,
                                  std::vector<int> *indices,
                                  std::vector<float> *distance2) {
    thrust::host_vector<int> host_indices;
    thrust::host_vector<float> host_distance2;
    int k = tree->SearchKNN(*query, knn_value, host_indices, host_distance2);
    indices->assign(host_indices.begin(), host_indices.end());
    distance2->assign(host_distance2.begin(), host_distance2.end());
    return k;
}

int cupoch_kdtreeflann_search_radius(const knn::KDTreeFlann *tree,
                                     const Eigen::Vector3f *query,
                                     float radius,
                                     int max_nn,
                                     std::vector<int> *indices,
                                     std::vector<float> *distance2) {
    thrust::host_vector<int> host_indices;
    thrust::host_vector<float> host_distance2;
    int k = tree->SearchRadius(*query, radius, max_nn, host_indices,
                               host_distance2);
    indices->assign(host_indices.begin(), host_indices.end());
    distance2->assign(host_distance2.begin(), host_distance2.end());
    return k;
}

}  // extern "C"
