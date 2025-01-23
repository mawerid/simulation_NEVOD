#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH

#include "G4Box.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VisAttributes.hh"
#include "control/Communicator.hh"
#include "detector/sensetive/Control.hh"
#include "detector/sensetive/DECOR.hh"
#include "detector/sensetive/QSM.hh"
#include "detector/sensetive/SCT.hh"
#include "detector/sensetive/Water.hh"
#include "globals.hh"

#define PMT_PER_QSM 6

#define DECOR_COUNT 8
#define DECOR_CHAMBER_COUNT 8

#define BOX_SIDE_COUNT 6

namespace nevod {

class DetectorConstruction : public G4VUserDetectorConstruction {
 public:
  DetectorConstruction();
  DetectorConstruction(Communicator* communicator);
  ~DetectorConstruction() override = default;

  G4VPhysicalVolume* Construct() override;
  void ConstructSDandField() override;

 private:
  void GenerateMaterials();

  void BuildNEVOD();
  void BuildOtherBuildings();

  // NEVOD
  void BuildCWD();

  // DECOR
  void BuildDECOR();

  // SCT
  void BuildSCT();

  // PRISMA-URAN
  void BuildPRISMA();
  void BuildURAN();

  // NEVOD-EAS
  void BuildEAS();

  Communicator* communicator_ = nullptr;

  // option to activate checking of volumes overlaps
  G4bool check_overlaps_ = true;

  G4double experimental_hall_x_;
  G4double experimental_hall_y_;
  G4double experimental_hall_z_;
  G4Box* world_box_ = nullptr;
  G4LogicalVolume* world_log_ = nullptr;
  G4VPhysicalVolume* world_phys_ = nullptr;

  // Configuration
  ConstructionFlags construction_flags_;

  // CWD
  G4int cwd_plane_number_;
  std::vector<G4int> stride_config_;
  std::vector<G4int> qsm_config_;

  G4Tubs* air_tube_ = nullptr;
  vector4d<G4LogicalVolume*> air_tube_log_;
  vector4d<G4VPhysicalVolume*> air_tube_phys_;

  G4Tubs* photocathode_tube_ = nullptr;
  vector4d<G4LogicalVolume*> photocathode_log_;
  vector4d<G4VPhysicalVolume*> photocathode_phys_;

  // SCT (2 for outer and inner)
  std::pair<G4int, G4int> sct_plane_number_;
  std::pair<G4int, G4int> sct_counter_number_;
  std::vector<std::pair<G4Box*, G4Box*>> sct_counter_box_;
  std::vector<std::pair<G4LogicalVolume*, G4LogicalVolume*>> sct_counter_log_;
  std::vector<std::pair<G4VPhysicalVolume*, G4VPhysicalVolume*>> sct_counter_phys_;

  std::vector<G4Box*> sct_scint_box_;
  std::vector<G4LogicalVolume*> sct_scint_log_;
  std::vector<G4VPhysicalVolume*> sct_scint_phys_;

  // DECOR (2 for X and Y stripes)
  std::vector<std::vector<std::array<G4Box*, 2>>> super_module_box_;
  std::vector<std::vector<std::array<G4LogicalVolume*, 2>>> super_module_log_;
  std::vector<std::vector<std::array<G4VPhysicalVolume*, 2>>> super_module_phys_;

  // mu track detection in NEVOD
  std::vector<G4Box*> control_nevod_box_{BOX_SIDE_COUNT};
  std::vector<G4LogicalVolume*> control_nevod_log_{BOX_SIDE_COUNT};
  std::vector<G4VPhysicalVolume*> control_nevod_phys_{BOX_SIDE_COUNT};
  G4Box* water_box_ = nullptr;
  G4LogicalVolume* water_log_ = nullptr;
  G4VPhysicalVolume* water_phys_ = nullptr;

