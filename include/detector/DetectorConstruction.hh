#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "control/Communicator.hh"
#include "globals.hh"

#define SCT_COUNT 80
#define DECOR_COUNT 8
#define DECOR_CHAMBER_COUNT 8

class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;

namespace nevod {

class DetectorConstruction : public G4VUserDetectorConstruction {
 public:
  DetectorConstruction();
  DetectorConstruction(Communicator* communicator);
  ~DetectorConstruction() override;

  G4VPhysicalVolume* Construct() override;
  void ConstructSDandField() override;

 private:
  void BuildWorld();
  void BuildCWD();
  void BuildDECOR();
  void BuildSCT();
  void BuildPRISMA();
  void BuildURAN();
  void BuildEAS();

  // TODO add building of each PMT and QSM

  Communicator* communicator_;

  // option to activate checking of volumes overlaps
  G4bool check_overlaps = true;

  G4double experimental_hall_x_;
  G4double experimental_hall_y_;
  G4double experimental_hall_z_;

  // SCT
  G4Box* boxCount[SCT_COUNT]{};
  G4LogicalVolume* logCount[SCT_COUNT]{};
  G4VPhysicalVolume* physCount[SCT_COUNT]{};

  G4Box* boxCountA[SCT_COUNT]{};
  G4LogicalVolume* logCountA[SCT_COUNT]{};
  G4VPhysicalVolume* physCountA[SCT_COUNT]{};

  G4Box* boxScint[SCT_COUNT]{};
  G4LogicalVolume* logScint[SCT_COUNT]{};
  G4VPhysicalVolume* physScint[SCT_COUNT]{};

  // DECOR
  G4Box* boxSMx[DECOR_COUNT][DECOR_CHAMBER_COUNT]{};
  G4LogicalVolume* logSMx[DECOR_COUNT][DECOR_CHAMBER_COUNT]{};
  G4VPhysicalVolume* physSMx[DECOR_COUNT][DECOR_CHAMBER_COUNT]{};

  G4Box* boxSMy[DECOR_COUNT][DECOR_CHAMBER_COUNT]{};
  G4LogicalVolume* logSMy[DECOR_COUNT][DECOR_CHAMBER_COUNT]{};
  G4VPhysicalVolume* physSMy[DECOR_COUNT][DECOR_CHAMBER_COUNT]{};

  // mu track
  G4Box* boxCtrlNVD[8]{};
  G4LogicalVolume* logCtrlNVD[8]{};
  G4VPhysicalVolume* physCtrlNVD[8]{};

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
