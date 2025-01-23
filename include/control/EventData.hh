#ifndef EVENT_DATA_HH
#define EVENT_DATA_HH

#include "G4ThreeVector.hh"
#include "globals.hh"

// Template for 3D vector
template <typename T>
using vector3d = std::vector<std::vector<std::vector<T>>>;

// Template for 4D vector
template <typename T>
using vector4d = std::vector<std::vector<std::vector<std::vector<T>>>>;

// Function to initialize a 3D vector
template <typename T>
vector3d<T> init_vector3d(size_t dim1, size_t dim2, size_t dim3, T init_value = nullptr) {
  return vector3d<T>(dim1, std::vector<std::vector<T>>(dim2, std::vector<T>(dim3, init_value)));
}

// Function to initialize a 4D vector
template <typename T>
vector4d<T> init_vector4d(size_t dim1, size_t dim2, size_t dim3, size_t dim4, T init_value = nullptr) {
  return vector4d<T>(dim1, std::vector<std::vector<std::vector<T>>>(dim2, std::vector<std::vector<T>>(dim3, std::vector<T>(dim4, init_value))));
}

namespace nevod {

struct TrackData {
  G4int detected_copy_num = -1;
  G4ThreeVector coordinate{};
  G4ThreeVector momentum{};
  G4double energy{};

  TrackData() = default;

  ~TrackData() = default;
};

struct EventData {
  G4long event_count{};
  G4long run_num{};
  G4long event_num{};

  // Initial data
  G4ThreeVector start{};
  G4ThreeVector end{};

  G4double energy{};
  G4int particle_num{};

  // Data after simulation
  G4double theta{}, phi{};

  G4double energy_dep{};
  G4long particle_count{};
  G4double track_length{};
  G4long muon_count{};
  G4double energy_start{};
  G4double energy_end{};

  std::chrono::duration<double> duration{};

  // NEVOD
  std::pair<TrackData, TrackData> muon_nevod{};

  // DECOR
  vector3d<G4double> muon_decor;

  // SCT
  std::vector<G4double> edep_count_sct_flatten;

  // CWD NEVOD
  std::vector<G4int> photoelectron_num;
  // vector_4d amplitude_qsm;

  // TODO: write proper constructor with initialization, loading and saving in file
  EventData() = default;

  ~EventData() = default;
};

}  // namespace nevod

#endif  // EVENT_DATA_HH