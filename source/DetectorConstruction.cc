#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"

#include "G4UserLimits.hh"

#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"

namespace NEVOD {

extern G4int perevKM[546][4];
extern G4int feuCount;

// int dXY(int DS, int det, double &x, double &y);

DetectorConstruction::DetectorConstruction() : fCheckOverlaps(true) {
  expHall_x = expHall_y = expHall_z = 10 * m;
}

DetectorConstruction::~DetectorConstruction() { ; }

G4VPhysicalVolume *DetectorConstruction::Construct() {

  //------------- Materials -------------

  G4NistManager *nman = G4NistManager::Instance();

  auto mFe = nman->FindOrBuildMaterial("G4_Fe");
  auto Steel = nman->FindOrBuildMaterial("G4_STAINLESS-STEEL");
  auto Rock = nman->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  auto rubber = nman->FindOrBuildMaterial("G4_RUBBER_NATURAL");

  G4double a, z, density, fractionmass;
  G4int nelements;

  // Air
  auto N = new G4Element("Nitrogen", "N", z = 7, a = 14.01 * g / mole);
  auto O = new G4Element("Oxygen", "O", z = 8, a = 16.00 * g / mole);

  auto Air = new G4Material("Air", density = 1.29 * mg / cm3, nelements = 2);
  Air->AddElement(N, 70. * perCent);
  Air->AddElement(O, 30. * perCent);

  // Water
  auto H = new G4Element("Hydrogen", "H", z = 1, a = 1.01 * g / mole);

  auto Water = new G4Material("Water", density = 1.0 * g / cm3, nelements = 2);
  Water->AddElement(H, 2);
  Water->AddElement(O, 1);

  // Concrete
  auto C = new G4Element("Carbon", "C", z = 6, a = 12.01 * g / mole);
  auto Na = new G4Element("Natrium", "Na", z = 11, a = 22.99 * g / mole);
  auto Hg = new G4Element("Hg", "Hg", z = 80, a = 200.59 * g / mole);
  auto Al = new G4Element("Aluminium", "Al", z = 13, a = 26.98 * g / mole);
  auto Si = new G4Element("Silicon", "Si", z = 14, a = 28.09 * g / mole);
  auto K = new G4Element("K", "K", z = 19, a = 39.1 * g / mole);
  auto Ca = new G4Element("Calzium", "Ca", z = 31, a = 69.72 * g / mole);
  auto Fe = new G4Element("Iron", "Fe", z = 26, a = 55.85 * g / mole);
  auto B = new G4Element("Boron", "B", z = 5., a = 10.811 * g / mole);

  auto Concrete = new G4Material("Concrete", density = 2.3 * g / cm3, 10);
  Concrete->AddElement(H, fractionmass = 0.01);
  Concrete->AddElement(O, fractionmass = 0.529);
  Concrete->AddElement(Na, fractionmass = 0.016);
  Concrete->AddElement(Hg, fractionmass = 0.002);
  Concrete->AddElement(Al, fractionmass = 0.034);
  Concrete->AddElement(Si, fractionmass = 0.337);
  Concrete->AddElement(K, fractionmass = 0.013);
  Concrete->AddElement(Ca, fractionmass = 0.044);
  Concrete->AddElement(Fe, fractionmass = 0.014);
  Concrete->AddElement(C, fractionmass = 0.001);

  // Aluminum
  auto mAl = new G4Material("Aluminum", z = 13., a = 26.98 * g / mole,
                            density = 2.7 * g / cm3);

  // Plexiglass
  auto Plexiglass =
      new G4Material("Plexiglass", density = 1.19 * g / cm3, nelements = 3);
  Plexiglass->AddElement(C, 0.60);
  Plexiglass->AddElement(O, 0.32);
  Plexiglass->AddElement(H, 0.08);

  // Silikon
  auto Silicone =
      new G4Material("Silicone", density = 0.98 * g / cm3, nelements = 4);
  Silicone->AddElement(C, 6);
  Silicone->AddElement(H, 18);
  Silicone->AddElement(O, 1);
  Silicone->AddElement(Si, 2);

  // Glass
  auto Glass = new G4Material("Glass", density = 2.23 * g / cm3, nelements = 4);
  Glass->AddElement(Si, 80 * 1);
  Glass->AddElement(B, 12 * 2);
  Glass->AddElement(Na, 8 * 2);
  Glass->AddElement(O, 80 * 2 + 12 * 3 + 8 * 1);

  // Kirpich
  auto Kirpich =
      new G4Material("Kirpich", density = 1.8 * g / cm3, nelements = 2);
  Kirpich->AddElement(Si, 1);
  Kirpich->AddElement(O, 2);

  // Keramzit
  auto Keramzit =
      new G4Material("Keramzit", density = 0.5 * g / cm3, nelements = 2);
  Keramzit->AddElement(Si, 1);
  Keramzit->AddElement(O, 2);

  // Scintillator
  auto Scintillator = new G4Material("Scintillator", density = 1.05 * g / cm3,
                                     nelements = 2); // ???1.06
  Scintillator->AddElement(H, 8);
  Scintillator->AddElement(C, 8);

  auto C9H10 = nman->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

  // PRISMA-URAN
  auto bar_mat = nman->FindOrBuildMaterial("G4_POLYETHYLENE");
  auto floor_mat = nman->FindOrBuildMaterial("G4_CONCRETE");
  auto floor_mat2 = new G4Material("floor_mat2", 0.9 * g / cm3, 3);
  floor_mat2->AddElement(H, 4);
  floor_mat2->AddElement(O, 1);
  floor_mat2->AddElement(C, 2);
  // PRISMA-URAN

  //
  // ------------ Generate & Add Material Properties Table ------------
  //

  // Water
  const G4int nEntries = 32;

  G4double PhotonEnergy[nEntries] = {
      2.034 * eV, 2.068 * eV, 2.103 * eV, 2.139 * eV, 2.177 * eV, 2.216 * eV,
      2.256 * eV, 2.298 * eV, 2.341 * eV, 2.386 * eV, 2.433 * eV, 2.481 * eV,
      2.532 * eV, 2.585 * eV, 2.640 * eV, 2.697 * eV, 2.757 * eV, 2.820 * eV,
      2.885 * eV, 2.954 * eV, 3.026 * eV, 3.102 * eV, 3.181 * eV, 3.265 * eV,
      3.353 * eV, 3.446 * eV, 3.545 * eV, 3.649 * eV, 3.760 * eV, 3.877 * eV,
      4.002 * eV, 4.136 * eV};

  G4double RefractiveIndexW[nEntries] = {
      1.3435, 1.344,  1.3445, 1.345,  1.3455, 1.346,  1.3465, 1.347,
      1.3475, 1.348,  1.3485, 1.3492, 1.35,   1.3505, 1.351,  1.3518,
      1.3522, 1.3530, 1.3535, 1.354,  1.3545, 1.355,  1.3555, 1.356,
      1.3568, 1.3572, 1.358,  1.3585, 1.359,  1.3595, 1.36,   1.3608};

  G4double AbsorptionW[nEntries] = {
      3.448 * m,  4.082 * m,  6.329 * m,  9.174 * m,  12.346 * m, 13.889 * m,
      15.152 * m, 17.241 * m, 18.868 * m, 20.000 * m, 26.316 * m, 35.714 * m,
      45.455 * m, 47.619 * m, 52.632 * m, 52.632 * m, 55.556 * m, 52.632 * m,
      52.632 * m, 47.619 * m, 45.455 * m, 41.667 * m, 37.037 * m, 33.333 * m,
      30.000 * m, 28.500 * m, 27.000 * m, 24.500 * m, 22.000 * m, 19.500 * m,
      17.500 * m, 14.500 * m};

  G4double energy_water[] = {
      1.56962 * eV, 1.58974 * eV, 1.61039 * eV, 1.63157 * eV, 1.65333 * eV,
      1.67567 * eV, 1.69863 * eV, 1.72222 * eV, 1.74647 * eV, 1.77142 * eV,
      1.7971 * eV,  1.82352 * eV, 1.85074 * eV, 1.87878 * eV, 1.90769 * eV,
      1.93749 * eV, 1.96825 * eV, 1.99999 * eV, 2.03278 * eV, 2.06666 * eV,
      2.10169 * eV, 2.13793 * eV, 2.17543 * eV, 2.21428 * eV, 2.25454 * eV,
      2.29629 * eV, 2.33962 * eV, 2.38461 * eV, 2.43137 * eV, 2.47999 * eV,
      2.53061 * eV, 2.58333 * eV, 2.63829 * eV, 2.69565 * eV, 2.75555 * eV,
      2.81817 * eV, 2.88371 * eV, 2.95237 * eV, 3.02438 * eV, 3.09999 * eV,
      3.17948 * eV, 3.26315 * eV, 3.35134 * eV, 3.44444 * eV, 3.54285 * eV,
      3.64705 * eV, 3.75757 * eV, 3.87499 * eV, 3.99999 * eV, 4.13332 * eV,
      4.27585 * eV, 4.42856 * eV, 4.59258 * eV, 4.76922 * eV, 4.95999 * eV,
      5.16665 * eV, 5.39129 * eV, 5.63635 * eV, 5.90475 * eV, 6.19998 * eV};

  // assume 100 times larger than the rayleigh scattering for now
  G4double mie_water[] = {
      167024.4 * m, 158726.7 * m, 150742 * m,   143062.5 * m, 135680.2 * m,
      128587.4 * m, 121776.3 * m, 115239.5 * m, 108969.5 * m, 102958.8 * m,
      97200.35 * m, 91686.86 * m, 86411.33 * m, 81366.79 * m, 76546.42 * m,
      71943.46 * m, 67551.29 * m, 63363.36 * m, 59373.25 * m, 55574.61 * m,
      51961.24 * m, 48527.00 * m, 45265.87 * m, 42171.94 * m, 39239.39 * m,
      36462.50 * m, 33835.68 * m, 31353.41 * m, 29010.30 * m, 26801.03 * m,
      24720.42 * m, 22763.36 * m, 20924.88 * m, 19200.07 * m, 17584.16 * m,
      16072.45 * m, 14660.38 * m, 13343.46 * m, 12117.33 * m, 10977.70 * m,
      9920.416 * m, 8941.407 * m, 8036.711 * m, 7202.470 * m, 6434.927 * m,
      5730.429 * m, 5085.425 * m, 4496.467 * m, 3960.210 * m, 3473.413 * m,
      3032.937 * m, 2635.746 * m, 2278.907 * m, 1959.588 * m, 1675.064 * m,
      1422.710 * m, 1200.004 * m, 1004.528 * m, 833.9666 * m, 686.1063 * m};

  // ### OpNovice
  G4double cosTeta = 0.99;     // c
  G4double cosTetaBack = 0.99; // c
  G4double f_ratio = 0.8;      // ###

  auto myMPTW = new G4MaterialPropertiesTable();
  myMPTW->AddProperty("RINDEX", PhotonEnergy, RefractiveIndexW, nEntries);
  myMPTW->AddProperty("ABSLENGTH", PhotonEnergy, AbsorptionW, nEntries);

  myMPTW->AddProperty("MIEHG", energy_water, mie_water, 60); // ### OpNovice
  myMPTW->AddConstProperty("MIEHG_FORWARD", cosTeta);
  myMPTW->AddConstProperty("MIEHG_BACKWARD", cosTeta);
  myMPTW->AddConstProperty("MIEHG_FORWARD_RATIO", f_ratio);

  Water->SetMaterialPropertiesTable(myMPTW);

  // Plexiglass
  G4double RefractiveIndexP[nEntries];
  for (double &i : RefractiveIndexP)
    i = 1.474;

  G4double AbsorptionP[nEntries];
  for (double &i : AbsorptionP)
    i = 40.0 * m; //???

  auto myMPTP = new G4MaterialPropertiesTable();
  myMPTP->AddProperty("RINDEX", PhotonEnergy, RefractiveIndexP, nEntries);
  myMPTP->AddProperty("ABSLENGTH", PhotonEnergy, AbsorptionP, nEntries);
  Plexiglass->SetMaterialPropertiesTable(myMPTP);

  // Glass
  G4double RefractiveIndexG[nEntries];
  for (double &i : RefractiveIndexG)
    i = 1.49;

  G4double AbsorptionG[nEntries];
  for (double &i : AbsorptionG)
    i = 40.0 * m; //???

  auto myMPTG = new G4MaterialPropertiesTable();
  myMPTG->AddProperty("RINDEX", PhotonEnergy, RefractiveIndexG, nEntries);
  myMPTG->AddProperty("ABSLENGTH", PhotonEnergy, AbsorptionG, nEntries);
  Glass->SetMaterialPropertiesTable(myMPTG);

  // Silicone
  G4double RefractiveIndexS[nEntries];
  for (double &i : RefractiveIndexS)
    i = 1.4325; //??? SKTH-B rus
  G4double AbsorptionS[nEntries];
  for (double &i : AbsorptionS)
    i = 40.0 * m; //???

  auto myMPTS = new G4MaterialPropertiesTable();
  myMPTS->AddProperty("RINDEX", PhotonEnergy, RefractiveIndexS, nEntries);
  myMPTS->AddProperty("ABSLENGTH", PhotonEnergy, AbsorptionS, nEntries);
  Silicone->SetMaterialPropertiesTable(myMPTS);

  // Air
  G4double RefractiveIndexA[nEntries];
  for (double &i : RefractiveIndexA)
    i = 1.0;

  auto myMPTA = new G4MaterialPropertiesTable();
  myMPTA->AddProperty("RINDEX", PhotonEnergy, RefractiveIndexA, nEntries);
  Air->SetMaterialPropertiesTable(myMPTA);

  //------------- Volumes --------------

  // The experimental Hall
  expHall_x = 200. * m; //-47+139
  expHall_y = 200. * m; //-162+117
  expHall_z = 50. * m;

  auto expHall_box = new G4Box("World", expHall_x, expHall_y, expHall_z);
  auto expHall_log =
      new G4LogicalVolume(expHall_box, Air, "World", nullptr, nullptr, nullptr);
  G4VPhysicalVolume *expHall_phys = new G4PVPlacement(
      nullptr, G4ThreeVector(), expHall_log, "World", nullptr, false, 0);

  // Krisha
  auto KrishaKer_g =
      new G4Box("KrishaKer", 17.95 / 2. * m, 15.45 * m, 0.125 * m);
  auto KrishaKer_log = new G4LogicalVolume(KrishaKer_g, Keramzit, "KrishaKer",
                                           nullptr, nullptr, nullptr);
  G4VPhysicalVolume *KrishaKer_phys = new G4PVPlacement(
      nullptr, G4ThreeVector(-4.45 / 2.0 * m, 0. * m, 11.675 * m),
      KrishaKer_log, "KrishaKer", expHall_log, false, 0, fCheckOverlaps);

  auto KrishaBet_g =
      new G4Box("KrishaBet", 17.95 / 2. * m, 15.45 * m, 0.05 * m);
  auto KrishaBet_log = new G4LogicalVolume(KrishaBet_g, Kirpich, "KrishaBet",
                                           nullptr, nullptr, nullptr);
  G4VPhysicalVolume *KrishaBet_phys = new G4PVPlacement(
      nullptr, G4ThreeVector(-4.45 / 2.0 * m, 0. * m, 11.5 * m), KrishaBet_log,
      "KrishaBet", expHall_log, false, 0, fCheckOverlaps);

  // Steni
  auto StY_g = new G4Box("Steni", 17.95 / 2. * m, 0.55 / 2. * m,
                         (9 + 0.6 + 6.65) / 2.0 * m);
  auto StY_log = new G4LogicalVolume(StY_g, Concrete, "Steni");
  G4VPhysicalVolume *StYmin_phys = new G4PVPlacement(
      nullptr, G4ThreeVector(-4.45 / 2.0 * m, -15.175 * m, 3.325 * m), StY_log,
      "Steni", expHall_log, false, 0, fCheckOverlaps);
  G4VPhysicalVolume *StYmax_phys = new G4PVPlacement(
      nullptr, G4ThreeVector(-4.45 / 2.0 * m, 15.175 * m, 3.325 * m), StY_log,
      "Steni", expHall_log, false, 0, fCheckOverlaps);

  auto StX_g =
      new G4Box("Steni", 0.55 / 2. * m, 14.9 * m, (9 + 0.6 + 6.65) / 2.0 * m);
  auto StX_log = new G4LogicalVolume(StX_g, Concrete, "Steni");
  G4VPhysicalVolume *StXmin_phys = new G4PVPlacement(
      nullptr, G4ThreeVector(-10.925 * m, 0 * m, 3.325 * m), StX_log, "Steni",
      expHall_log, false, 0, fCheckOverlaps);
  G4VPhysicalVolume *StXmax_phys = new G4PVPlacement(
      nullptr, G4ThreeVector(6.475 * m, 0 * m, 3.325 * m), StX_log, "Steni",
      expHall_log, false, 0, fCheckOverlaps);

  // The concrete bassein
  G4double basx, basy, basz, posx, posy, posz, hsten;

  basx = 10.2;
  basy = 27.2;
  basz = 9.6;
  hsten = 0.6;
  posx = 0.;
  posy = 0.;
  posz = 0.;

  auto bassein_g =
      new G4Box("bassein", basx / 2. * m, basy / 2. * m, basz / 2. * m);
  auto bassein_l = new G4LogicalVolume(bassein_g, Concrete, "bassein", nullptr,
                                       nullptr, nullptr);

  G4VPhysicalVolume *bassein_phys = new G4PVPlacement(
      nullptr, G4ThreeVector(posx * m, posy * m, posz * m), bassein_l,
      "bassein", expHall_log, false, 0, fCheckOverlaps);

  auto krishka_g =
      new G4Box("KrishkaBas", 10.2 / 2. * m, 27.2 / 2. * m, 0.005 / 2. * m);
  auto krishka_l = new G4LogicalVolume(krishka_g, mFe, "KrishkaBas", nullptr,
                                       nullptr, nullptr);

  G4VPhysicalVolume *krishka_phys = new G4PVPlacement(
      nullptr, G4ThreeVector(0. * m, 0. * m, (9.6 / 2. + 0.005 / 2.) * m),
      krishka_l, "KrishkaBas", expHall_log, false, 0, fCheckOverlaps);

  // The water
  G4double wx, wy, wz;

  wx = basx - 2. * hsten;
  wy = basy - 2. * hsten;
  wz = basz - hsten;
  posx = 0.;
  posy = 0.;
  posz = hsten / 2.;

  auto water_g = new G4Box("WaterBox", wx / 2. * m, wy / 2. * m, wz / 2. * m);
  auto water_l = new G4LogicalVolume(water_g, Water, "WaterBox", nullptr,
                                     nullptr, nullptr); // ###
  G4VPhysicalVolume *water_phys = new G4PVPlacement(
      nullptr, G4ThreeVector(posx * m, posy * m, posz * m), water_l, "WaterBox",
      bassein_l, false, 0, fCheckOverlaps);

  // The air
  G4double ax, ay, az;

  ax = wx;
  ay = wy;
  az = 0.5;
  posx = 0.;
  posy = 0.;
  posz = wz / 2. - az / 2.;

  auto air_g = new G4Box("AirBox", ax / 2. * m, ay / 2. * m, az / 2. * m);
  auto air_l =
      new G4LogicalVolume(air_g, Air, "AirBox", nullptr, nullptr, nullptr);
  G4VPhysicalVolume *air_phys = new G4PVPlacement(
      nullptr, G4ThreeVector(posx * m, posy * m, posz * m), air_l, "AirBox",
      //                                                  water_l,false,0);
      water_l, false, 0, fCheckOverlaps);

  //=====

  //===== PRISMA-URAN
  /*
    // Floor (1-Ymin)
    G4double floor1_sizeX = 10.2 * m;
    G4double floor1_sizeY = 1.3 * m;
    G4double floor1_sizeZ = 0.2 * m;
    auto solidFloor1 = new G4Box("Floor", // its name
                                 0.5 * floor1_sizeX, 0.5 * floor1_sizeY,
                                 0.5 * floor1_sizeZ); // its size

    auto logicFloor1 = new G4LogicalVolume(solidFloor1, // its solid
                                           floor_mat,   // its material
                                           "Floor");    // its name

    G4double floor1_posX = 0. * m;
    G4double floor1_posY = (-15.175 + 0.55 / 2. + 1.3 / 2.) * m;
    G4double floor1_posZ = (9.6 / 2. - 0.2 / 2. + 0.005) * m;
    G4VPhysicalVolume *physFloor1 =
        new G4PVPlacement(nullptr, // no rotation
                          G4ThreeVector(floor1_posX, floor1_posY, floor1_posZ),
    // logicFloor1,     // its logical volume "Floor",         // its name
                          expHall_log,     // its mother  volume
                          false,           // no boolean operation
                          0,               // copy number
                          fCheckOverlaps); // overlaps checking

    // Floor (2-Ymax)
    G4double floor2_posY = (15.175 - 0.55 / 2. - 1.3 / 2.) * m;
    auto solidFloor2 = new G4Box("Floor", // its name
                                 0.5 * floor1_sizeX, 0.5 * floor1_sizeY,
                                 0.5 * floor1_sizeZ);   // its size
    auto logicFloor2 = new G4LogicalVolume(solidFloor2, // its solid
                                           floor_mat,   // its material
                                           "Floor");    // its name
    G4VPhysicalVolume *physFloor2 =
        new G4PVPlacement(nullptr, // no rotation
                          G4ThreeVector(floor1_posX, floor2_posY, floor1_posZ),
    // logicFloor2,     // its logical volume "Floor",         // its name
                          expHall_log,     // its mother  volume
                          false,           // no boolean operation
                          0,               // copy number
                          fCheckOverlaps); // overlaps checking

    // Floor (3-Xmin) devide on 5 part
    G4double floor31_sizeX = (5.55 - 4.15) * m;
    G4double floor31_sizeY = (5.9) * m;
    auto solidFloor31 = new G4Box("Floor", // its name
                                  0.5 * floor31_sizeX, 0.5 * floor31_sizeY,
                                  0.5 * floor1_sizeZ); // its size

    auto logicFloor31 = new G4LogicalVolume(solidFloor31, // its solid
                                            floor_mat,    // its material
                                            "Floor");     // its name

    G4double floor31_posX = (-10.925 + 0.55 / 2. + 4.15 + (5.55 - 4.15) / 2.) *
    m; G4double floor31_posY = (29.8 / 2. - 5.9 / 2.) * m; G4VPhysicalVolume
    *physFloor31 = new G4PVPlacement( nullptr, // no rotation
        G4ThreeVector(floor31_posX, floor31_posY, floor1_posZ), //
        logicFloor31,    // its logical volume
        "Floor",         // its name
        expHall_log,     // its mother  volume
        false,           // no boolean operation
        0,               // copy number
        fCheckOverlaps); // overlaps checking

    G4double floor32_sizeX = (5.55 - 4.15) * m;
    G4double floor32_sizeY = (5.9) * m;
    auto solidFloor32 = new G4Box("Floor", // its name
                                  0.5 * floor32_sizeX, 0.5 * floor32_sizeY,
                                  0.5 * floor1_sizeZ); // its size

    auto logicFloor32 = new G4LogicalVolume(solidFloor32, // its solid
                                            floor_mat,    // its material
                                            "Floor");     // its name

    G4double floor32_posX = (-10.925 + 0.55 / 2. + 4.15 + (5.55 - 4.15) / 2.) *
    m; G4double floor32_posY = (-29.8 / 2. + 5.9 / 2.) * m; G4VPhysicalVolume
    *physFloor32 = new G4PVPlacement( nullptr, // no rotation
        G4ThreeVector(floor32_posX, floor32_posY, floor1_posZ), //
        logicFloor32,    // its logical volume
        "Floor",         // its name
        expHall_log,     // its mother  volume
        false,           // no boolean operation
        0,               // copy number
        fCheckOverlaps); // overlaps checking

    G4double floor33_sizeX = (5.55) * m;
    G4double floor33_sizeY = (18) * m;
    auto solidFloor33 = new G4Box("Floor", // its name
                                  0.5 * floor33_sizeX, 0.5 * floor33_sizeY,
                                  0.5 * floor1_sizeZ); // its size

    auto logicFloor33 = new G4LogicalVolume(solidFloor33, // its solid
                                            floor_mat,    // its material
                                            "Floor");     // its name

    G4double floor33_posX = (-10.925 + 0.55 / 2. + 5.55 / 2.) * m;
    G4double floor33_posY = 0 * m;
    G4VPhysicalVolume *physFloor33 = new G4PVPlacement(
        nullptr,                                                // no rotation
        G4ThreeVector(floor33_posX, floor33_posY, floor1_posZ), //
        logicFloor33,    // its logical volume
        "Floor",         // its name
        expHall_log,     // its mother  volume
        false,           // no boolean operation
        0,               // copy number
        fCheckOverlaps); // overlaps checking

    G4double floor34_sizeX = (4.15) * m;
    G4double floor34_sizeY = (5.9) * m;
    G4double floor3_sizeZ = (0.02) * m;
    auto solidFloor34 = new G4Box("Floor", // its name
                                  0.5 * floor34_sizeX, 0.5 * floor34_sizeY,
                                  0.5 * floor3_sizeZ); // its size

    auto logicFloor34 = new G4LogicalVolume(solidFloor34, // its solid
                                            floor_mat2,   // its material
                                            "Floor");     // its name

    G4double floor34_posX = (-10.925 + 0.55 / 2. + 4.15 / 2.) * m;
    G4double floor34_posY = (29.8 / 2. - 5.9 / 2.) * m;
    G4double floor34_posZ = (9.6 / 2. - 0.2 / 2. + 0.005 - 0.18) * m;
    G4VPhysicalVolume *physFloor34 = new G4PVPlacement(
        nullptr,                                                 // no rotation
        G4ThreeVector(floor34_posX, floor34_posY, floor34_posZ), //
        logicFloor34,    // its logical volume
        "Floor",         // its name
        expHall_log,     // its mother  volume
        false,           // no boolean operation
        0,               // copy number
        fCheckOverlaps); // overlaps checking

    G4double floor35_sizeX = (4.15) * m;
    G4double floor35_sizeY = (5.9) * m;

    auto solidFloor35 = new G4Box("Floor", // its name
                                  0.5 * floor35_sizeX, 0.5 * floor35_sizeY,
                                  0.5 * floor3_sizeZ); // its size

    auto logicFloor35 = new G4LogicalVolume(solidFloor35, // its solid
                                            floor_mat2,   // its material
                                            "Floor");     // its name

    G4double floor35_posX = (-10.925 + 0.55 / 2. + 4.15 / 2.) * m;
    G4double floor35_posY = (-29.8 / 2. + 5.9 / 2.) * m;

    G4VPhysicalVolume *physFloor35 = new G4PVPlacement(
        nullptr,                                                 // no rotation
        G4ThreeVector(floor35_posX, floor35_posY, floor34_posZ), //
        logicFloor35,    // its logical volume
        "Floor",         // its name
        expHall_log,     // its mother  volume
        false,           // no boolean operation
        0,               // copy number
        fCheckOverlaps); // overlaps checking

    // Floor (4-Xmax)
    G4double floor4_sizeX = 1.1 * m;
    G4double floor4_sizeY = 29.8 * m;
    auto solidFloor4 = new G4Box("Floor", // its name
                                 0.5 * floor4_sizeX, 0.5 * floor4_sizeY,
                                 0.5 * floor1_sizeZ); // its size

    auto logicFloor4 = new G4LogicalVolume(solidFloor4, // its solid
                                           floor_mat,   // its material
                                           "Floor");    // its name

    G4double floor4_posX = (6.475 - 0.55 / 2. - 1.1 / 2.) * m;
    G4double floor4_posY = 0 * m;
    G4VPhysicalVolume *physFloor4 =
        new G4PVPlacement(nullptr, // no rotation
                          G4ThreeVector(floor4_posX, floor4_posY, floor1_posZ),
    // logicFloor4,     // its logical volume "Floor",         // its name
                          expHall_log,     // its mother  volume
                          false,           // no boolean operation
                          0,               // copy number
                          fCheckOverlaps); // overlaps checking
    //-----

    // Material Prisma

    auto Zn = nman->FindOrBuildElement("Zn");
    auto S = nman->FindOrBuildElement("S");
    auto Ag = nman->FindOrBuildElement("Ag");

    auto ZnS = new G4Material("ZnS", 4.09 * g / cm3, 3, kStateSolid, 293 *
    kelvin, 1 * atmosphere); ZnS->AddElement(Zn, 49.995 * perCent);
    ZnS->AddElement(S, 49.995 * perCent);
    ZnS->AddElement(Ag, 0.01 * perCent);

    auto Li6 = new G4Isotope("Li6", 3, 6, 6.02 * g / mole);
    auto Li7 = new G4Isotope("Li7", 3, 7, 7.02 * g / mole);
    auto Li = new G4Element("Li", "Li", 2);
    Li->AddIsotope(Li6, 90.000 * perCent);
    Li->AddIsotope(Li7, 10.000 * perCent);

    auto F = nman->FindOrBuildElement("F");
    auto LiF = new G4Material("LiF", 2.64 * g / cm3, 2, kStateSolid, 293 *
    kelvin, 1 * atmosphere); LiF->AddElement(Li, 1); LiF->AddElement(F, 1);

    auto LiF_ZnS = new G4Material("LiF_ZnS", 3.51 * g / cm3, 2, kStateSolid,
                                  293 * kelvin, 1 * atmosphere);
    LiF_ZnS->AddMaterial(LiF, 40 * perCent);
    LiF_ZnS->AddMaterial(ZnS, 60 * perCent);

    auto B1 = nman->FindOrBuildElement("B");
    auto B2O3 = new G4Material("B2O3", 2.4 * g / cm3, 2, kStateSolid,
                               293 * kelvin, 1 * atmosphere);
    B2O3->AddElement(B1, 2);
    B2O3->AddElement(O, 3);

    auto B2O3_ZnS = new G4Material("B2O3_ZnS", 3.53 * g / cm3, 2, kStateSolid,
                                   293 * kelvin, 1 * atmosphere);
    B2O3_ZnS->AddMaterial(B2O3, 33 * perCent);
    B2O3_ZnS->AddMaterial(ZnS, 67 * perCent);

    auto matTarget = nman->FindOrBuildMaterial("G4_POLYETHYLENE"); // !!!###
    auto matModerate = nman->FindOrBuildMaterial("G4_Pb");         // !!!###

    // Detector(en)
    G4double posXPr[32] = {-9.993 * m,  -7.633 * m, -4.783 * m, -9.883 * m,
                           -7.633 * m,  -4.783 * m, -9.703 * m, -7.383 * m,
                           -4.783 * m,  -7.383 * m, -4.783 * m, -7.383 * m,
                           -4.783 * m,  -9.963 * m, -7.383 * m, -4.783 * m,
                           -10.621 * m, -8.011 * m, -4.783 * m, -2.411 * m,
                           0.409 * m,   2.369 * m,  4.979 * m,  4.979 * m,
                           4.979 * m,   4.979 * m,  4.979 * m,  (5.779 - 0.3) *
    m, 4.979 * m,   2.317 * m,  0.347 * m,  -1.653 * m};

    G4double posYPr[32] = {
        13.902 * m,          13.902 * m,          13.919 * m,
        7.922 * m,           7.922 * m,           8.949 * m,
        5.582 * m,           5.582 * m,           4.399 * m,
        -1.118 * m,          -0.961 * m,          -3.778 * m,
        -4.481 * m,          -9.868 * m,          -9.878 * m,
        -9.691 * m,          (-15.118 + 0.1) * m, (-15.118 + 0.1) * m,
        (-15.118 + 0.1) * m, (-15.118 + 0.1) * m, (-15.118 + 0.1) * m,
        (-15.118 + 0.1) * m, (-15.118 + 0.1) * m, -9.848 * m,
        -4.758 * m,          -1.168 * m,          3.512 * m,
        10.032 * m,          13.919 * m,          13.919 * m,
        13.919 * m,          13.919 * m};

    G4double posYU[72] = {
        (14.32) * m,  (14.32) * m,  (14.32) * m,  (14.32) * m,  (10.745) * m,
        (10.74) * m,  (10.74) * m,  (10.745) * m, (7.16) * m,   (7.16) * m,
        (7.16) * m,   (7.16) * m,   (3.58) * m,   (3.58) * m,   (3.58) * m,
        (3.58) * m,   0. * m,       0. * m,       0. * m,       0. * m,
        (-3.58) * m,  (-3.58) * m,  (-3.58) * m,  (-3.58) * m,  (-7.16) * m,
        (-7.16) * m,  (-7.16) * m,  (-7.16) * m,  (-10.74) * m, (-10.74) * m,
        (-10.74) * m, (-10.74) * m, (-14.32) * m, (-14.32) * m, (-14.32) * m,
        (-14.32) * m, 25.981 * m,   25.981 * m,   25.981 * m,   25.981 * m,
        19.881 * m,   19.881 * m,   19.881 * m,   19.881 * m,   13.639 * m,
        13.639 * m,   13.639 * m,   13.639 * m,   7.539 * m,    7.539 * m,
        7.539 * m,    7.539 * m,    1.439 * m,    1.439 * m,    1.439 * m,
        0.499 * m,    -4.661 * m,   -4.661 * m,   -4.661 * m,   -4.661 * m,
        -10.761 * m,  -10.761 * m,  -10.761 * m,  -10.761 * m,  -16.861 * m,
        -16.861 * m,  -16.861 * m,  -17.131 * m,  -22.961 * m,  -22.961 * m,
        -22.961 * m,  -22.961 * m};

    G4double posXU[72] = {
        7.880 * m,   2.430 * m,   -2.980 * m,  -8.350 * m,  7.880 * m,
        2.430 * m,   -2.980 * m,  -8.350 * m,  7.880 * m,   2.430 * m,
        -2.980 * m,  -8.350 * m,  7.880 * m,   2.430 * m,   -2.980 * m,
        -8.350 * m,  7.880 * m,   2.430 * m,   -2.980 * m,  -8.350 * m,
        7.880 * m,   2.430 * m,   -2.980 * m,  -8.350 * m,  7.880 * m,
        2.430 * m,   -2.980 * m,  -8.350 * m,  7.880 * m,   2.430 * m,
        -2.980 * m,  -8.350 * m,  7.880 * m,   2.430 * m,   -2.980 * m,
        -8.350 * m,  -24.880 * m, -29.290 * m, -33.700 * m, -38.110 * m,
        -24.880 * m, -29.290 * m, -33.700 * m, -38.110 * m, -24.880 * m,
        -29.290 * m, -33.700 * m, -36.570 * m, -24.880 * m, -29.290 * m,
        -33.700 * m, -38.110 * m, -24.880 * m, -29.290 * m, -33.700 * m,
        -38.110 * m, -24.880 * m, -29.290 * m, -33.700 * m, -38.110 * m,
        -24.880 * m, -29.290 * m, -33.700 * m, -38.110 * m, -24.880 * m,
        -29.290 * m, -33.700 * m, -38.110 * m, -24.880 * m, -29.290 * m,
        -33.700 * m, -38.110 * m};

    for (G4int i = 0; i < 32; i++) {
      posXPr[i] = posXPr[i] + 0.345 * m;
      posYPr[i] = posYPr[i] + 0.5 * m;
    }

    for (G4int i = 0; i < 72; i++) {
      posXU[i] = posXU[i] - 1.8 * m;
      posYU[i] = posYU[i] - 0.3 * m;
    }

    for (G4int i = 35; i < 72; i++) {
      posYU[i] = posYU[i] + 13.2 * m;
    }

    G4double posZPr = (9.6 / 2. + 0.005 + 0.2 + 0.0001 / 2. + 0.005 + 0.005) *
                      m;                                     // over the cover
    G4double posZU = (11.8 + 0.1 + 0.0001 / 2. + 0.005) * m; // over the roof

    G4double posZPrBar =
        (9.6 / 2. + 0.005 + 0.2 + 0.005 + 0.285) * m;    // over the cover
    G4double posZUBar = (11.8 + 0.1 + 0.005 + 0.28) * m; // over the roof

    G4double posZPrBarDown =
        (9.6 / 2. + 0.005 + 0.2 + 0.005 + 0.005 / 2.) * m; // over the cover
    G4double posZUBarDown = (11.8 + 0.1 + 0.005 / 2.) * m; // over the roof

    G4double posZPrBarUp =
        (9.6 / 2. + 0.005 + 0.2 + 0.005 + 0.005 + 0.56 + 0.005 / 2.) *
        m; // over the cover
    G4double posZUBarUp =
        (11.8 + 0.1 + 0.005 + 0.56 + 0.005 / 2.) * m; // over the roof

    for (G4int i = 0; i < 32; i++) // PRISMA
    {
      solidDet[i] = new G4Tubs("DetPr", 0., 34. * cm, 0.5 * 0.1 * mm, 0.,
                               2. * M_PI); // 0.1*mm
      logicDet[i] = new G4LogicalVolume(solidDet[i], LiF_ZnS, "DetPr");
      physDet[i] = new G4PVPlacement(
          nullptr, G4ThreeVector(posXPr[i], posYPr[i], posZPr), logicDet[i],
          "DetPr", expHall_log, false, i, fCheckOverlaps);

      solidBar1[i] =
          new G4Tubs("BarrelPr", 36.5 * cm, 37. * cm, 28 * cm, 0., 2. * M_PI);
      logicBar1[i] = new G4LogicalVolume(solidBar1[i], bar_mat, "BarrelPr");
      physBar1[i] = new G4PVPlacement(
          nullptr, G4ThreeVector(posXPr[i], posYPr[i], posZPrBar), logicBar1[i],
          "BarrelPr", expHall_log, false, i, fCheckOverlaps);

      solidBarDown[i] =
          new G4Tubs("BarDown", 0., 37. * cm, 0.5 * 5 * mm, 0., 2. * M_PI); //
      logicBarDown[i] = new G4LogicalVolume(solidBarDown[i], bar_mat,
    "BarDown"); physBarDown[i] = new G4PVPlacement( nullptr,
    G4ThreeVector(posXPr[i], posYPr[i], posZPrBarDown), logicBarDown[i],
    "BarDown", expHall_log, false, i, fCheckOverlaps);

      solidBarUp[i] =
          new G4Tubs("BarUp", 0., 37. * cm, 0.5 * 5 * mm, 0., 2. * M_PI); //
      logicBarUp[i] = new G4LogicalVolume(solidBarUp[i], bar_mat, "BarUp");
      physBarUp[i] = new G4PVPlacement(
          nullptr, G4ThreeVector(posXPr[i], posYPr[i], posZPrBarUp),
          logicBarUp[i], "BarUp", expHall_log, false, i, fCheckOverlaps);
    }

    for (G4int i = 0; i < 72; i++) // URAN
    {
      solidDet2[i] =
          new G4Tubs("DetU", 0., 34. * cm, 0.5 * 0.1 * mm, 0., 2. * M_PI);
      logicDet2[i] = new G4LogicalVolume(solidDet2[i], B2O3_ZnS, "DetU");
      physDet2[i] = new G4PVPlacement(
          nullptr, G4ThreeVector(posXU[i], posYU[i], posZU), logicDet2[i],
    "DetU", expHall_log, false, i, fCheckOverlaps);

      solidBar2[i] =
          new G4Tubs("BarrelU", 36.5 * cm, 37. * cm, 28. * cm, 0., 2. * M_PI);
      logicBar2[i] = new G4LogicalVolume(solidBar2[i], bar_mat, "BarrelU");
      physBar2[i] = new G4PVPlacement(
          nullptr, G4ThreeVector(posXU[i], posYU[i], posZUBar), logicBar2[i],
          "BarrelU", expHall_log, false, i, fCheckOverlaps);

      solidBarDown2[i] =
          new G4Tubs("BarDown2", 0., 37. * cm, 2.5 * mm, 0., 2. * M_PI); //
      logicBarDown2[i] =
          new G4LogicalVolume(solidBarDown2[i], bar_mat, "BarDown2");
      physBarDown2[i] = new G4PVPlacement(
          nullptr, G4ThreeVector(posXU[i], posYU[i], posZUBarDown),
          logicBarDown2[i], "BarDown2", expHall_log, false, i, fCheckOverlaps);

      solidBarUp2[i] =
          new G4Tubs("BarUp2", 0., 37. * cm, 2.5 * mm, 0., 2. * M_PI); //
      logicBarUp2[i] = new G4LogicalVolume(solidBarUp2[i], bar_mat, "BarUp2");
      physBarUp2[i] = new G4PVPlacement(
          nullptr, G4ThreeVector(posXU[i], posYU[i], posZUBarUp),
    logicBarUp2[i], "BarUp2", expHall_log, false, i, fCheckOverlaps);
    }
  */
  //===== PRISMA-URAN

  //===== MuTrack

  G4double CtrlNVDSizeX13, CtrlNVDSizeX24, CtrlNVDSizeX56;
  G4double CtrlNVDSizeY13, CtrlNVDSizeY24, CtrlNVDSizeY56;
  G4double CtrlNVDSizeZ13, CtrlNVDSizeZ24, CtrlNVDSizeZ56;
  G4double CtrlNVDPos13, CtrlNVDPos24, CtrlNVDPos56u, CtrlNVDPos56d;

  CtrlNVDSizeX24 = 1.E-6 * m;
  CtrlNVDSizeY24 = 26. * m;
  CtrlNVDSizeZ24 = 8.5 * m;
  CtrlNVDPos24 = (4.5 - 1.E-6 / 2.) * m;

  CtrlNVDSizeX13 = (9. - 2. * 1.E-6) * m;
  CtrlNVDSizeY13 = 1.E-6 * m;
  CtrlNVDSizeZ13 = 8.5 * m;
  CtrlNVDPos13 = (13. - 1.E-6 / 2.) * m;

  CtrlNVDSizeX56 = (9. - 2. * 1.E-6) * m;
  CtrlNVDSizeY56 = (26. - 2. * 1.E-6) * m;
  CtrlNVDSizeZ56 = 1.E-6 * m;
  CtrlNVDPos56u = (4. - 1.E-6 / 2.) * m;
  CtrlNVDPos56d = (-4.5 + 1.E-6 / 2.) * m;

  boxCtrlNVD[0] = new G4Box("CtrlNVD", CtrlNVDSizeX13 / 2., CtrlNVDSizeY13 / 2.,
                            CtrlNVDSizeZ13 / 2.);
  logCtrlNVD[0] = new G4LogicalVolume(boxCtrlNVD[0], Water, "CtrlNVD", nullptr,
                                      nullptr, nullptr);

  physCtrlNVD[0] = new G4PVPlacement(
      nullptr, G4ThreeVector(0, CtrlNVDPos13, -0.25 * m), logCtrlNVD[0],
      "CtrlNVD", water_l, false, 0, fCheckOverlaps);

  boxCtrlNVD[1] = new G4Box("CtrlNVD", CtrlNVDSizeX24 / 2., CtrlNVDSizeY24 / 2.,
                            CtrlNVDSizeZ24 / 2.);
  logCtrlNVD[1] = new G4LogicalVolume(boxCtrlNVD[1], Water, "CtrlNVD", nullptr,
                                      nullptr, nullptr);
  physCtrlNVD[1] = new G4PVPlacement(
      nullptr, G4ThreeVector(CtrlNVDPos24, 0, -0.25 * m), logCtrlNVD[1],
      "CtrlNVD", water_l, false, 1, fCheckOverlaps);

  boxCtrlNVD[2] = new G4Box("CtrlNVD", CtrlNVDSizeX13 / 2., CtrlNVDSizeY13 / 2.,
                            CtrlNVDSizeZ13 / 2.);
  logCtrlNVD[2] = new G4LogicalVolume(boxCtrlNVD[2], Water, "CtrlNVD", nullptr,
                                      nullptr, nullptr);
  physCtrlNVD[2] = new G4PVPlacement(
      nullptr, G4ThreeVector(0, -CtrlNVDPos13, -0.25 * m), logCtrlNVD[2],
      "CtrlNVD", water_l, false, 2, fCheckOverlaps);

  boxCtrlNVD[3] = new G4Box("CtrlNVD", CtrlNVDSizeX24 / 2., CtrlNVDSizeY24 / 2.,
                            CtrlNVDSizeZ24 / 2.);
  logCtrlNVD[3] = new G4LogicalVolume(boxCtrlNVD[3], Water, "CtrlNVD", nullptr,
                                      nullptr, nullptr);
  physCtrlNVD[3] = new G4PVPlacement(
      nullptr, G4ThreeVector(-CtrlNVDPos24, 0, -0.25 * m), logCtrlNVD[3],
      "CtrlNVD", water_l, false, 3, fCheckOverlaps);

  boxCtrlNVD[4] = new G4Box("CtrlNVD", CtrlNVDSizeX56 / 2., CtrlNVDSizeY56 / 2.,
                            CtrlNVDSizeZ56 / 2.);
  logCtrlNVD[4] = new G4LogicalVolume(boxCtrlNVD[4], Water, "CtrlNVD", nullptr,
                                      nullptr, nullptr);
  physCtrlNVD[4] = new G4PVPlacement(
      nullptr, G4ThreeVector(0, 0, CtrlNVDPos56d), logCtrlNVD[4], "CtrlNVD",
      water_l, false, 4, fCheckOverlaps);

  boxCtrlNVD[5] = new G4Box("CtrlNVD", CtrlNVDSizeX56 / 2., CtrlNVDSizeY56 / 2.,
                            CtrlNVDSizeZ56 / 2.);
  logCtrlNVD[5] = new G4LogicalVolume(boxCtrlNVD[5], Water, "CtrlNVD", nullptr,
                                      nullptr, nullptr);
  physCtrlNVD[5] = new G4PVPlacement(
      nullptr, G4ThreeVector(0, 0, CtrlNVDPos56u), logCtrlNVD[5], "CtrlNVD",
      water_l, false, 5, fCheckOverlaps);
  //===== MuTrack

  // Ploskosty FEU
  G4int npl, nstr, nmod, nms[7] = {4, 3, 4, 3, 4, 3, 4}, nkopy, Npl;
  G4int Vect[6][3] = {{0, 1, 0},  {1, 0, 0},  {0, -1, 0},
                      {-1, 0, 0}, {0, 0, -1}, {0, 0, 1}};
  G4double coord_x, coord_y, coord_z, rasst;

  G4double feu_x = 120.0 * mm;
  G4double feu_y = 120.0 * mm;
  G4double feu_z = 120.0 * mm;
  G4double startAngle = 0. * deg;
  G4double spanningAngle = 360. * deg;
  G4double innerRadGlass = 0.0 * mm;
  G4double outerRadGlass = 7.5 * cm;
  G4double hightGlass = 6.0 / 2.0 * mm;
  G4double innerRadPhotokat = 0.0 * mm;
  G4double outerRadPhotokat = 150.0 / 2.0 * mm;
  G4double hightPhotokat = 0.1 / 2.0 * mm;

  G4RotationMatrix *rm[6];
  G4ThreeVector position;

  for (auto &i : rm)
    i = new G4RotationMatrix();

  rm[0]->rotateX(90.0 * deg);
  rm[1]->rotateY(270.0 * deg);
  rm[2]->rotateX(270.0 * deg);
  rm[3]->rotateY(90.0 * deg);
  rm[4]->rotateY(180.0 * deg);
  rm[5]->rotateX(0.0 * deg);

  auto Mbox_g = new G4Box("MBox", feu_x, feu_y, feu_z);
  auto MboxA_g =
      new G4Box("MBoxA", feu_x - 5. * mm, feu_y - 5. * mm, feu_z - 5. * mm);

  auto AlTube_g = new G4Tubs("Tube", 0. * mm, 102.5 * mm, 157. / 2. * mm,
                             startAngle, spanningAngle); // ???

  auto AirTube_g =
      new G4Tubs("AirTube", 0. * mm, 97.5 * mm, 157. / 2. * mm, startAngle,
                 spanningAngle); // ??? innerRadius, outerRadius, hz

  auto Illum1_g = new G4Tubs("Illum1", 0. * mm, 102.5 * mm, 8. / 2. * mm,
                             startAngle, spanningAngle); // a) 1/2 v vode

  auto Illum2_g = new G4Tubs("Illum2", 0. * mm, 97.5 * mm, 16. / 2. * mm,
                             startAngle, spanningAngle); // b) 1 v trube

  auto Silic_g = new G4Tubs("Silic", innerRadGlass, outerRadGlass, 3. / 2. * mm,
                            startAngle, spanningAngle);

  auto Glass_g = new G4Tubs("Glass", innerRadGlass, outerRadGlass, hightGlass,
                            startAngle, spanningAngle);

  auto Photokat_g = new G4Tubs("Photokat", innerRadPhotokat, outerRadPhotokat,
                               hightPhotokat, startAngle, spanningAngle);

  G4LogicalVolume *Mbox_log[7][4][4] = {nullptr},
                  *MboxA_log[7][4][4] = {nullptr},
                  *AlTube_log[7][4][4][6] = {nullptr},
                  *AirTube_l[7][4][4][6] = {nullptr},
                  *Illum1_log[7][4][4][6] = {nullptr},
                  *Illum2_log[7][4][4][6] = {nullptr},
                  *Silic_log[7][4][4][6] = {nullptr},
                  *Glass_log[7][4][4][6] = {nullptr},
                  *Photokat_log[7][4][4][6] = {nullptr};

  G4VPhysicalVolume *Mbox_phys[7][4][4] = {nullptr},
                    *MboxA_phys[7][4][4] = {nullptr},
                    *AlTube_phys[7][4][4][6] = {nullptr},
                    *AirTube_phys[7][4][4][6] = {nullptr},
                    *Illum1_phys[7][4][4][6] = {nullptr},
                    *Illum2_phys[7][4][4][6] = {nullptr},
                    *Silic_phys[7][4][4][6] = {nullptr},
                    *Glass_phys[7][4][4][6] = {nullptr},
                    *Photokat_phys[7][4][4][6] = {nullptr};

  nkopy = 0;
  Npl = 7;

  for (npl = 0; npl < Npl; npl++) {
    for (nstr = 0; nstr < nms[npl]; nstr++) {
      for (nmod = 0; nmod < nms[npl]; nmod++) {
        coord_x = 0.0;
        coord_y = 0.0;
        coord_z = 0.0;
        if ((npl == 0) || (npl == 2) || (npl == 4) ||
            (npl == 6)) { // dla chetverok
          coord_x = -3.0 + 2.0 * nstr;
          coord_z = 3.0 - 2.0 * nmod;
        } else { // dla troek
          coord_x = -2.0 + 2.0 * nstr;
          coord_z = 2.0 - 2.0 * nmod;
        }
        coord_y = -6.125 + 1.25 * npl;
        position = G4ThreeVector(coord_x * m, coord_y * m, coord_z * m);

        Mbox_log[npl][nstr][nmod] =
            new G4LogicalVolume(Mbox_g, mAl, "MBox", nullptr, nullptr, nullptr);

        Mbox_phys[npl][nstr][nmod] =
            new G4PVPlacement(nullptr, position, Mbox_log[npl][nstr][nmod],
                              "MBox", water_l, false, 0, fCheckOverlaps);

        MboxA_log[npl][nstr][nmod] = new G4LogicalVolume(
            MboxA_g, Air, "MBoxA", nullptr, nullptr, nullptr);

        MboxA_phys[npl][nstr][nmod] = new G4PVPlacement(
            nullptr, G4ThreeVector(0 * m, 0 * m, 0 * m),
            MboxA_log[npl][nstr][nmod], "MBoxA", Mbox_log[npl][nstr][nmod],
            false, 0, fCheckOverlaps);

        for (G4int i = 0; i < 6; i++) {
          rasst = (120. + 157. / 2.) * mm; //
          position = G4ThreeVector(rasst * Vect[i][0] + coord_x * m,
                                   rasst * Vect[i][1] + coord_y * m,
                                   rasst * Vect[i][2] + coord_z * m);
          AlTube_log[npl][nstr][nmod][i] = new G4LogicalVolume(
              AlTube_g, mAl, "Tube", nullptr, nullptr, nullptr);

          AlTube_phys[npl][nstr][nmod][i] =
              new G4PVPlacement(rm[i], position, AlTube_log[npl][nstr][nmod][i],
                                "Tube", water_l, false, 0, fCheckOverlaps);

          AirTube_l[npl][nstr][nmod][i] = new G4LogicalVolume(
              AirTube_g, Air, "AirTube", nullptr, nullptr, nullptr); //

          AirTube_phys[npl][nstr][nmod][i] = new G4PVPlacement(
              nullptr, G4ThreeVector(0. * mm, 0. * mm, 0. * mm), //
              AirTube_l[npl][nstr][nmod][i], "AirTube",
              AlTube_log[npl][nstr][nmod][i], false, 0, fCheckOverlaps); //

          rasst = (120. + 157. + 8. / 2.) * mm; //
          position = G4ThreeVector(rasst * Vect[i][0] + coord_x * m,
                                   rasst * Vect[i][1] + coord_y * m,
                                   rasst * Vect[i][2] + coord_z * m); // a)

          position =
              G4ThreeVector(0. * m, 0. * m, (157. / 2. - 16. / 2.) * mm); // b)
          Illum2_log[npl][nstr][nmod][i] = new G4LogicalVolume(
              Illum2_g, Plexiglass, "Illum2", nullptr, nullptr, nullptr); //
          Illum2_phys[npl][nstr][nmod][i] = new G4PVPlacement(
              nullptr, position, Illum2_log[npl][nstr][nmod][i], "Illum2", //

              AirTube_l[npl][nstr][nmod][i], false, 0, fCheckOverlaps); //

          Silic_log[npl][nstr][nmod][i] = new G4LogicalVolume(
              Silic_g, Silicone, "Silic", nullptr, nullptr, nullptr); //

          Silic_phys[npl][nstr][nmod][i] = new G4PVPlacement(
              nullptr,
              G4ThreeVector(0. * mm, 0. * mm,
                            (157. / 2. - 16. - 3. / 2.) * mm), // b)
              Silic_log[npl][nstr][nmod][i], "Silic",
              AirTube_l[npl][nstr][nmod][i], false, 0, fCheckOverlaps); //

          Glass_log[npl][nstr][nmod][i] = new G4LogicalVolume(
              Glass_g, Glass, "Glass", nullptr, nullptr, nullptr); //

          Glass_phys[npl][nstr][nmod][i] = new G4PVPlacement(
              nullptr,
              G4ThreeVector(0. * mm, 0. * mm,
                            (157. / 2. - 16. - 3. - 6. / 2.) * mm), // b)

              Glass_log[npl][nstr][nmod][i], "Glass",
              AirTube_l[npl][nstr][nmod][i], false, 0, fCheckOverlaps); //

          Photokat_log[npl][nstr][nmod][i] = new G4LogicalVolume(
              Photokat_g, mAl, "Photokat", nullptr, nullptr, nullptr); //

          Photokat_phys[npl][nstr][nmod][i] = new G4PVPlacement(
              nullptr,
              G4ThreeVector(0. * mm, 0. * mm,
                            (157. / 2. - 16. - 3. - 6. - 0.1 / 2.) * mm), // b)

              Photokat_log[npl][nstr][nmod][i], "Photokat",
              AirTube_l[npl][nstr][nmod][i], false, nkopy, fCheckOverlaps); //

          perevKM[nkopy][0] = npl;
          perevKM[nkopy][1] = nstr;
          perevKM[nkopy][2] = nmod;
          perevKM[nkopy][3] = i;

          nkopy++;
        }
      }
    }
  }
  feuCount = nkopy;

  // --------------- SKT ---------------

  G4double CntSizeX = 0.23 * m;
  G4double CntSizeY = 0.63 * m;
  G4double CntSizeZ = 0.055 * m;
  G4double ScintSizeX = 0.20 * m;
  G4double ScintSizeY = 0.40 * m;
  G4double ScintSizeZ = 0.020 * m;
  G4double CntPosX = 0. * m;
  G4double CntPosY = 0. * m;
  G4double Cnt1PosZ = (9.6 / 2. + 0.005 + 0.055 / 2.) * m; //
  G4double Cnt2PosZ = (-4.5 + 0.055 / 2. + 1.E-6) * m;     // CtrlNVD

  G4int ncnt;

  ncnt = 0;

  // top
  for (int j = 0; j < 5; j++) {
    ncnt = 9 * j;
    for (int i = 0; i < 4; i++) {
      CntPosX = (-4.5 + 1.5 + 2. * i) * m;
      CntPosY = (-13.0 + 5.625 + 2.5 * j) * m;

      boxCount[ncnt] =
          new G4Box("boxCount", CntSizeX / 2., CntSizeY / 2., CntSizeZ / 2.);
      logCount[ncnt] = new G4LogicalVolume(boxCount[ncnt], mAl,
                                           "logCount"); // solid, material, name
      physCount[ncnt] = new G4PVPlacement(
          nullptr,                                   // no rotation
          G4ThreeVector(CntPosX, CntPosY, Cnt1PosZ), // position
          logCount[ncnt],                            // logical volume
          "physCount",                               // name
          expHall_log,                               // mother volume
          false,                                     // no boolean operations
          ncnt,                                      // copy number
          fCheckOverlaps);                           // overlaps checking

      boxCountA[ncnt] =
          new G4Box("boxCountA", 0.22 / 2. * m, 0.62 / 2. * m, 0.045 / 2. * m);
      logCountA[ncnt] = new G4LogicalVolume(
          boxCountA[ncnt], Air, "logCountA"); // solid, material, name
      physCountA[ncnt] = new G4PVPlacement(
          nullptr,                                  // no rotation
          G4ThreeVector(0.0 * m, 0.0 * m, 0.0 * m), // position
          logCountA[ncnt],                          // logical volume
          "physCountA",                             // name
          logCount[ncnt],                           // mother volume
          false,                                    // no boolean operations
          ncnt,                                     // copy number
          fCheckOverlaps);                          // overlaps checking

      boxScint[ncnt] = new G4Box("boxScint", ScintSizeX / 2., ScintSizeY / 2.,
                                 ScintSizeZ / 2.);
      logScint[ncnt] = new G4LogicalVolume(boxScint[ncnt], Scintillator,
                                           "logScint"); // solid, material, name
      physScint[ncnt] = new G4PVPlacement(
          nullptr,                                  // no rotation
          G4ThreeVector(0.0 * m, 0.0 * m, 0.0 * m), // position
          logScint[ncnt],                           // logical volume
          "physScint",                              // name
          logCountA[ncnt],                          // mother volume
          false,                                    // no boolean operations
          ncnt,                                     // copy number
          fCheckOverlaps);                          // overlaps checking

      ncnt = ncnt + 1;
    }
  }

  for (int j = 0; j < 4; j++) {
    ncnt = 4 + 9 * j;
    for (int i = 0; i < 5; i++) {
      CntPosX = (-4.5 + 0.5 + 2. * i) * m;
      CntPosY = (-13.0 + 6.875 + 2.5 * j) * m;

      boxCount[ncnt] =
          new G4Box("boxCount", CntSizeX / 2., CntSizeY / 2., CntSizeZ / 2.);
      logCount[ncnt] = new G4LogicalVolume(boxCount[ncnt], mAl,
                                           "logCount"); // solid, material, name
      physCount[ncnt] = new G4PVPlacement(
          nullptr,                                   // no rotation
          G4ThreeVector(CntPosX, CntPosY, Cnt1PosZ), // position
          logCount[ncnt],                            // logical volume
          "physCount",                               // name
          expHall_log,                               // mother volume
          false,                                     // no boolean operations
          ncnt,                                      // copy number
          fCheckOverlaps);                           // overlaps checking

      boxCountA[ncnt] =
          new G4Box("boxCountA", 0.22 / 2. * m, 0.62 / 2. * m, 0.045 / 2. * m);
      logCountA[ncnt] = new G4LogicalVolume(
          boxCountA[ncnt], Air, "logCountA"); // solid, material, name
      physCountA[ncnt] = new G4PVPlacement(
          nullptr,                                  // no rotation
          G4ThreeVector(0.0 * m, 0.0 * m, 0.0 * m), // position
          logCountA[ncnt],                          // logical volume
          "physCountA",                             // name
          logCount[ncnt],                           // mother volume
          false,                                    // no boolean operations
          ncnt,                                     // copy number
          fCheckOverlaps);                          // overlaps checking

      boxScint[ncnt] = new G4Box("boxScint", ScintSizeX / 2., ScintSizeY / 2.,
                                 ScintSizeZ / 2.);
      logScint[ncnt] = new G4LogicalVolume(boxScint[ncnt], Scintillator,
                                           "logScint"); // solid, material, name
      physScint[ncnt] = new G4PVPlacement(
          nullptr,                                  // no rotation
          G4ThreeVector(0.0 * m, 0.0 * m, 0.0 * m), // position
          logScint[ncnt],                           // logical volume
          "physScint",                              // name
          logCountA[ncnt],                          // mother volume
          false,                                    // no boolean operations
          ncnt,                                     // copy number
          fCheckOverlaps);                          // overlaps checking

      ncnt = ncnt + 1;
    }
  }

  // bottom
  for (int j = 0; j < 5; j++) {
    ncnt = 9 * j + 40;
    for (int i = 0; i < 4; i++) {
      CntPosX = (-4.5 + 1.5 + 2. * i) * m;
      CntPosY = (-13.0 + 5.625 + 2.5 * j) * m;

      boxCount[ncnt] =
          new G4Box("boxCount", CntSizeX / 2., CntSizeY / 2., CntSizeZ / 2.);
      logCount[ncnt] = new G4LogicalVolume(boxCount[ncnt], mAl,
                                           "logCount"); // solid, material, name
      physCount[ncnt] = new G4PVPlacement(
          nullptr,                                   // no rotation
          G4ThreeVector(CntPosX, CntPosY, Cnt2PosZ), // position
          logCount[ncnt],                            // logical volume
          "physCount",                               // name
          water_l,                                   // mother volume
          false,                                     // no boolean operations
          ncnt,                                      // copy number
          fCheckOverlaps);                           // overlaps checking

      boxCountA[ncnt] =
          new G4Box("boxCountA", 0.22 / 2. * m, 0.62 / 2. * m, 0.045 / 2. * m);
      logCountA[ncnt] = new G4LogicalVolume(
          boxCountA[ncnt], Air, "logCountA"); // solid, material, name
      physCountA[ncnt] = new G4PVPlacement(
          nullptr,                                  // no rotation
          G4ThreeVector(0.0 * m, 0.0 * m, 0.0 * m), // position
          logCountA[ncnt],                          // logical volume
          "physCountA",                             // name
          logCount[ncnt],                           // mother volume
          false,                                    // no boolean operations
          ncnt,                                     // copy number
          fCheckOverlaps);                          // overlaps checking

      boxScint[ncnt] = new G4Box("boxScint", ScintSizeX / 2., ScintSizeY / 2.,
                                 ScintSizeZ / 2.);
      logScint[ncnt] = new G4LogicalVolume(boxScint[ncnt], Scintillator,
                                           "logScint"); // solid, material, name
      physScint[ncnt] = new G4PVPlacement(
          nullptr,                                  // no rotation
          G4ThreeVector(0.0 * m, 0.0 * m, 0.0 * m), // position
          logScint[ncnt],                           // logical volume
          "physScint",                              // name
          logCountA[ncnt],                          // mother volume
          false,                                    // no boolean operations
          ncnt,                                     // copy number
          fCheckOverlaps);                          // overlaps checking

      ncnt = ncnt + 1;
    }
  }

  for (int j = 0; j < 4; j++) {
    ncnt = 4 + 9 * j + 40;
    for (int i = 0; i < 5; i++) {
      CntPosX = (-4.5 + 0.5 + 2. * i) * m;
      CntPosY = (-13.0 + 6.875 + 2.5 * j) * m;

      boxCount[ncnt] =
          new G4Box("boxCount", CntSizeX / 2., CntSizeY / 2., CntSizeZ / 2.);
      logCount[ncnt] = new G4LogicalVolume(boxCount[ncnt], mAl,
                                           "logCount"); // solid, material, name
      physCount[ncnt] = new G4PVPlacement(
          nullptr,                                   // no rotation
          G4ThreeVector(CntPosX, CntPosY, Cnt2PosZ), // position
          logCount[ncnt],                            // logical volume
          "physCount",                               // name
          water_l,                                   // mother volume
          false,                                     // no boolean operations
          ncnt,                                      // copy number
          fCheckOverlaps);                           // overlaps checking

      boxCountA[ncnt] =
          new G4Box("boxCountA", 0.22 / 2. * m, 0.62 / 2. * m, 0.045 / 2. * m);
      logCountA[ncnt] = new G4LogicalVolume(
          boxCountA[ncnt], Air, "logCountA"); // solid, material, name
      physCountA[ncnt] = new G4PVPlacement(
          nullptr,                                  // no rotation
          G4ThreeVector(0.0 * m, 0.0 * m, 0.0 * m), // position
          logCountA[ncnt],                          // logical volume
          "physCountA",                             // name
          logCount[ncnt],                           // mother volume
          false,                                    // no boolean operations
          ncnt,                                     // copy number
          fCheckOverlaps);                          // overlaps checking

      boxScint[ncnt] = new G4Box("boxScint", ScintSizeX / 2., ScintSizeY / 2.,
                                 ScintSizeZ / 2.);
      logScint[ncnt] = new G4LogicalVolume(boxScint[ncnt], Scintillator,
                                           "logScint"); // solid, material, name
      physScint[ncnt] = new G4PVPlacement(
          nullptr,                                  // no rotation
          G4ThreeVector(0.0 * m, 0.0 * m, 0.0 * m), // position
          logScint[ncnt],                           // logical volume
          "physScint",                              // name
          logCountA[ncnt],                          // mother volume
          false,                                    // no boolean operations
          ncnt,                                     // copy number
          fCheckOverlaps);                          // overlaps checking

      ncnt = ncnt + 1;
    }
  }

  // --------------- DECOR ---------------

  G4double shSMSizeX[2];
  shSMSizeX[0] = 3430 * mm;
  shSMSizeX[1] = 3120 * mm;
  G4double shSMSizeZ = 2708 * mm;
  G4double shSMSizeY = 1 * nm;

  G4double lgSMSizeY[2];
  lgSMSizeY[0] = 3430 * mm;
  lgSMSizeY[1] = 3120 * mm;
  G4double lgSMSizeZ = 2708 * mm;
  G4double lgSMSizeX = 1 * nm;

  G4double SM0PosX, SM0PosY, SM0PosZ;
  G4double SM1PosX, SM1PosY, SM1PosZ;
  G4double SM2PosX, SM2PosY, SM2PosZ;
  G4double SM3PosX, SM3PosY, SM3PosZ;
  G4double SM4PosX, SM4PosY, SM4PosZ;
  G4double SM5PosX, SM5PosY, SM5PosZ;
  G4double SM6PosX, SM6PosY, SM6PosZ;
  G4double SM7PosX, SM7PosY, SM7PosZ;
  G4double smeSMx, smeSMy, smeSMz;
  G4double xSM0PosY1, xSM1PosY1, xSM6PosY1, xSM7PosY1;
  G4double xSM2PosX1, xSM3PosX1, xSM4PosX1, xSM5PosX1;
  G4double ySM0PosY1, ySM1PosY1, ySM6PosY1, ySM7PosY1;
  G4double ySM2PosX1, ySM3PosX1, ySM4PosX1, ySM5PosX1;

  smeSMx = 4.5 * 1000.;
  smeSMy = 13. * 1000.;
  smeSMz = (4.5 - 0.3) * 1000.; // pravilno

  SM0PosX = (2310.38 - smeSMx) * mm;
  SM0PosY = (-1011 - smeSMy) * mm;
  SM0PosZ = (3926.62 - smeSMz) * mm;
  SM1PosX = (6727.38 - smeSMx) * mm;
  SM1PosY = (-1011 - smeSMy) * mm;
  SM1PosZ = (3923.62 - smeSMz) * mm;
  SM2PosX = (9960 - smeSMx) * mm;
  SM2PosY = (3002.38 - smeSMy) * mm;
  SM2PosZ = (3920.62 - smeSMz) * mm;
  SM3PosX = (9961 - smeSMx) * mm;
  SM3PosY = (8115.38 - smeSMy) * mm;
  SM3PosZ = (3916.62 - smeSMz) * mm;
  SM4PosX = (9958 - smeSMx) * mm;
  SM4PosY = (13243.4 - smeSMy) * mm;
  SM4PosZ = (3914.62 - smeSMz) * mm;
  SM5PosX = (9960 - smeSMx) * mm;
  SM5PosY = (18362.4 - smeSMy) * mm;
  SM5PosZ = (3923.62 - smeSMz) * mm;
  SM6PosX = (6748.62 - smeSMx) * mm;
  SM6PosY = (27022 - smeSMy) * mm;
  SM6PosZ = (3916.62 - smeSMz) * mm;
  SM7PosX = (2348.62 - smeSMx) * mm;
  SM7PosY = (27022 - smeSMy) * mm;
  SM7PosZ = (3919.62 - smeSMz) * mm;

  for (int nplane = 0; nplane < 8; nplane++) {
    // X strips
    xSM0PosY1 = SM0PosY - 3. * cm - 3. * 6. * cm + nplane * 6. * cm -
                1. * nm; // gap = 60 mm
    boxSM0x[nplane] =
        new G4Box("SM0x", shSMSizeX[0] / 2., shSMSizeY / 2., shSMSizeZ / 2.);
    logSM0x[nplane] = new G4LogicalVolume(boxSM0x[nplane], mAl, "SM0x", nullptr,
                                          nullptr, nullptr);
    physSM0x[nplane] = new G4PVPlacement(
        nullptr, G4ThreeVector(SM0PosX, xSM0PosY1, SM0PosZ), logSM0x[nplane],
        "SM0x", expHall_log, false, nplane, fCheckOverlaps);

    xSM1PosY1 = SM1PosY - 3. * cm - 3. * 6. * cm + nplane * 6. * cm - 1. * nm;
    boxSM1x[nplane] =
        new G4Box("SM1x", shSMSizeX[0] / 2., shSMSizeY / 2., shSMSizeZ / 2.);
    logSM1x[nplane] = new G4LogicalVolume(boxSM1x[nplane], mAl, "SM1x", nullptr,
                                          nullptr, nullptr);
    physSM1x[nplane] = new G4PVPlacement(
        nullptr, G4ThreeVector(SM1PosX, xSM1PosY1, SM1PosZ), logSM1x[nplane],
        "SM1x", expHall_log, false, nplane, fCheckOverlaps);

    xSM2PosX1 = SM2PosX - 3. * cm - 3. * 6. * cm + nplane * 6. * cm - 1. * nm;
    boxSM2x[nplane] =
        new G4Box("SM2x", lgSMSizeX / 2., lgSMSizeY[0] / 2., lgSMSizeZ / 2.);
    logSM2x[nplane] = new G4LogicalVolume(boxSM2x[nplane], mAl, "SM2x", nullptr,
                                          nullptr, nullptr);
    physSM2x[nplane] = new G4PVPlacement(
        nullptr, G4ThreeVector(xSM2PosX1, SM2PosY, SM2PosZ), logSM2x[nplane],
        "SM2x", expHall_log, false, nplane, fCheckOverlaps);

    xSM3PosX1 = SM3PosX - 3. * cm - 3. * 6. * cm + nplane * 6. * cm - 1. * nm;
    boxSM3x[nplane] =
        new G4Box("SM3x", lgSMSizeX / 2., lgSMSizeY[0] / 2., lgSMSizeZ / 2.);
    logSM3x[nplane] = new G4LogicalVolume(boxSM3x[nplane], mAl, "SM3x", nullptr,
                                          nullptr, nullptr);
    physSM3x[nplane] = new G4PVPlacement(
        nullptr, G4ThreeVector(xSM3PosX1, SM3PosY, SM3PosZ), logSM3x[nplane],
        "SM3x", expHall_log, false, nplane, fCheckOverlaps);

    xSM4PosX1 = SM4PosX - 3. * cm - 3. * 6. * cm + nplane * 6. * cm - 1. * nm;
    boxSM4x[nplane] =
        new G4Box("SM4x", lgSMSizeX / 2., lgSMSizeY[0] / 2., lgSMSizeZ / 2.);
    logSM4x[nplane] = new G4LogicalVolume(boxSM4x[nplane], mAl, "SM4x", nullptr,
                                          nullptr, nullptr);
    physSM4x[nplane] = new G4PVPlacement(
        nullptr, G4ThreeVector(xSM4PosX1, SM4PosY, SM4PosZ), logSM4x[nplane],
        "SM4x", expHall_log, false, nplane, fCheckOverlaps);

    xSM5PosX1 = SM5PosX - 3. * cm - 3. * 6. * cm + nplane * 6. * cm - 1. * nm;
    boxSM5x[nplane] =
        new G4Box("SM5x", lgSMSizeX / 2., lgSMSizeY[0] / 2., lgSMSizeZ / 2.);
    logSM5x[nplane] = new G4LogicalVolume(boxSM5x[nplane], mAl, "SM5x", nullptr,
                                          nullptr, nullptr);
    physSM5x[nplane] = new G4PVPlacement(
        nullptr, G4ThreeVector(xSM5PosX1, SM5PosY, SM5PosZ), logSM5x[nplane],
        "SM5x", expHall_log, false, nplane, fCheckOverlaps);

    xSM6PosY1 = SM6PosY - 3. * cm - 3. * 6. * cm + nplane * 6. * cm - 1. * nm;
    boxSM6x[nplane] =
        new G4Box("SM6x", shSMSizeX[0] / 2., shSMSizeY / 2., shSMSizeZ / 2.);
    logSM6x[nplane] = new G4LogicalVolume(boxSM6x[nplane], mAl, "SM6x", nullptr,
                                          nullptr, nullptr);
    physSM6x[nplane] = new G4PVPlacement(
        nullptr, G4ThreeVector(SM6PosX, xSM6PosY1, SM6PosZ), logSM6x[nplane],
        "SM6x", expHall_log, false, nplane, fCheckOverlaps);

    xSM7PosY1 = SM7PosY - 3. * cm - 3. * 6. * cm + nplane * 6. * cm - 1. * nm;
    boxSM7x[nplane] =
        new G4Box("SM7x", shSMSizeX[0] / 2., shSMSizeY / 2., shSMSizeZ / 2.);
    logSM7x[nplane] = new G4LogicalVolume(boxSM7x[nplane], mAl, "SM7x", nullptr,
                                          nullptr, nullptr);
    physSM7x[nplane] = new G4PVPlacement(
        nullptr, G4ThreeVector(SM7PosX, xSM7PosY1, SM7PosZ), logSM7x[nplane],
        "SM7x", expHall_log, false, nplane, fCheckOverlaps);

    // Y strips
    ySM0PosY1 = SM0PosY - 3. * cm - 3. * 6. * cm + nplane * 6. * cm +
                1. * nm; // gap = 60 mm
    boxSM0y[nplane] =
        new G4Box("SM0y", shSMSizeX[1] / 2., shSMSizeY / 2., shSMSizeZ / 2.);
    logSM0y[nplane] = new G4LogicalVolume(boxSM0y[nplane], mAl, "SM0y", nullptr,
                                          nullptr, nullptr);
    physSM0y[nplane] = new G4PVPlacement(
        nullptr, G4ThreeVector(SM0PosX, ySM0PosY1, SM0PosZ), logSM0y[nplane],
        "SM0y", expHall_log, false, nplane, fCheckOverlaps);

    ySM1PosY1 = SM1PosY - 3. * cm - 3. * 6. * cm + nplane * 6. * cm + 1. * nm;
    boxSM1y[nplane] =
        new G4Box("SM1y", shSMSizeX[1] / 2., shSMSizeY / 2., shSMSizeZ / 2.);
    logSM1y[nplane] = new G4LogicalVolume(boxSM1y[nplane], mAl, "SM1y", nullptr,
                                          nullptr, nullptr);
    physSM1y[nplane] = new G4PVPlacement(
        nullptr, G4ThreeVector(SM1PosX, ySM1PosY1, SM1PosZ), logSM1y[nplane],
        "SM1y", expHall_log, false, nplane, fCheckOverlaps);

    ySM2PosX1 = SM2PosX - 3. * cm - 3. * 6. * cm + nplane * 6. * cm + 1. * nm;
    boxSM2y[nplane] =
        new G4Box("SM2y", lgSMSizeX / 2., lgSMSizeY[1] / 2., lgSMSizeZ / 2.);
    logSM2y[nplane] = new G4LogicalVolume(boxSM2y[nplane], mAl, "SM2y", nullptr,
                                          nullptr, nullptr);
    physSM2y[nplane] = new G4PVPlacement(
        nullptr, G4ThreeVector(ySM2PosX1, SM2PosY, SM2PosZ), logSM2y[nplane],
        "SM2y", expHall_log, false, nplane, fCheckOverlaps);

    ySM3PosX1 = SM3PosX - 3. * cm - 3. * 6. * cm + nplane * 6. * cm + 1. * nm;
    boxSM3y[nplane] =
        new G4Box("SM3y", lgSMSizeX / 2., lgSMSizeY[1] / 2., lgSMSizeZ / 2.);
    logSM3y[nplane] = new G4LogicalVolume(boxSM3y[nplane], mAl, "SM3y", nullptr,
                                          nullptr, nullptr);
    physSM3y[nplane] = new G4PVPlacement(
        nullptr, G4ThreeVector(ySM3PosX1, SM3PosY, SM3PosZ), logSM3y[nplane],
        "SM3y", expHall_log, false, nplane, fCheckOverlaps);

    ySM4PosX1 = SM4PosX - 3. * cm - 3. * 6. * cm + nplane * 6. * cm + 1. * nm;
    boxSM4y[nplane] =
        new G4Box("SM4y", lgSMSizeX / 2., lgSMSizeY[1] / 2., lgSMSizeZ / 2.);
    logSM4y[nplane] = new G4LogicalVolume(boxSM4y[nplane], mAl, "SM4y", nullptr,
                                          nullptr, nullptr);
    physSM4y[nplane] = new G4PVPlacement(
        nullptr, G4ThreeVector(ySM4PosX1, SM4PosY, SM4PosZ), logSM4y[nplane],
        "SM4y", expHall_log, false, nplane, fCheckOverlaps);

    ySM5PosX1 = SM5PosX - 3. * cm - 3. * 6. * cm + nplane * 6. * cm + 1. * nm;
    boxSM5y[nplane] =
        new G4Box("SM5y", lgSMSizeX / 2., lgSMSizeY[1] / 2., lgSMSizeZ / 2.);
    logSM5y[nplane] = new G4LogicalVolume(boxSM5y[nplane], mAl, "SM5y", nullptr,
                                          nullptr, nullptr);
    physSM5y[nplane] = new G4PVPlacement(
        nullptr, G4ThreeVector(ySM5PosX1, SM5PosY, SM5PosZ), logSM5y[nplane],
        "SM5y", expHall_log, false, nplane, fCheckOverlaps);

    ySM6PosY1 = SM6PosY - 3. * cm - 3. * 6. * cm + nplane * 6. * cm + 1. * nm;
    boxSM6y[nplane] =
        new G4Box("SM6y", shSMSizeX[1] / 2., shSMSizeY / 2., shSMSizeZ / 2.);
    logSM6y[nplane] = new G4LogicalVolume(boxSM6y[nplane], mAl, "SM6y", nullptr,
                                          nullptr, nullptr);
    physSM6y[nplane] = new G4PVPlacement(
        nullptr, G4ThreeVector(SM6PosX, ySM6PosY1, SM6PosZ), logSM6y[nplane],
        "SM6y", expHall_log, false, nplane, fCheckOverlaps);

    ySM7PosY1 = SM7PosY - 3. * cm - 3. * 6. * cm + nplane * 6. * cm + 1. * nm;
    boxSM7y[nplane] =
        new G4Box("SM7y", shSMSizeX[1] / 2., shSMSizeY / 2., shSMSizeZ / 2.);
    logSM7y[nplane] = new G4LogicalVolume(boxSM7y[nplane], mAl, "SM7y", nullptr,
                                          nullptr, nullptr);
    physSM7y[nplane] = new G4PVPlacement(
        nullptr, G4ThreeVector(SM7PosX, ySM7PosY1, SM7PosZ), logSM7y[nplane],
        "SM7y", expHall_log, false, nplane, fCheckOverlaps);
  }

  // --------------- NEVOD-EAS ---------------

  G4double Build47sizeX, Build47sizeY, Build47sizeZ;
  G4double Build47posX, Build47posY, Build47posZ;
  Build47sizeX = 16.;
  Build47sizeY = 52.;
  Build47sizeZ = 10.28 + 0.79;
  Build47posX = -32. - 4.45 / 2.;
  Build47posY = 24.5 + 30. / 2. - 52. / 2.;
  Build47posZ = 11.8 - 6.68 - Build47sizeZ / 2.;

  auto boxBuild47 = new G4Box("boxBuild47", Build47sizeX / 2. * m,
                              Build47sizeY / 2. * m, Build47sizeZ / 2. * m);
  auto logBuild47 = new G4LogicalVolume(boxBuild47, Concrete,
                                        "logBuild47"); // solid, material, name
  G4VPhysicalVolume *physBuild47 =
      new G4PVPlacement(nullptr, // no rotation
                        G4ThreeVector(Build47posX * m, Build47posY * m,
                                      Build47posZ * m), // position
                        logBuild47,                     // logical volume
                        "physBuild47",                  // name
                        expHall_log,                    // mother volume
                        false,                          // no boolean operations
                        0,                              // copy number
                        fCheckOverlaps);                // overlaps checking

  auto boxBuildAir47 =
      new G4Box("boxBuildAir47", (Build47sizeX - 1.) / 2. * m,
                (Build47sizeY - 1.) / 2. * m, (Build47sizeZ - 1.) / 2. * m);
  auto logBuildAir47 = new G4LogicalVolume(
      boxBuildAir47, Air, "logBuildAir47"); // solid, material, name
  G4VPhysicalVolume *physBuildAir47 =
      new G4PVPlacement(nullptr,                // no rotation
                        G4ThreeVector(0, 0, 0), // position
                        logBuildAir47,          // logical volume
                        "physBuildAir47",       // name
                        logBuild47,             // mother volume
                        false,                  // no boolean operations
                        0,                      // copy number
                        fCheckOverlaps);        // overlaps checking

  G4double Build31sizeX, Build31sizeY, Build31sizeZ;
  G4double Build31posX, Build31posY, Build31posZ;
  Build31sizeX = 16.;
  Build31sizeY = 65.;
  Build31sizeZ = 10.28 + 0.79;
  Build31posX = -32. - 4.45 / 2.;
  Build31posY = (-65. / 2. + 30. / 2.) - 27.5;
  Build31posZ = 11.8 - 7.37 - Build31sizeZ / 2.;

  auto boxBuild31 = new G4Box("boxBuild31", Build31sizeX / 2. * m,
                              Build31sizeY / 2. * m, Build31sizeZ / 2. * m);
  auto logBuild31 = new G4LogicalVolume(boxBuild31, Concrete,
                                        "logBuild31"); // solid, material, name
  G4VPhysicalVolume *physBuild31 =
      new G4PVPlacement(nullptr, // no rotation
                        G4ThreeVector(Build31posX * m, Build31posY * m,
                                      Build31posZ * m), // position
                        logBuild31,                     // logical volume
                        "physBuild31",                  // name
                        expHall_log,                    // mother volume
                        false,                          // no boolean operations
                        0,                              // copy number
                        fCheckOverlaps);                // overlaps checking

  auto boxBuildAir31 =
      new G4Box("boxBuildAir31", (Build31sizeX - 1.) / 2. * m,
                (Build31sizeY - 1.) / 2. * m, (Build31sizeZ - 1.) / 2. * m);
  auto logBuildAir31 = new G4LogicalVolume(
      boxBuildAir31, Air, "logBuildAir31"); // solid, material, name
  G4VPhysicalVolume *physBuildAir31 =
      new G4PVPlacement(nullptr,                // no rotation
                        G4ThreeVector(0, 0, 0), // position
                        logBuildAir31,          // logical volume
                        "physBuildAir31",       // name
                        logBuild31,             // mother volume
                        false,                  // no boolean operations
                        0,                      // copy number
                        fCheckOverlaps);        // overlaps checking

  G4double Build32lsizeX, Build32lsizeY, Build32lsizeZ;
  G4double Build32lposX, Build32lposY, Build32lposZ;
  Build32lsizeX = 12.;
  Build32lsizeY = 25.;
  Build32lsizeZ = 2.77 + 1.35; //???
  Build32lposX = -(35.5 + 17. / 2. - 12. / 2.) - 4.45 / 2.;
  Build32lposY = 60. + 30. / 2. - 25. / 2.;
  Build32lposZ = Build32lsizeZ / 2. - 11.8 / 2.;

  auto boxBuild32l = new G4Box("boxBuild32l", Build32lsizeX / 2. * m,
                               Build32lsizeY / 2. * m, Build32lsizeZ / 2. * m);
  auto logBuild32l = new G4LogicalVolume(
      boxBuild32l, Concrete, "logBuild32l"); // solid, material, name
  G4VPhysicalVolume *physBuild32l =
      new G4PVPlacement(nullptr, // no rotation
                        G4ThreeVector(Build32lposX * m, Build32lposY * m,
                                      Build32lposZ * m), // position
                        logBuild32l,                     // logical volume
                        "physBuild32l",                  // name
                        expHall_log,                     // mother volume
                        false,           // no boolean operations
                        0,               // copy number
                        fCheckOverlaps); // overlaps checking

  auto boxBuildAir32l =
      new G4Box("boxBuildAir32l", (Build32lsizeX - 1.) / 2. * m,
                (Build32lsizeY - 1.) / 2. * m, (Build32lsizeZ - 1.) / 2. * m);
  auto logBuildAir32l = new G4LogicalVolume(
      boxBuildAir32l, Air, "logBuildAir32l"); // solid, material, name
  G4VPhysicalVolume *physBuildAir32l =
      new G4PVPlacement(nullptr,                // no rotation
                        G4ThreeVector(0, 0, 0), // position
                        logBuildAir32l,         // logical volume
                        "physBuildAir32l",      // name
                        logBuild32l,            // mother volume
                        false,                  // no boolean operations
                        0,                      // copy number
                        fCheckOverlaps);        // overlaps checking

  G4double Build32hsizeX, Build32hsizeY, Build32hsizeZ;
  G4double Build32hposX, Build32hposY, Build32hposZ;
  Build32hsizeX = 12.;
  Build32hsizeY = 25.;
  Build32hsizeZ = 4.35 + 2.04; //???
  Build32hposX = -(35.5 + 17. / 2. - 12. / 2.) - 4.45 / 2.;
  Build32hposY = 85. + 30. / 2. - 25. / 2.;
  Build32hposZ = Build32hsizeZ / 2. - 11.8 / 2.;

  auto boxBuild32h = new G4Box("boxBuild32h", Build32hsizeX / 2. * m,
                               Build32hsizeY / 2. * m, Build32hsizeZ / 2. * m);
  auto logBuild32h = new G4LogicalVolume(
      boxBuild32h, Concrete, "logBuild32h"); // solid, material, name
  G4VPhysicalVolume *physBuild32h =
      new G4PVPlacement(nullptr, // no rotation
                        G4ThreeVector(Build32hposX * m, Build32hposY * m,
                                      Build32hposZ * m), // position
                        logBuild32h,                     // logical volume
                        "physBuild32h",                  // name
                        expHall_log,                     // mother volume
                        false,           // no boolean operations
                        0,               // copy number
                        fCheckOverlaps); // overlaps checking

  auto boxBuildAir32h =
      new G4Box("boxBuildAir32h", (Build32hsizeX - 1.) / 2. * m,
                (Build32hsizeY - 1.) / 2. * m, (Build32hsizeZ - 1.) / 2. * m);
  auto logBuildAir32h = new G4LogicalVolume(
      boxBuildAir32h, Air, "logBuildAir32h"); // solid, material, name
  G4VPhysicalVolume *physBuildAir32h =
      new G4PVPlacement(nullptr,                // no rotation
                        G4ThreeVector(0, 0, 0), // position
                        logBuildAir32h,         // logical volume
                        "physBuildAir32h",      // name
                        logBuild32h,            // mother volume
                        false,                  // no boolean operations
                        0,                      // copy number
                        fCheckOverlaps);        // overlaps checking

  G4double Build33sizeX, Build33sizeY, Build33sizeZ;
  G4double Build33posX, Build33posY, Build33posZ;
  Build33sizeX = 15.;
  Build33sizeY = 43.;
  Build33sizeZ = 12.29 + 1.91;
  Build33posX = (50. - 17. / 2. + 15. / 2.) - 4.45 / 2.;
  Build33posY = 21.5 + 30. / 2. - 43. / 2.;
  Build33posZ = Build33sizeZ / 2. - 11.8 / 2.;

  auto boxBuild33 = new G4Box("boxBuild33", Build33sizeX / 2. * m,
                              Build33sizeY / 2. * m, Build33sizeZ / 2. * m);
  auto logBuild33 = new G4LogicalVolume(boxBuild33, Concrete,
                                        "logBuild33"); // solid, material, name
  G4VPhysicalVolume *physBuild33 =
      new G4PVPlacement(nullptr, // no rotation
                        G4ThreeVector(Build33posX * m, Build33posY * m,
                                      Build33posZ * m), // position
                        logBuild33,                     // logical volume
                        "physBuild33",                  // name
                        expHall_log,                    // mother volume
                        false,                          // no boolean operations
                        0,                              // copy number
                        fCheckOverlaps);                // overlaps checking

  auto boxBuildAir33 =
      new G4Box("boxBuildAir33", (Build33sizeX - 1.) / 2. * m,
                (Build33sizeY - 1.) / 2. * m, (Build33sizeZ - 1.) / 2. * m);
  auto logBuildAir33 = new G4LogicalVolume(
      boxBuildAir33, Air, "logBuildAir33"); // solid, material, name
  G4VPhysicalVolume *physBuildAir33 =
      new G4PVPlacement(nullptr,                // no rotation
                        G4ThreeVector(0, 0, 0), // position
                        logBuildAir33,          // logical volume
                        "physBuildAir33",       // name
                        logBuild33,             // mother volume
                        false,                  // no boolean operations
                        0,                      // copy number
                        fCheckOverlaps);        // overlaps checking

  G4double Build6sizeX, Build6sizeY, Build6sizeZ;
  G4double Build6posX, Build6posY, Build6posZ;
  Build6sizeX = 15.;
  Build6sizeY = 65.;
  Build6sizeZ = 12.29 + 1.91;
  Build6posX = (50. - 17. / 2. + 15. / 2.) - 4.45 / 2.;
  Build6posY = 86.5 + 30. / 2. - 65. / 2.;
  Build6posZ = Build6sizeZ / 2. - 11.8 / 2.;

  auto boxBuild6 = new G4Box("boxBuild6", Build6sizeX / 2. * m,
                             Build6sizeY / 2. * m, Build6sizeZ / 2. * m);
  auto logBuild6 = new G4LogicalVolume(boxBuild6, Concrete,
                                       "logBuild6"); // solid, material, name
  G4VPhysicalVolume *physBuild6 = new G4PVPlacement(
      nullptr, // no rotation
      G4ThreeVector(Build6posX * m, Build6posY * m, Build6posZ * m), // position
      logBuild6,       // logical volume
      "physBuild6",    // name
      expHall_log,     // mother volume
      false,           // no boolean operations
      0,               // copy number
      fCheckOverlaps); // overlaps checking

  auto boxBuildAir6 =
      new G4Box("boxBuildAir6", (Build6sizeX - 1.) / 2. * m,
                (Build6sizeY - 1.) / 2. * m, (Build6sizeZ - 1.) / 2. * m);
  auto logBuildAir6 = new G4LogicalVolume(
      boxBuildAir6, Air, "logBuildAir6"); // solid, material, name
  G4VPhysicalVolume *physBuildAir6 =
      new G4PVPlacement(nullptr,                // no rotation
                        G4ThreeVector(0, 0, 0), // position
                        logBuildAir6,           // logical volume
                        "physBuildAir6",        // name
                        logBuild6,              // mother volume
                        false,                  // no boolean operations
                        0,                      // copy number
                        fCheckOverlaps);        // overlaps checking

  G4double Build6asizeX, Build6asizeY, Build6asizeZ;
  G4double Build6aposX, Build6aposY, Build6aposZ;
  Build6asizeX = 17.;
  Build6asizeY = 13.;
  Build6asizeZ = 6.97 + 1.8;
  Build6aposX = (29. + 17. / 2. - 17. / 2.) - 4.45 / 2.;
  Build6aposY = 87 + 30. / 2 - 13. / 2.;
  Build6aposZ = Build6asizeZ / 2. - 11.8 / 2.;

  auto boxBuild6a = new G4Box("boxBuild6a", Build6asizeX / 2. * m,
                              Build6asizeY / 2. * m, Build6asizeZ / 2. * m);
  auto logBuild6a = new G4LogicalVolume(boxBuild6a, Concrete,
                                        "logBuild6a"); // solid, material, name
  G4VPhysicalVolume *physBuild6a =
      new G4PVPlacement(nullptr, // no rotation
                        G4ThreeVector(Build6aposX * m, Build6aposY * m,
                                      Build6aposZ * m), // position
                        logBuild6a,                     // logical volume
                        "physBuild6a",                  // name
                        expHall_log,                    // mother volume
                        false,                          // no boolean operations
                        0,                              // copy number
                        fCheckOverlaps);                // overlaps checking

  auto boxBuildAir6a =
      new G4Box("boxBuildAir6a", (Build6asizeX - 1.) / 2. * m,
                (Build6asizeY - 1.) / 2. * m, (Build6asizeZ - 1.) / 2. * m);
  auto logBuildAir6a = new G4LogicalVolume(
      boxBuildAir6a, Air, "logBuildAir6a"); // solid, material, name
  G4VPhysicalVolume *physBuildAir6a =
      new G4PVPlacement(nullptr,                // no rotation
                        G4ThreeVector(0, 0, 0), // position
                        logBuildAir6a,          // logical volume
                        "physBuildAir6a",       // name
                        logBuild6a,             // mother volume
                        false,                  // no boolean operations
                        0,                      // copy number
                        fCheckOverlaps);        // overlaps checking

  G4double Build7sizeX, Build7sizeY, Build7sizeZ;
  G4double Build7posX, Build7posY, Build7posZ;
  Build7sizeX = 29.;
  Build7sizeY = 30.;
  Build7sizeZ = 4.78 + 1.51;
  Build7posX = -(12. + 17. / 2. - 29. / 2.) - 4.45 / 2.;
  Build7posY = 85. + 30. / 2. - 30. / 2.;
  Build7posZ = Build7sizeZ / 2. - 11.8 / 2.;

  auto boxBuild7 = new G4Box("boxBuild7", Build7sizeX / 2. * m,
                             Build7sizeY / 2. * m, Build7sizeZ / 2. * m);
  auto logBuild7 = new G4LogicalVolume(boxBuild7, Concrete,
                                       "logBuild7"); // solid, material, name
  G4VPhysicalVolume *physBuild7 = new G4PVPlacement(
      nullptr, // no rotation
      G4ThreeVector(Build7posX * m, Build7posY * m, Build7posZ * m), // position
      logBuild7,       // logical volume
      "physBuild7",    // name
      expHall_log,     // mother volume
      false,           // no boolean operations
      0,               // copy number
      fCheckOverlaps); // overlaps checking

  auto boxBuildAir7 =
      new G4Box("boxBuildAir7", (Build7sizeX - 1.) / 2. * m,
                (Build7sizeY - 1.) / 2. * m, (Build7sizeZ - 1.) / 2. * m);
  auto logBuildAir7 = new G4LogicalVolume(
      boxBuildAir7, Air, "logBuildAir7"); // solid, material, name
  G4VPhysicalVolume *physBuildAir7 =
      new G4PVPlacement(nullptr,                // no rotation
                        G4ThreeVector(0, 0, 0), // position
                        logBuildAir7,           // logical volume
                        "physBuildAir7",        // name
                        logBuild7,              // mother volume
                        false,                  // no boolean operations
                        0,                      // copy number
                        fCheckOverlaps);        // overlaps checking

  G4double Build9sizeX, Build9sizeY, Build9sizeZ;
  G4double Build9posX, Build9posY, Build9posZ;
  Build9sizeX = 11.;
  Build9sizeY = 11.;
  Build9sizeZ = 4.78 + 1.51; //???
  Build9posX = (-2. + 17. / 2. - 11. / 2.) - 4.45 / 2.;
  Build9posY = 40. + 30. / 2. - 11. / 2.;
  Build9posZ = Build9sizeZ / 2. - 11.8 / 2.;

  auto boxBuild9 = new G4Box("boxBuild9", Build9sizeX / 2. * m,
                             Build9sizeY / 2. * m, Build9sizeZ / 2. * m);
  auto logBuild9 = new G4LogicalVolume(boxBuild9, Concrete,
                                       "logBuild9"); // solid, material, name
  G4VPhysicalVolume *physBuild9 = new G4PVPlacement(
      nullptr, // no rotation
      G4ThreeVector(Build9posX * m, Build9posY * m, Build9posZ * m), // position
      logBuild9,       // logical volume
      "physBuild9",    // name
      expHall_log,     // mother volume
      false,           // no boolean operations
      0,               // copy number
      fCheckOverlaps); // overlaps checking

  auto boxBuildAir9 =
      new G4Box("boxBuildAir9", (Build9sizeX - 1.) / 2. * m,
                (Build9sizeY - 1.) / 2. * m, (Build9sizeZ - 1.) / 2. * m);
  auto logBuildAir9 = new G4LogicalVolume(
      boxBuildAir9, Air, "logBuildAir9"); // solid, material, name
  G4VPhysicalVolume *physBuildAir9 =
      new G4PVPlacement(nullptr,                // no rotation
                        G4ThreeVector(0, 0, 0), // position
                        logBuildAir9,           // logical volume
                        "physBuildAir9",        // name
                        logBuild9,              // mother volume
                        false,                  // no boolean operations
                        0,                      // copy number
                        fCheckOverlaps);        // overlaps checking

  G4double BuildDsizeX, BuildDsizeY, BuildDsizeZ;
  G4double BuildDposX, BuildDposY, BuildDposZ;
  BuildDsizeX = 60.;
  BuildDsizeY = 15.5;
  BuildDsizeZ = 18.21 + 1.33; //???
  BuildDposX = (89. - 17. / 2. + 60. / 2.) - 4.45 / 2.;
  BuildDposY = -23. + 30. / 2. - 15.5 / 2.;
  BuildDposZ = BuildDsizeZ / 2. - 11.8 / 2.;

  auto boxBuildD = new G4Box("boxBuildD", BuildDsizeX / 2. * m,
                             BuildDsizeY / 2. * m, BuildDsizeZ / 2. * m);
  auto logBuildD = new G4LogicalVolume(boxBuildD, Concrete,
                                       "logBuildD"); // solid, material, name
  G4VPhysicalVolume *physBuildD = new G4PVPlacement(
      nullptr, // no rotation
      G4ThreeVector(BuildDposX * m, BuildDposY * m, BuildDposZ * m), // position
      logBuildD,       // logical volume
      "physBuildD",    // name
      expHall_log,     // mother volume
      false,           // no boolean operations
      0,               // copy number
      fCheckOverlaps); // overlaps checking

  auto boxBuildAirD =
      new G4Box("boxBuildAirD", (BuildDsizeX - 1.) / 2. * m,
                (BuildDsizeY - 1.) / 2. * m, (BuildDsizeZ - 1.) / 2. * m);
  auto logBuildAirD = new G4LogicalVolume(
      boxBuildAirD, Air, "logBuildAirD"); // solid, material, name
  G4VPhysicalVolume *physBuildAirD =
      new G4PVPlacement(nullptr,                // no rotation
                        G4ThreeVector(0, 0, 0), // position
                        logBuildAirD,           // logical volume
                        "physBuildAirD",        // name
                        logBuildD,              // mother volume
                        false,                  // no boolean operations
                        0,                      // copy number
                        fCheckOverlaps);        // overlaps checking

  G4double BuildNewsizeX, BuildNewsizeY, BuildNewsizeZ;
  G4double BuildNewposX, BuildNewposY, BuildNewposZ;
  BuildNewsizeX = 63.;
  BuildNewsizeY = 104.;
  BuildNewsizeZ = 18.21 + 1.33; //???
  BuildNewposX = (37. - 17. / 2. + 63. / 2.) - 4.45 / 2.;
  BuildNewposY = -72. + 30. / 2. - 104. / 2.;
  BuildNewposZ = BuildNewsizeZ / 2. - 11.8 / 2.;

  auto boxBuildNew = new G4Box("boxBuildNew", BuildNewsizeX / 2. * m,
                               BuildNewsizeY / 2. * m, BuildNewsizeZ / 2. * m);
  auto logBuildNew = new G4LogicalVolume(
      boxBuildNew, Concrete, "logBuildNew"); // solid, material, name
  G4VPhysicalVolume *physBuildNew =
      new G4PVPlacement(nullptr, // no rotation
                        G4ThreeVector(BuildNewposX * m, BuildNewposY * m,
                                      BuildNewposZ * m), // position
                        logBuildNew,                     // logical volume
                        "physBuildNew",                  // name
                        expHall_log,                     // mother volume
                        false,           // no boolean operations
                        0,               // copy number
                        fCheckOverlaps); // overlaps checking

  auto boxBuildAirNew =
      new G4Box("boxBuildAirNew", (BuildNewsizeX - 1.) / 2. * m,
                (BuildNewsizeY - 1.) / 2. * m, (BuildNewsizeZ - 1.) / 2. * m);
  auto logBuildAirNew = new G4LogicalVolume(
      boxBuildAirNew, Air, "logBuildAirNew"); // solid, material, name
  G4VPhysicalVolume *physBuildAirNew =
      new G4PVPlacement(nullptr,                // no rotation
                        G4ThreeVector(0, 0, 0), // position
                        logBuildAirNew,         // logical volume
                        "physBuildAirNew",      // name
                        logBuildNew,            // mother volume
                        false,                  // no boolean operations
                        0,                      // copy number
                        fCheckOverlaps);        // overlaps checking

  G4double ReactsizeX, ReactsizeY, ReactsizeZ;
  G4double ReactposX, ReactposY, ReactposZ;
  ReactsizeX = 34.;
  ReactsizeY = 25.;
  ReactsizeZ = 16.9 + 0.92;                            //???
  ReactposX = -(25. + 17. / 2. - 34. / 2) - 4.45 / 2.; //???
  ReactposY = -109. + 30. / 2. - 25. / 2.;             //???
  ReactposZ = ReactsizeZ / 2. - 11.8 / 2.;

  auto boxReact = new G4Box("boxReact", ReactsizeX / 2. * m,
                            ReactsizeY / 2. * m, ReactsizeZ / 2. * m);
  auto logReact = new G4LogicalVolume(boxReact, Concrete,
                                      "logReact"); // solid, material, name
  G4VPhysicalVolume *physReact = new G4PVPlacement(
      nullptr,                                                    // no rotation
      G4ThreeVector(ReactposX * m, ReactposY * m, ReactposZ * m), // position
      logReact,        // logical volume
      "physReact",     // name
      expHall_log,     // mother volume
      false,           // no boolean operations
      0,               // copy number
      fCheckOverlaps); // overlaps checking

  auto boxReactAir =
      new G4Box("boxReactAir", (ReactsizeX - 1.) / 2. * m,
                (ReactsizeY - 1.) / 2. * m, (ReactsizeZ - 1.) / 2. * m);
  auto logReactAir = new G4LogicalVolume(
      boxReactAir, Air, "logReactAir"); // solid, material, name
  G4VPhysicalVolume *physReactAir =
      new G4PVPlacement(nullptr,                // no rotation
                        G4ThreeVector(0, 0, 0), // position
                        logReactAir,            // logical volume
                        "physReactAir",         // name
                        logReact,               // mother volume
                        false,                  // no boolean operations
                        0,                      // copy number
                        fCheckOverlaps);        // overlaps checking

  /*
  G4double csc[9][4][3];
  csc[0][0][0] = -25.36;
  csc[0][0][1] = 5.89;
  csc[0][0][2] = -6.68;
  csc[0][1][0] = -37.61;
  csc[0][1][1] = 5.89;
  csc[0][1][2] = -6.68;
  csc[0][2][0] = -37.61;
  csc[0][2][1] = -7.32;
  csc[0][2][2] = -6.68;
  csc[0][3][0] = -25.36;
  csc[0][3][1] = -7.32;
  csc[0][3][2] = -6.68;

  csc[1][0][0] = -25.36;
  csc[1][0][1] = 37.34;
  csc[1][0][2] = -6.68;
  csc[1][1][0] = -37.61;
  csc[1][1][1] = 37.34;
  csc[1][1][2] = -6.68;
  csc[1][2][0] = -37.61;
  csc[1][2][1] = 24.14;
  csc[1][2][2] = -6.68;
  csc[1][3][0] = -25.36;
  csc[1][3][1] = 24.14;
  csc[1][3][2] = -6.68;

  csc[2][0][0] = 6.48;
  csc[2][0][1] = 12.65;
  csc[2][0][2] = 0.00;
  csc[2][1][0] = -6.87;
  csc[2][1][1] = 12.65;
  csc[2][1][2] = 0.00;
  csc[2][2][0] = -6.87;
  csc[2][2][1] = -12.63;
  csc[2][2][2] = 0.00;
  csc[2][3][0] = 6.48;
  csc[2][3][1] = -12.63;
  csc[2][3][2] = 0.00;

  csc[3][0][0] = 37.37;
  csc[3][0][1] = 10.96;
  csc[3][0][2] = -14.95;
  csc[3][1][0] = 22.47;
  csc[3][1][1] = 10.96;
  csc[3][1][2] = -15.27;
  csc[3][2][0] = 22.47;
  csc[3][2][1] = -3.95;
  csc[3][2][2] = -15.35;
  csc[3][3][0] = 37.37;
  csc[3][3][1] = -3.95;
  csc[3][3][2] = -15.08;

  csc[4][0][0] = 37.37;
  csc[4][0][1] = 45.57;
  csc[4][0][2] = -16.46;
  csc[4][1][0] = 22.98;
  csc[4][1][1] = 45.57;
  csc[4][1][2] = -16.18;
  csc[4][2][0] = 22.98;
  csc[4][2][1] = 28.48;
  csc[4][2][2] = -16.52;
  csc[4][3][0] = 37.37;
  csc[4][3][1] = 28.48;
  csc[4][3][2] = -16.52;

  csc[5][0][0] = 6.92; // 6.91
  csc[5][0][1] = -56.16;
  csc[5][0][2] = -16.15;
  csc[5][1][0] = -2.79;
  csc[5][1][1] = -56.16;
  csc[5][1][2] = -15.91;
  csc[5][2][0] = -2.79;
  csc[5][2][1] = -70.54;
  csc[5][2][2] = -15.93;
  csc[5][3][0] = 6.92; // 6.91
  csc[5][3][1] = -70.54;
  csc[5][3][2] = -16.10;

  csc[6][0][0] = -9.54;
  csc[6][0][1] = 53.62;
  csc[6][0][2] = -17.03;
  csc[6][1][0] = -20.56;
  csc[6][1][1] = 64.71;
  csc[6][1][2] = -17.80;
  csc[6][2][0] = -20.56;
  csc[6][2][1] = 53.62;
  csc[6][2][2] = -17.50;
  csc[6][3][0] = -9.54;
  csc[6][3][1] = 42.54;
  csc[6][3][2] = -16.93;

  csc[7][0][0] = 42.14;
  csc[7][0][1] = -16.80;
  csc[7][0][2] = -15.14;
  csc[7][1][0] = 26.94;
  csc[7][1][1] = -16.80;
  csc[7][1][2] = -15.46;
  csc[7][2][0] = 26.94;
  csc[7][2][1] = -32.10;
  csc[7][2][2] = -15.46;
  csc[7][3][0] = 42.14;
  csc[7][3][1] = -32.10;
  csc[7][3][2] = -14.69;

  csc[8][0][0] = -25.36;
  csc[8][0][1] = -25.75;
  csc[8][0][2] = -7.33;
  csc[8][1][0] = -37.26;
  csc[8][1][1] = -25.75;
  csc[8][1][2] = -7.36;
  csc[8][2][0] = -37.26;
  csc[8][2][1] = -39.55;
  csc[8][2][2] = -7.34;
  csc[8][3][0] = -25.36;
  csc[8][3][1] = -39.55;
  csc[8][3][2] = -7.28;

  G4int icl = 0;
  G4int ist = 0;
  G4int idet = 0;
  G4double dX, dY;
  G4double RoofNEdx1, RoofNEdx2, RoofNEdy1, RoofNEdy2, RoofNEdz;
  G4double RoofNEposX, RoofNEposY, RoofNEposZ;
  G4double AirRoofNEdx1, AirRoofNEdx2, AirRoofNEdy1, AirRoofNEdy2, AirRoofNEdz;
  G4double FloorNEsizeX, FloorNEsizeY, FloorNEsizeZ;
  G4double FloorNEposX, FloorNEposY, FloorNEposZ;
  G4double CaseNEsizeX, CaseNEsizeY, CaseNEsizeZ;
  G4double CaseNEposX, CaseNEposY, CaseNEposZ;
  G4double AirCaseNEsizeX, AirCaseNEsizeY, AirCaseNEsizeZ;
  G4double AirCaseNEposX, AirCaseNEposY, AirCaseNEposZ;
  G4double ScintNEsizeX, ScintNEsizeY, ScintNEsizeZ;
  G4double ScintNEposX, ScintNEposY, ScintNEposZ;

  G4double WallStatNEXsizeX, WallStatNEXsizeY, WallStatNEYsizeX,
      WallStatNEYsizeY, WallStatNEsizeZ;
  G4double WallStatNEXposX, WallStatNEXposY1, WallStatNEXposY2, WallStatNEposZ;
  G4double WallStatNEYposX1, WallStatNEYposX2, WallStatNEYposY;

  RoofNEdz = 100.;
  RoofNEdx1 = 225.;
  RoofNEdx2 = 0.1;
  RoofNEdy1 = 190.;
  RoofNEdy2 = 190.;

  AirRoofNEdz = 99.85;
  AirRoofNEdx1 = 224.7;
  AirRoofNEdx2 = 0.1;
  AirRoofNEdy1 = 190.;
  AirRoofNEdy2 = 190.;

  WallStatNEXsizeX = 189.8;
  WallStatNEXsizeY = 0.1;
  WallStatNEYsizeX = 0.1;
  WallStatNEYsizeY = 190.;
  WallStatNEsizeZ = 50.;

  FloorNEsizeX = 20.;
  FloorNEsizeY = 25.;
  FloorNEsizeZ = 3.;

  ScintNEsizeX = 80.;
  ScintNEsizeY = 80.;
  ScintNEsizeZ = 4.;
  CaseNEsizeX = 85.;
  CaseNEsizeY = 85.;
  CaseNEsizeZ = 15.;
  AirCaseNEsizeX = 84.9;
  AirCaseNEsizeY = 84.9;
  AirCaseNEsizeZ = 14.9;

  for (int ncl = 0; ncl < 9; ncl++) // 7
  {

    if (ncl == 3 || ncl == 4 || ncl == 5 || ncl == 6 || ncl == 7) {

      if (ncl == 3) {
        FloorNEposX = 37.37 - (37.37 - 22.47) / 2. - 4.45 / 2.;
        FloorNEposY = 10.96 - (10.96 + 3.95) / 2.;
        FloorNEposZ = -15.35 + 11.8 - FloorNEsizeZ / 2.;
      }
      if (ncl == 4) {
        FloorNEposX = 37.37 - (37.37 - 22.98) / 2. - 4.45 / 2.;
        FloorNEposY = 45.57 - (45.57 - 28.48) / 2.;
        FloorNEposZ = -16.52 + 11.8 - FloorNEsizeZ / 2.;
      }
      if (ncl == 5) {
        FloorNEposX = 6.92 - (6.92 + 2.79) / 2. - 4.45 / 2.; // 6.91
        FloorNEposY = -56.16 - (70.54 - 56.16) / 2.;
        FloorNEposZ = -16.15 + 11.8 - FloorNEsizeZ / 2.;
      }
      if (ncl == 6) {
        FloorNEposX = -9.54 - (20.56 - 9.54) / 2. - 4.45 / 2.;
        FloorNEposY = 64.71 - (64.71 - 42.54) / 2.;
        FloorNEposZ = -17.80 + 11.8 - FloorNEsizeZ / 2.;
      }
      if (ncl == 7) {
        FloorNEposX = 42.14 - (42.14 - 26.94) / 2. - 4.45 / 2.;
        FloorNEposY = -32.10 + (32.10 - 16.80) / 2.;
        FloorNEposZ = -15.46 + 11.8 - FloorNEsizeZ / 2.;
      }

      boxFloorNE[icl] = new G4Box("boxFloorNE", FloorNEsizeX / 2. * m,
                                  FloorNEsizeY / 2. * m, FloorNEsizeZ / 2. * m);
      logFloorNE[icl] = new G4LogicalVolume(
          boxFloorNE[icl], Air, "logFloorNE"); // solid, material, name
      physFloorNE[icl] =
          new G4PVPlacement(nullptr, // no rotation
                            G4ThreeVector(FloorNEposX * m, FloorNEposY * m,
                                          FloorNEposZ * m), // position
                            logFloorNE[icl],                // logical volume
                            "physFloorNE",                  // name
                            expHall_log,                    // mother volume
                            false,           // no boolean operations
                            icl,             // copy number
                            fCheckOverlaps); // overlaps checking

      icl = icl + 1;
    }

    for (int nst = 0; nst < 4; nst++) {

      RoofNEposX = csc[ncl][nst][0] - 4.45 / 2.;
      RoofNEposY = csc[ncl][nst][1];
      RoofNEposZ = csc[ncl][nst][2] + 11.8 + 0.2 + 0.5 + 0.1 +
                   1. / 2.; //??? 47b+legsNEdet+caseNEdet+gap+roofNEdet/2

      trdRoofNE[ist] = new G4Trd("trdRoofNE", RoofNEdx1 / 2. * cm,
                                 RoofNEdx2 / 2. * cm, RoofNEdy1 / 2. * cm,
                                 RoofNEdy2 / 2. * cm, RoofNEdz / 2. * cm);
      logRoofNE[ist] = new G4LogicalVolume(
          trdRoofNE[ist], mFe, "logRoofNE"); // solid, material, name
      physRoofNE[ist] =
          new G4PVPlacement(nullptr, // no rotation
                            G4ThreeVector(RoofNEposX * m, RoofNEposY * m,
                                          RoofNEposZ * m), // position
                            logRoofNE[ist],                // logical volume
                            "physRoofNE",                  // name
                            expHall_log,                   // mother volume
                            false,           // no boolean operations
                            ist,             // copy number
                            fCheckOverlaps); // overlaps checking

      trdAirRoofNE[ist] =
          new G4Trd("trdAirRoofNE", AirRoofNEdx1 / 2. * cm,
                    AirRoofNEdx2 / 2. * cm, AirRoofNEdy1 / 2. * cm,
                    AirRoofNEdy2 / 2. * cm, AirRoofNEdz / 2. * cm);
      logAirRoofNE[ist] = new G4LogicalVolume(
          trdAirRoofNE[ist], Air, "logAirRoofNE"); // solid, material, name
      physAirRoofNE[ist] = new G4PVPlacement(
          nullptr, // no rotation
          G4ThreeVector(0. * m, 0. * m,
                        (AirRoofNEdz - RoofNEdz) / 2. * cm), // position
          logAirRoofNE[ist],                                 // logical volume
          "physAirRoofNE",                                   // name
          logRoofNE[ist],                                    // mother volume
          false,           // no boolean operations
          ist,             // copy number
          fCheckOverlaps); // overlaps checking

      WallStatNEXposX = csc[ncl][nst][0] - 4.45 / 2.;
      WallStatNEXposY1 = csc[ncl][nst][1] + 1.9 / 2.;
      WallStatNEXposY2 = csc[ncl][nst][1] - 1.9 / 2.;
      WallStatNEYposX1 = csc[ncl][nst][0] - 4.45 / 2. - 1.9 / 2.;
      WallStatNEYposX2 = csc[ncl][nst][0] - 4.45 / 2. + 1.9 / 2.;
      WallStatNEYposY = csc[ncl][nst][1];
      WallStatNEposZ =
          csc[ncl][nst][2] + 11.8 + 0.2 + 0.5 + 0.1 - 0.5 / 2. - 0.02; //

      boxWallStatNEX1[ist] =
          new G4Box("boxWallStatNEX1", WallStatNEXsizeX / 2. * cm,
                    WallStatNEXsizeY / 2. * cm, WallStatNEsizeZ / 2. * cm);
      logWallStatNEX1[ist] =
          new G4LogicalVolume(boxWallStatNEX1[ist], Steel,
                              "logWallStatNEX1"); // solid, material, name
      physWallStatNEX1[ist] = new G4PVPlacement(
          nullptr, // no rotation
          G4ThreeVector(WallStatNEXposX * m, WallStatNEXposY1 * m,
                        WallStatNEposZ * m), // position
          logWallStatNEX1[ist],              // logical volume
          "physWallStatNEX1",                // name
          expHall_log,                       // mother volume
          false,                             // no boolean operations
          idet,                              // copy number
          fCheckOverlaps);                   // overlaps checking

      boxWallStatNEX2[ist] =
          new G4Box("boxWallStatNEX2", WallStatNEXsizeX / 2. * cm,
                    WallStatNEXsizeY / 2. * cm, WallStatNEsizeZ / 2. * cm);
      logWallStatNEX2[ist] =
          new G4LogicalVolume(boxWallStatNEX2[ist], Steel,
                              "logWallStatNEX2"); // solid, material, name
      physWallStatNEX2[ist] = new G4PVPlacement(
          nullptr, // no rotation
          G4ThreeVector(WallStatNEXposX * m, WallStatNEXposY2 * m,
                        WallStatNEposZ * m), // position
          logWallStatNEX2[ist],              // logical volume
          "physWallStatNEX2",                // name
          expHall_log,                       // mother volume
          false,                             // no boolean operations
          idet,                              // copy number
          fCheckOverlaps);                   // overlaps checking

      boxWallStatNEY1[ist] =
          new G4Box("boxWallStatNEY1", WallStatNEYsizeX / 2. * cm,
                    WallStatNEYsizeY / 2. * cm, WallStatNEsizeZ / 2. * cm);
      logWallStatNEY1[ist] =
          new G4LogicalVolume(boxWallStatNEY1[ist], Steel,
                              "logWallStatNEY1"); // solid, material, name
      physWallStatNEY1[ist] = new G4PVPlacement(
          nullptr, // no rotation
          G4ThreeVector(WallStatNEYposX1 * m, WallStatNEYposY * m,
                        WallStatNEposZ * m), // position
          logWallStatNEY1[ist],              // logical volume
          "physWallStatNEY1",                // name
          expHall_log,                       // mother volume
          false,                             // no boolean operations
          idet,                              // copy number
          fCheckOverlaps);                   // overlaps checking

      boxWallStatNEY2[ist] =
          new G4Box("boxWallStatNEY2", WallStatNEYsizeX / 2. * cm,
                    WallStatNEYsizeY / 2. * cm, WallStatNEsizeZ / 2. * cm);
      logWallStatNEY2[ist] =
          new G4LogicalVolume(boxWallStatNEY2[ist], Steel,
                              "logWallStatNEY2"); // solid, material, name
      physWallStatNEY2[ist] = new G4PVPlacement(
          nullptr, // no rotation
          G4ThreeVector(WallStatNEYposX2 * m, WallStatNEYposY * m,
                        WallStatNEposZ * m), // position
          logWallStatNEY2[ist],              // logical volume
          "physWallStatNEY2",                // name
          expHall_log,                       // mother volume
          false,                             // no boolean operations
          idet,                              // copy number
          fCheckOverlaps);                   // overlaps checking

      ist = ist + 1;

      for (int ndet = 0; ndet < 4; ndet++) {

        dXY(nst, ndet, dX, dY);
        CaseNEposX = csc[ncl][nst][0] + dX - 4.45 / 2.;
        CaseNEposY = csc[ncl][nst][1] + dY;
        CaseNEposZ = csc[ncl][nst][2] + 11.8 + 0.2 + 0.5;

        boxCaseNE[idet] =
            new G4Box("boxCaseNE", CaseNEsizeX / 2. * cm, CaseNEsizeY / 2. * cm,
                      CaseNEsizeZ / 2. * cm);
        logCaseNE[idet] = new G4LogicalVolume(
            boxCaseNE[idet], Steel, "logCaseNE"); // solid, material, name
        physCaseNE[idet] =
            new G4PVPlacement(nullptr, // no rotation
                              G4ThreeVector(CaseNEposX * m, CaseNEposY * m,
                                            CaseNEposZ * m), // position
                              logCaseNE[idet],               // logical volume
                              "physCaseNE",                  // name
                              expHall_log,                   // mother volume
                              false,           // no boolean operations
                              idet,            // copy number
                              fCheckOverlaps); // overlaps checking

        boxAirCaseNE[idet] = new G4Box(
            "boxAirCaseNE", (CaseNEsizeX - 0.2) / 2. * cm,
            (CaseNEsizeY - 0.2) / 2. * cm, (CaseNEsizeZ - 0.2) / 2. * cm);
        logAirCaseNE[idet] = new G4LogicalVolume(
            boxAirCaseNE[idet], Air, "logAirCaseNE"); // solid, material, name
        physAirCaseNE[idet] =
            new G4PVPlacement(nullptr, // no rotation
                              G4ThreeVector(0. * m, 0. * m, 0. * m), // position
                              logAirCaseNE[idet], // logical volume
                              "physAirCaseNE",    // name
                              logCaseNE[idet],    // mother volume
                              false,              // no boolean operations
                              idet,               // copy number
                              fCheckOverlaps);    // overlaps checking

        boxScintNE[idet] =
            new G4Box("boxScintNE", ScintNEsizeX / 2. * cm,
                      ScintNEsizeY / 2. * cm, ScintNEsizeZ / 2. * cm);
        logScintNE[idet] = new G4LogicalVolume(
            boxScintNE[idet], C9H10, "logScintNE"); // solid, material, name
        physScintNE[idet] = new G4PVPlacement(
            nullptr, // no rotation
            G4ThreeVector(0. * m, 0. * m,
                          (7.5 - 2. - 0.1 - 0.1) * cm), // position
            logScintNE[idet],                           // logical volume
            "physScintNE",                              // name
            logAirCaseNE[idet],                         // mother volume
            false,                                      // no boolean operations
            idet,                                       // copy number
            fCheckOverlaps);                            // overlaps checking

        idet = idet + 1;
      }
    }
  }
*/
  // --------------- NEVOD-EAS ---------------

  //------------- Surfaces --------------

  G4OpticalSurface *OpPlGlassTubeSurf, *OpPlGlassWaterSurf;

  OpPlGlassTubeSurf = new G4OpticalSurface("OpPlGlassTubeSurf");
  OpPlGlassTubeSurf->SetType(dielectric_metal);
  OpPlGlassTubeSurf->SetFinish(polished);
  OpPlGlassTubeSurf->SetModel(unified);

  G4LogicalBorderSurface *PlGlassTubeSurf[7][4][4][6] = {nullptr},
                         *PlGlassWaterSurf[7][4][4][6] = {nullptr},
                         *PlGlassGlassSurf[7][4][4][6] = {nullptr},
                         *GlassTubeSurf[7][4][4][6] = {nullptr},
                         *GlassFeuSurf[7][4][4][6] = {nullptr},
                         *WaterTubeSurf[7][4][4][6] = {nullptr},
                         *WaterMBoxSurf[7][4][4] = {nullptr};

  for (npl = 0; npl < Npl; npl++) {
    for (nstr = 0; nstr < nms[npl]; nstr++) {
      for (nmod = 0; nmod < nms[npl]; nmod++) {

        for (G4int i = 0; i < 6; i++) {
          GlassFeuSurf[npl][nstr][nmod][i] = new G4LogicalBorderSurface(
              "GlassFeuSurf", Glass_phys[npl][nstr][nmod][i],
              Photokat_phys[npl][nstr][nmod][i], OpPlGlassTubeSurf);
        }
      }
    }
  }

  auto OpSurface = new G4OpticalSurface("BasseinSurface");
  auto Surface =
      new G4LogicalSkinSurface("BasseinSurface", bassein_l, OpSurface);
  OpSurface->SetType(dielectric_dielectric);
  OpSurface->SetModel(unified);
  OpSurface->SetFinish(groundfrontpainted);

  const G4int NUM = 2;
  G4double pp[NUM] = {2.038 * eV, 4.144 * eV};
  G4double reflectivity[NUM] = {0.05, 0.05};

  auto OpSurfaceProperty = new G4MaterialPropertiesTable();
  OpSurfaceProperty->AddProperty("REFLECTIVITY", pp, reflectivity, NUM);
  OpSurface->SetMaterialPropertiesTable(OpSurfaceProperty);

  //-------------- Colour ----------------------

  G4Colour grey(0.5, 0.5, 0.5);
  G4Colour black(0.0, 0.0, 0.0);
  G4Colour red(1.0, 0.0, 0.0);
  G4Colour green(0.0, 1.0, 0.0);
  G4Colour blue(0.0, 0.0, 1.0);
  G4Colour cyan(0.0, 1.0, 1.0);
  G4Colour magenta(1.0, 0.0, 1.0);
  G4Colour yellow(1.0, 1.0, 0.0);

  G4Colour lgrey(153. / 255., 153. / 255., 153. / 255.);
  G4Colour dyellow(153. / 255., 153. / 255., 0. / 255.);
  G4Colour orange(255. / 255., 153. / 255., 0. / 255.);
  G4Colour brown(102. / 255., 51. / 255., 0. / 255.);
  G4Colour lbrown(153. / 255., 102. / 255., 0. / 255.);
  G4Colour dgreen(0. / 255., 102. / 255., 0. / 255.);
  G4Colour olive(102. / 255., 153. / 255., 51. / 255.);
  G4Colour teal(51. / 255., 255. / 255., 153. / 255.);
  G4Colour dcyan(0. / 255., 102. / 255., 102. / 255.);

  expHall_log->SetVisAttributes(G4VisAttributes::GetInvisible());

  auto krishaVisAtt = new G4VisAttributes(black);
  krishaVisAtt->SetVisibility(true);
  krishaVisAtt->SetForceWireframe(true);
  KrishaKer_log->SetVisAttributes(krishaVisAtt);
  KrishaBet_log->SetVisAttributes(krishaVisAtt);

  auto bassVisAtt = new G4VisAttributes(grey);
  bassVisAtt->SetVisibility(true);
  bassVisAtt->SetForceWireframe(true);
  bassein_l->SetVisAttributes(bassVisAtt);

  auto krishkaVisAtt = new G4VisAttributes(lbrown);
  krishkaVisAtt->SetVisibility(true);
  krishkaVisAtt->SetForceWireframe(true);
  krishka_l->SetVisAttributes(krishkaVisAtt);

  auto steniVisAtt = new G4VisAttributes(black);
  steniVisAtt->SetVisibility(true);
  steniVisAtt->SetForceWireframe(true);
  StX_log->SetVisAttributes(steniVisAtt);
  StY_log->SetVisAttributes(steniVisAtt);

  auto watVisAtt = new G4VisAttributes(blue);
  watVisAtt->SetVisibility(true);
  watVisAtt->SetForceWireframe(true);
  water_l->SetVisAttributes(G4VisAttributes::GetInvisible());

  auto airVisAtt = new G4VisAttributes(cyan);
  airVisAtt->SetVisibility(true);
  airVisAtt->SetForceWireframe(true);
  air_l->SetVisAttributes(airVisAtt);

  auto modVisAtt = new G4VisAttributes(grey);
  modVisAtt->SetVisibility(true);
  modVisAtt->SetForceSolid(true);

  auto modaVisAtt = new G4VisAttributes(cyan);
  modaVisAtt->SetVisibility(true);
  modaVisAtt->SetForceSolid(true);

  auto tubVisAtt = new G4VisAttributes(grey);
  tubVisAtt->SetVisibility(true);
  tubVisAtt->SetForceSolid(true);

  auto atubVisAtt = new G4VisAttributes(cyan);
  atubVisAtt->SetVisibility(true);
  atubVisAtt->SetForceSolid(true);

  auto illum1VisAtt = new G4VisAttributes(green);
  illum1VisAtt->SetVisibility(true);
  illum1VisAtt->SetForceSolid(true);

  auto illum2VisAtt = new G4VisAttributes(dgreen);
  illum2VisAtt->SetVisibility(true);
  illum2VisAtt->SetForceSolid(true);

  auto silicVisAtt = new G4VisAttributes(dyellow);
  silicVisAtt->SetVisibility(true);
  silicVisAtt->SetForceSolid(true);

  auto glassVisAtt = new G4VisAttributes(orange);
  glassVisAtt->SetVisibility(true);
  glassVisAtt->SetForceSolid(true);

  auto photVisAtt = new G4VisAttributes(magenta);
  photVisAtt->SetVisibility(true);
  photVisAtt->SetForceSolid(true);

  for (G4int k = 0; k < 6; k++)
    logCtrlNVD[k]->SetVisAttributes(G4VisAttributes::GetInvisible());

  auto detVisAtt = new G4VisAttributes(grey);
  detVisAtt->SetVisibility(true);
  detVisAtt->SetForceWireframe(true);

  auto adetVisAtt = new G4VisAttributes(cyan);
  adetVisAtt->SetVisibility(true);
  adetVisAtt->SetForceWireframe(true);

  auto scineVisAtt = new G4VisAttributes(green);
  scineVisAtt->SetVisibility(true);
  scineVisAtt->SetForceWireframe(true);
  /*
    for (G4int k = 0; k < 144; k++) // 112
    {
      logCaseNE[k]->SetVisAttributes(G4VisAttributes::GetInvisible());
      logAirCaseNE[k]->SetVisAttributes(G4VisAttributes::GetInvisible());
      logScintNE[k]->SetVisAttributes(G4VisAttributes::GetInvisible());
    }

    for (G4int k = 0; k < 36; k++) // 28
    {
      logRoofNE[k]->SetVisAttributes(G4VisAttributes::GetInvisible());
      logAirRoofNE[k]->SetVisAttributes(G4VisAttributes::GetInvisible());
    }

    for (auto &k : logFloorNE) // 3
    {
      k->SetVisAttributes(G4VisAttributes::GetInvisible());
    }
  */

  auto buildVisAtt = new G4VisAttributes(brown);
  buildVisAtt->SetVisibility(false);
  buildVisAtt->SetForceWireframe(true);

  logBuild47->SetVisAttributes(buildVisAtt);
  logBuildAir47->SetVisAttributes(buildVisAtt);

  logBuild31->SetVisAttributes(buildVisAtt);
  logBuildAir31->SetVisAttributes(buildVisAtt);

  logBuild32l->SetVisAttributes(buildVisAtt);
  logBuildAir32l->SetVisAttributes(buildVisAtt);

  logBuild32h->SetVisAttributes(buildVisAtt);
  logBuildAir32h->SetVisAttributes(buildVisAtt);

  logBuild33->SetVisAttributes(buildVisAtt);
  logBuildAir33->SetVisAttributes(buildVisAtt);

  logBuild6->SetVisAttributes(buildVisAtt);
  logBuildAir6->SetVisAttributes(buildVisAtt);

  logBuild6a->SetVisAttributes(buildVisAtt);
  logBuildAir6a->SetVisAttributes(buildVisAtt);

  logBuild7->SetVisAttributes(buildVisAtt);
  logBuildAir7->SetVisAttributes(buildVisAtt);

  logBuild9->SetVisAttributes(buildVisAtt);
  logBuildAir9->SetVisAttributes(buildVisAtt);

  logBuildD->SetVisAttributes(buildVisAtt);
  logBuildAirD->SetVisAttributes(buildVisAtt);

  logBuildNew->SetVisAttributes(buildVisAtt);
  logBuildAirNew->SetVisAttributes(buildVisAtt);

  logReact->SetVisAttributes(buildVisAtt);
  logReactAir->SetVisAttributes(buildVisAtt);

  return expHall_phys;
}

// int dXY(int DS, int det, double &x, double &y) {
//   double xx[] = {0.4255, 0.4255, -0.4255, -0.4255};
//   double yy[] = {-0.4255, 0.4255, 0.4255, -0.4255};
//   int ind0[] = {0, 3, 2, 1};
//   int index;
//   int dir;
//
//   x = 0;
//   y = 0;
//
//   if (DS < 0 || DS > 3)
//     return -1;
//
//   if (det < 0 || det > 3)
//     return -2;
//
//   if (DS & 1)
//
//     dir = -1;
//   else
//     dir = 1;
//
//   index = ind0[DS] + det * dir;
//   if (index < 0)
//     index += 4;
//
//   if (index > 3)
//     index -= 4;
//
//   x = xx[index];
//   y = yy[index];
//
//   return 0;
// }
} // namespace NEVOD
