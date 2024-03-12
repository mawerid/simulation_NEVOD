#ifndef NVDDETECTORCONSTRUCTION_HH
#define NVDDETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;

class NVDDetectorConstruction : public G4VUserDetectorConstruction {
public:
  NVDDetectorConstruction();
  ~NVDDetectorConstruction() override;

public:
  G4VPhysicalVolume *Construct() override;

private:
  G4double expHall_x;
  G4double expHall_y;
  G4double expHall_z;

  G4bool fCheckOverlaps; // option to activate checking of volumes overlaps ###

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
};

#endif // NVDDETECTORCONSTRUCTION_HH
