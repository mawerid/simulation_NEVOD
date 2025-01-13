#ifndef EVENT_DATA_HH
#define EVENT_DATA_HH

#include "globals.hh"

namespace nevod {

using vector_3d = std::vector<std::vector<std::vector<G4double>>>;
using vector_4d = std::vector<std::vector<std::vector<std::vector<G4double>>>>;

struct EventData {
  G4long event_count{};
  G4long run_num{};
  G4long event_num{};

  // Initial data
  G4double start_x{}, start_y{}, start_z{};
  G4double end_x{}, end_y{}, end_z{};

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

  // DECOR
  G4long mu_decor{}, mu_sm{}, n_sm{}, mu_decor_w{}, mu_sm_w{}, n_sm_w{};

  std::vector<G4long> h_sm;
  std::vector<G4long> n_track_sm;
  std::vector<G4long> n_track_sm_x;
  std::vector<G4long> n_track_sm_y;

  // SCT
  vector_3d edep_count_sct;

  // CWD NEVOD
  vector_4d amplitude_qsm;

  EventData() = default;

  ~EventData() = default;
};
}  // namespace nevod

#endif  // EVENT_DATA_HH