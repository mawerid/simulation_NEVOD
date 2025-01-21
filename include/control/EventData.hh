#ifndef EVENT_DATA_HH
#define EVENT_DATA_HH

#include "G4ThreeVector.hh"
#include "globals.hh"

namespace nevod {

using vector_3d = std::vector<std::vector<std::vector<G4double>>>;
using vector_4d = std::vector<std::vector<std::vector<std::vector<G4double>>>>;

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
  vector_3d muon_decor;

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