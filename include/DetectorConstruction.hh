#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4Box;
class G4Trd;
class G4Tubs;
class G4LogicalVolume;
class G4VPhysicalVolume;

namespace NEVOD {

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
  DetectorConstruction();
  ~DetectorConstruction() override;

public:
  G4VPhysicalVolume *Construct() override;

private:
  G4double expHall_x;
  G4double expHall_y;
  G4double expHall_z;

  G4bool fCheckOverlaps; // option to activate checking of volumes overlaps

  G4Box *boxCount[80]{}; // SKT cnt
  G4LogicalVolume *logCount[80]{};
  G4VPhysicalVolume *physCount[80]{};

  G4Box *boxCountA[80]{};
  G4LogicalVolume *logCountA[80]{};
  G4VPhysicalVolume *physCountA[80]{};

  G4Box *boxScint[80]{};
  G4LogicalVolume *logScint[80]{};
  G4VPhysicalVolume *physScint[80]{};

  G4Box *boxSM0x[8]{};
  G4LogicalVolume *logSM0x[8]{};
  G4VPhysicalVolume *physSM0x[8]{};

  G4Box *boxSM1x[8]{};
  G4LogicalVolume *logSM1x[8]{};
  G4VPhysicalVolume *physSM1x[8]{};

  G4Box *boxSM2x[8]{};
  G4LogicalVolume *logSM2x[8]{};
  G4VPhysicalVolume *physSM2x[8]{};

  G4Box *boxSM3x[8]{};
  G4LogicalVolume *logSM3x[8]{};
  G4VPhysicalVolume *physSM3x[8]{};

  G4Box *boxSM4x[8]{};
  G4LogicalVolume *logSM4x[8]{};
  G4VPhysicalVolume *physSM4x[8]{};

  G4Box *boxSM5x[8]{};
  G4LogicalVolume *logSM5x[8]{};
  G4VPhysicalVolume *physSM5x[8]{};

  G4Box *boxSM6x[8]{};
  G4LogicalVolume *logSM6x[8]{};
  G4VPhysicalVolume *physSM6x[8]{};

  G4Box *boxSM7x[8]{};
  G4LogicalVolume *logSM7x[8]{};
  G4VPhysicalVolume *physSM7x[8]{};

  G4Box *boxSM0y[8]{}; // DECOR SM
  G4LogicalVolume *logSM0y[8]{};
  G4VPhysicalVolume *physSM0y[8]{};

  G4Box *boxSM1y[8]{};
  G4LogicalVolume *logSM1y[8]{};
  G4VPhysicalVolume *physSM1y[8]{};

  G4Box *boxSM2y[8]{};
  G4LogicalVolume *logSM2y[8]{};
  G4VPhysicalVolume *physSM2y[8]{};

  G4Box *boxSM3y[8]{};
  G4LogicalVolume *logSM3y[8]{};
  G4VPhysicalVolume *physSM3y[8]{};

  G4Box *boxSM4y[8]{};
  G4LogicalVolume *logSM4y[8]{};
  G4VPhysicalVolume *physSM4y[8]{};

  G4Box *boxSM5y[8]{};
  G4LogicalVolume *logSM5y[8]{};
  G4VPhysicalVolume *physSM5y[8]{};

  G4Box *boxSM6y[8]{};
  G4LogicalVolume *logSM6y[8]{};
  G4VPhysicalVolume *physSM6y[8]{};

  G4Box *boxSM7y[8]{};
  G4LogicalVolume *logSM7y[8]{};
  G4VPhysicalVolume *physSM7y[8]{};

  G4Box *boxCtrlNVD[8]{}; // mu track
  G4LogicalVolume *logCtrlNVD[8]{};
  G4VPhysicalVolume *physCtrlNVD[8]{};

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
};
} // namespace NEVOD

#endif // DETECTORCONSTRUCTION_HH
