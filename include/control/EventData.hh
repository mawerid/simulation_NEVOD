#ifndef EVENT_DATA_HH
#define EVENT_DATA_HH

#include <vector>

#include "G4RandomTools.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "Rtypes.h"
#include "TFile.h"
#include "TMath.h"
#include "TObject.h"
#include "TROOT.h"
#include "TTree.h"
#include "TVector3.h"
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
};

struct ParticleData {
  ULong_t particle_id{};
  ULong_t particle_num{};
  TVector3 coordinate{};  // in metres
  TVector3 momentum{};
  Double_t energy{};  // in GeV
};

struct Particles {
  std::vector<ULong_t> particle_id{};
  std::vector<ULong_t> particle_num{};
  std::vector<TVector3> coordinate{};  // in metres
  std::vector<TVector3> momentum{};
  std::vector<Double_t> energy{};  // in GeV

  Particles() = default;
  ~Particles() = default;

  void push_back(const ParticleData particle);
  void push_back(const ULong_t particle_id, const ULong_t particle_num, const TVector3 coordinate, const TVector3 momentum, const Double_t energy);

  void resize(const size_t size);
  void clear();
  ParticleData operator[](const size_t index) const;
  size_t size() const;
};

struct EventData {
  ULong_t event_id{};
  ULong_t primary_particle_id{};
  ULong_t particle_amount{};
  Double_t theta{}, phi{};  // in degrees

  // Initial data
  Particles particles;

  // Data after simulation
  Double_t theta_rec{}, phi_rec{};  // in degrees

  Double_t energy_dep{};  // in GeV
  ULong_t particle_count{};
  Double_t track_length{};  // in metres
  ULong_t muon_count{};
  Double_t energy_start{};  // in GeV
  Double_t energy_end{};    // in GeV

  std::chrono::steady_clock::time_point start_time;
  Long64_t duration{};  // in nanoseconds

  // NEVOD
  std::pair<TrackData, TrackData> muon_nevod;

  // DECOR
  vector3d<Double_t> muon_decor;
  vector3d<Double_t> muon_decor_w;

  // SCT
  vector3d<Double_t> edep_count_sct;

  // CWD NEVOD
  std::vector<Int_t> photoelectron_num;

  vector4d<Double_t> amplitude_qsm;

  EventData();
  EventData(ULong_t event_id, ULong_t primary_particle_id, ULong_t particle_amount, Double_t theta, Double_t phi);

  ~EventData() = default;

  void ConnectHeaderTree(TTree* tree);
  void ConnectEventTree(TTree* tree);

  void Print() const;
  std::ostream& operator<<(std::ostream& os) const;

  void Clear(G4bool clear_header = true);
};

}  // namespace nevod

#endif  // EVENT_DATA_HH