  /*
  // NEVOD-EAS
  G4Trd *trdRoofNE[36]{}; // 28
  G4LogicalVolume *logRoofNE[36]{};
  G4VPhysicalVolume *physRoofNE[36]{};

  G4Trd *trdAirRoofNE[36]{};
  G4LogicalVolume *logAirRoofNE[36]{};
  G4VPhysicalVolume *physAirRoofNE[36]{};

  G4Box *boxFloorNE[5]{};
  G4LogicalVolume *logFloorNE[5]{};
  G4VPhysicalVolume *physFloorNE[5]{};

  G4Box *boxCaseNE[144]{}; // 112
  G4LogicalVolume *logCaseNE[144]{};
  G4VPhysicalVolume *physCaseNE[144]{};

  G4Box *boxAirCaseNE[144]{};
  G4LogicalVolume *logAirCaseNE[144]{};
  G4VPhysicalVolume *physAirCaseNE[144]{};

  G4Box *boxScintNE[144]{};
  G4LogicalVolume *logScintNE[144]{};
  G4VPhysicalVolume *physScintNE[144]{};

  G4Box *boxWallStatNEX1[36]{}; //
  G4LogicalVolume *logWallStatNEX1[36]{};
  G4VPhysicalVolume *physWallStatNEX1[36]{};

  G4Box *boxWallStatNEX2[36]{}; //
  G4LogicalVolume *logWallStatNEX2[36]{};
  G4VPhysicalVolume *physWallStatNEX2[36]{};

  G4Box *boxWallStatNEY1[36]{}; //
  G4LogicalVolume *logWallStatNEY1[36]{};
  G4VPhysicalVolume *physWallStatNEY1[36]{};

  G4Box *boxWallStatNEY2[36]{}; //
  G4LogicalVolume *logWallStatNEY2[36]{};
  G4VPhysicalVolume *physWallStatNEY2[36]{};

  G4Tubs *tubeCable[560]{};
  G4LogicalVolume *logCable[560]{};
  G4VPhysicalVolume *physCable[560]{};

  G4Tubs *tubeRing[182]{};
  G4LogicalVolume *logRing[182]{};
  G4VPhysicalVolume *physRing[182]{};

  // PRISMA-URAN
  G4Tubs *solidDet[32]{};
  G4LogicalVolume *logicDet[32]{};
  G4VPhysicalVolume *physDet[32]{};

  G4Tubs *solidBar1[32]{};
  G4LogicalVolume *logicBar1[32]{};
  G4VPhysicalVolume *physBar1[32]{};

  G4Tubs *solidBarUp[32]{};
  G4LogicalVolume *logicBarUp[32]{};
  G4VPhysicalVolume *physBarUp[32]{};

  G4Tubs *solidBarDown[32]{};
  G4LogicalVolume *logicBarDown[32]{};
  G4VPhysicalVolume *physBarDown[32]{};

  G4Tubs *solidDet2[72]{};
  G4LogicalVolume *logicDet2[72]{};
  G4VPhysicalVolume *physDet2[72]{};

  G4Tubs *solidBar2[72]{};
  G4LogicalVolume *logicBar2[72]{};
  G4VPhysicalVolume *physBar2[72]{};

  G4Tubs *solidBarUp2[72]{};
  G4LogicalVolume *logicBarUp2[72]{};
  G4VPhysicalVolume *physBarUp2[72]{};

  G4Tubs *solidBarDown2[72]{};
  G4LogicalVolume *logicBarDown2[72]{};
  G4VPhysicalVolume *physBarDown2[72]{};

  G4Box *boxTarget[72]{};
  G4LogicalVolume *logTarget[72]{};
  G4VPhysicalVolume *physTarget[72]{};

  G4Box *boxModerate[72]{};
  G4LogicalVolume *logModerate[72]{};
  G4VPhysicalVolume *physModerate[72]{};
  */
};
}  // namespace nevod

#endif  // DETECTORCONSTRUCTION_HH
