#include "NVDDetectorConstruction.hh"

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

extern G4int PerevKM[600][4], PerevMK[7][4][4][6];
extern G4int Nfeu;

NVDDetectorConstruction::NVDDetectorConstruction() : fCheckOverlaps(true) {
  expHall_x = expHall_y = expHall_z = 10 * m;
}

NVDDetectorConstruction::~NVDDetectorConstruction() = default;

G4VPhysicalVolume *NVDDetectorConstruction::Construct() {

  //------------- Materials -------------

  G4NistManager *nman = G4NistManager::Instance();

  G4Material *mFe = nman->FindOrBuildMaterial("G4_Fe");
  G4Material *Steel = nman->FindOrBuildMaterial("G4_STAINLESS-STEEL");
  G4Material *Rock = nman->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  G4Material *rubber = nman->FindOrBuildMaterial("G4_RUBBER_NATURAL");

  G4double a, z, density, fractionmass;
  G4int nelements;

  // Air
  auto *N = new G4Element("Nitrogen", "N", z = 7, a = 14.01 * g / mole);
  auto *O = new G4Element("Oxygen", "O", z = 8, a = 16.00 * g / mole);

  auto *Air = new G4Material("Air", density = 1.29 * mg / cm3, nelements = 2);
  Air->AddElement(N, 70. * perCent);
  Air->AddElement(O, 30. * perCent);

  // Water
  auto *H = new G4Element("Hydrogen", "H", z = 1, a = 1.01 * g / mole);

  auto *Water = new G4Material("Water", density = 1.0 * g / cm3, nelements = 2);
  Water->AddElement(H, 2);
  Water->AddElement(O, 1);

  // Concrete
  auto *C = new G4Element("Carbon", "C", z = 6, a = 12.01 * g / mole);
  auto *Na = new G4Element("Natrium", "Na", z = 11, a = 22.99 * g / mole);
  auto *Hg = new G4Element("Hg", "Hg", z = 80, a = 200.59 * g / mole);
  auto *Al = new G4Element("Aluminium", "Al", z = 13, a = 26.98 * g / mole);
  auto *Si = new G4Element("Silicon", "Si", z = 14, a = 28.09 * g / mole);
  auto *K = new G4Element("K", "K", z = 19, a = 39.1 * g / mole);
  auto *Ca = new G4Element("Calzium", "Ca", z = 31, a = 69.72 * g / mole);
  auto *Fe = new G4Element("Iron", "Fe", z = 26, a = 55.85 * g / mole);
  auto *B = new G4Element("Boron", "B", z = 5., a = 10.811 * g / mole);

  auto *Concrete = new G4Material("Concrete", density = 2.3 * g / cm3, 10);
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
  auto *mAl = new G4Material("Aluminum", z = 13., a = 26.98 * g / mole,
                             density = 2.7 * g / cm3);

  // Plexiglass
  auto *Plexiglass =
      new G4Material("Plexiglass", density = 1.19 * g / cm3, nelements = 3);
  Plexiglass->AddElement(C, 0.60);
  Plexiglass->AddElement(O, 0.32);
  Plexiglass->AddElement(H, 0.08);

  // Silikon
  auto *Silicone =
      new G4Material("Silicone", density = 0.98 * g / cm3, nelements = 4);
  Silicone->AddElement(C, 6);
  Silicone->AddElement(H, 18);
  Silicone->AddElement(O, 1);
  Silicone->AddElement(Si, 2);

  // Glass
  auto *Glass =
      new G4Material("Glass", density = 2.23 * g / cm3, nelements = 4);
  Glass->AddElement(Si, 80 * 1);
  Glass->AddElement(B, 12 * 2);
  Glass->AddElement(Na, 8 * 2);
  Glass->AddElement(O, 80 * 2 + 12 * 3 + 8 * 1);

  // Kirpich
  auto *Kirpich =
      new G4Material("Kirpich", density = 1.8 * g / cm3, nelements = 2);
  Kirpich->AddElement(Si, 1);
  Kirpich->AddElement(O, 2);

  // Keramzit
  auto *Keramzit =
      new G4Material("Keramzit", density = 0.5 * g / cm3, nelements = 2);
  Keramzit->AddElement(Si, 1);
  Keramzit->AddElement(O, 2);

  // Scintillator
  auto *Scintillator = new G4Material("Scintillator", density = 1.05 * g / cm3,
                                      nelements = 2); // ???1.06
  Scintillator->AddElement(H, 8);
  Scintillator->AddElement(C, 8);

  G4Material *C9H10 = nman->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

  // PRISMA-URAN
  G4Material *bar_mat = nman->FindOrBuildMaterial("G4_POLYETHYLENE");
  G4Material *floor_mat = nman->FindOrBuildMaterial("G4_CONCRETE");
  auto *floor_mat2 = new G4Material("floor_mat2", 0.9 * g / cm3, 3);
  floor_mat2->AddElement(H, 4);
  floor_mat2->AddElement(O, 1);
  floor_mat2->AddElement(C, 2);

  //
  // ------------ Generate & Add Material Properties Table ------------
  //

  // Water

  const G4int nEntries = 32;

  // ### OpNovice
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

  // ### OpNovice
  G4double AbsorptionW[nEntries] = {
      3.448 * m,  4.082 * m,  6.329 * m,  9.174 * m,  12.346 * m, 13.889 * m,
      15.152 * m, 17.241 * m, 18.868 * m, 20.000 * m, 26.316 * m, 35.714 * m,
      45.455 * m, 47.619 * m, 52.632 * m, 52.632 * m, 55.556 * m, 52.632 * m,
      52.632 * m, 47.619 * m, 45.455 * m, 41.667 * m, 37.037 * m, 33.333 * m,
      30.000 * m, 28.500 * m, 27.000 * m, 24.500 * m, 22.000 * m, 19.500 * m,
      17.500 * m, 14.500 * m};

  // ### OpNovice
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

  auto *myMPTW = new G4MaterialPropertiesTable();
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

  auto *myMPTP = new G4MaterialPropertiesTable();
  myMPTP->AddProperty("RINDEX", PhotonEnergy, RefractiveIndexP, nEntries);
  myMPTP->AddProperty("ABSLENGTH", PhotonEnergy, AbsorptionP, nEntries);
  Plexiglass->SetMaterialPropertiesTable(myMPTP);

  // Glass
  G4double RefractiveIndexG[nEntries];
  for (double &i : RefractiveIndexG)
    i = 1.49;

  //   borosilicate
  G4double AbsorptionG[nEntries];
  for (double &i : AbsorptionG)
    i = 40.0 * m; //???

  auto *myMPTG = new G4MaterialPropertiesTable();
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

  auto *myMPTS = new G4MaterialPropertiesTable();
  myMPTS->AddProperty("RINDEX", PhotonEnergy, RefractiveIndexS, nEntries);
  myMPTS->AddProperty("ABSLENGTH", PhotonEnergy, AbsorptionS, nEntries);
  Silicone->SetMaterialPropertiesTable(myMPTS);

  // Air
  G4double RefractiveIndexA[nEntries];
  for (double &i : RefractiveIndexA)
    i = 1.0;

  auto *myMPTA = new G4MaterialPropertiesTable();
  myMPTA->AddProperty("RINDEX", PhotonEnergy, RefractiveIndexA, nEntries);
  Air->SetMaterialPropertiesTable(myMPTA);

  //------------- Volumes --------------

  expHall_x = 200. * m; //-47+139
  expHall_y = 200. * m; //-162+117
  expHall_z = 50. * m;

  auto *expHall_box = new G4Box("World", expHall_x, expHall_y, expHall_z);
  auto *expHall_log =
      new G4LogicalVolume(expHall_box, Air, "World", nullptr, nullptr, nullptr);
  G4VPhysicalVolume *expHall_phys = new G4PVPlacement(
      nullptr, G4ThreeVector(), expHall_log, "World", nullptr, false, 0);

  // Krisha
  auto *KrishaKer_g =
      new G4Box("KrishaKer", 17.95 / 2. * m, 15.45 * m, 0.125 * m);
  auto *KrishaKer_log = new G4LogicalVolume(KrishaKer_g, Keramzit, "KrishaKer",
                                            nullptr, nullptr, nullptr);

  G4VPhysicalVolume *KrishaKer_phys = new G4PVPlacement(
      nullptr, G4ThreeVector(-4.45 / 2.0 * m, 0. * m, 11.675 * m),

      KrishaKer_log, "KrishaKer", expHall_log, false, 0, fCheckOverlaps);

  auto *KrishaBet_g =
      new G4Box("KrishaBet", 17.95 / 2. * m, 15.45 * m, 0.05 * m);
  auto *KrishaBet_log = new G4LogicalVolume(KrishaBet_g, Kirpich, "KrishaBet",
                                            nullptr, nullptr, nullptr);

  G4VPhysicalVolume *KrishaBet_phys = new G4PVPlacement(
      nullptr, G4ThreeVector(-4.45 / 2.0 * m, 0. * m, 11.5 * m), KrishaBet_log,
      "KrishaBet", expHall_log, false, 0, fCheckOverlaps);

  // Steni
  auto *StY_g = new G4Box("Steni", 17.95 / 2. * m, 0.55 / 2. * m,
                          (9 + 0.6 + 6.65) / 2.0 * m);
  auto *StY_log = new G4LogicalVolume(StY_g, Concrete, "Steni");
  G4VPhysicalVolume *StYmin_phys = new G4PVPlacement(
      nullptr, G4ThreeVector(-4.45 / 2.0 * m, -15.175 * m, 3.325 * m), StY_log,
      "Steni", expHall_log, false, 0, fCheckOverlaps);
  G4VPhysicalVolume *StYmax_phys = new G4PVPlacement(
      nullptr, G4ThreeVector(-4.45 / 2.0 * m, 15.175 * m, 3.325 * m), StY_log,
      "Steni", expHall_log, false, 0, fCheckOverlaps);

  auto *StX_g =
      new G4Box("Steni", 0.55 / 2. * m, 14.9 * m, (9 + 0.6 + 6.65) / 2.0 * m);
  auto *StX_log = new G4LogicalVolume(StX_g, Concrete, "Steni");
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

  auto *bassein_g =
      new G4Box("bassein", basx / 2. * m, basy / 2. * m, basz / 2. * m);
  auto *bassein_l = new G4LogicalVolume(bassein_g, Concrete, "bassein", nullptr,
                                        nullptr, nullptr);
  G4VPhysicalVolume *bassein_phys = new G4PVPlacement(
      nullptr, G4ThreeVector(posx * m, posy * m, posz * m),
      //                                                       bassein_l,"bassein",expHall_log,false,0);
      bassein_l, "bassein", expHall_log, false, 0, fCheckOverlaps);

  auto *krishka_g =
      new G4Box("KrishkaBas", 10.2 / 2. * m, 27.2 / 2. * m, 0.005 / 2. * m);
  auto *krishka_l = new G4LogicalVolume(krishka_g, mFe, "KrishkaBas", nullptr,
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

  auto *water_g = new G4Box("WaterBox", wx / 2. * m, wy / 2. * m, wz / 2. * m);
  auto *water_l = new G4LogicalVolume(water_g, Water, "WaterBox", nullptr,
                                      nullptr, nullptr); // ###

  G4VPhysicalVolume *water_phys = new G4PVPlacement(
      nullptr, G4ThreeVector(posx * m, posy * m, posz * m),
      //                                                     water_l,"WaterBox",bassein_l,false,0);
      water_l, "WaterBox", bassein_l, false, 0, fCheckOverlaps);

  // The air
  G4double ax, ay, az;

  ax = wx;
  ay = wy;
  az = 0.5;
  //   az=0.3; // up water level
  //   az=0.7; // down water level
  posx = 0.;
  posy = 0.;
  posz = wz / 2. - az / 2.;

  auto *air_g = new G4Box("AirBox", ax / 2. * m, ay / 2. * m, az / 2. * m);
  auto *air_l =
      new G4LogicalVolume(air_g, Air, "AirBox", nullptr, nullptr, nullptr);
  G4VPhysicalVolume *air_phys = new G4PVPlacement(
      nullptr, G4ThreeVector(posx * m, posy * m, posz * m), air_l, "AirBox",
      //                                                  water_l,false,0);
      water_l, false, 0, fCheckOverlaps);

  //=====

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

  auto *Mbox_g = new G4Box("MBox", feu_x, feu_y, feu_z);
  auto *MboxA_g =
      new G4Box("MBoxA", feu_x - 5. * mm, feu_y - 5. * mm, feu_z - 5. * mm);

  auto *AlTube_g = new G4Tubs("Tube", 0. * mm, 102.5 * mm, 157. / 2. * mm,
                              startAngle, spanningAngle); // ???

  auto *AirTube_g =
      new G4Tubs("AirTube", 0. * mm, 97.5 * mm, 157. / 2. * mm, startAngle,
                 spanningAngle); // ??? innerRadius, outerRadius, hz

  auto *Illum1_g = new G4Tubs("Illum1", 0. * mm, 102.5 * mm, 8. / 2. * mm,
                              startAngle, spanningAngle); // a) 1/2 v vode
  auto *Illum2_g = new G4Tubs("Illum2", 0. * mm, 97.5 * mm, 16. / 2. * mm,
                              startAngle, spanningAngle); // b) 1 v trube

  auto *Silic_g = new G4Tubs("Silic", innerRadGlass, outerRadGlass,
                             3. / 2. * mm, startAngle, spanningAngle);

  auto *Glass_g = new G4Tubs("Glass", innerRadGlass, outerRadGlass, hightGlass,
                             startAngle, spanningAngle);

  auto *Photokat_g = new G4Tubs("Photokat", innerRadPhotokat, outerRadPhotokat,
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
              //+ no silic      Photokat_phys[npl][nstr][nmod][i] = new
              // G4PVPlacement(0,G4ThreeVector(0.*mm, 0.*mm,
              //(157./2.-16.-6.-0.1/2.)*mm), // b)
              Photokat_log[npl][nstr][nmod][i], "Photokat",
              AirTube_l[npl][nstr][nmod][i], false, nkopy, fCheckOverlaps); //

          PerevKM[nkopy][0] = npl;
          PerevKM[nkopy][1] = nstr;
          PerevKM[nkopy][2] = nmod;
          PerevKM[nkopy][3] = i;

          PerevMK[npl][nstr][nmod][i] = nkopy;

          nkopy++;
        }
      }
    }
  }
  Nfeu = nkopy;

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
          //                                           water_l, // mother volume
          false,           // no boolean operations
          ncnt,            // copy number
          fCheckOverlaps); // overlaps checking

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
          //                                           water_l, // mother volume
          false,           // no boolean operations
          ncnt,            // copy number
          fCheckOverlaps); // overlaps checking

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
      //       CntPosY = (-10.625 + 6.875 + 2.5*j)*m; // centre
      CntPosY = (-13.0 + 6.875 + 2.5 * j) * m;

      boxCount[ncnt] =
          new G4Box("boxCount", CntSizeX / 2., CntSizeY / 2., CntSizeZ / 2.);
      logCount[ncnt] = new G4LogicalVolume(boxCount[ncnt], mAl,
                                           "logCount"); // solid, material, name
      physCount[ncnt] = new G4PVPlacement(
          nullptr, // no rotation
                   //                                           G4ThreeVector(CntPosX,
                   //                                           CntPosY,
                   //                                           Cnt1PosZ), //
                   //                                           position
          G4ThreeVector(CntPosX, CntPosY, Cnt2PosZ), // position
          logCount[ncnt],                            // logical volume
          "physCount",                               // name
          //                                           expHall_log, // mother
          //                                           volume
          water_l,         // mother volume
          false,           // no boolean operations
          ncnt,            // copy number
          fCheckOverlaps); // overlaps checking

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
  // new 30.03.2020

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
  //   smeSMz = 4.5;
  smeSMz = (4.5 - 0.3) * 1000.; // pravilno
                                //   smeSMz = 4.5-0.3-0.3;

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

  for (int npl = 0; npl < 8; npl++) {
    // X strips
    xSM0PosY1 = SM0PosY - 3. * cm - 3. * 6. * cm + npl * 6. * cm -
                1. * nm; // gap = 60 mm
    boxSM0x[npl] =
        new G4Box("SM0x", shSMSizeX[0] / 2., shSMSizeY / 2., shSMSizeZ / 2.);
    logSM0x[npl] = new G4LogicalVolume(boxSM0x[npl], mAl, "SM0x", nullptr,
                                       nullptr, nullptr);
    physSM0x[npl] = new G4PVPlacement(
        nullptr, G4ThreeVector(SM0PosX, xSM0PosY1, SM0PosZ), logSM0x[npl],
        "SM0x", expHall_log, false, npl, fCheckOverlaps);

    xSM1PosY1 = SM1PosY - 3. * cm - 3. * 6. * cm + npl * 6. * cm - 1. * nm;
    boxSM1x[npl] =
        new G4Box("SM1x", shSMSizeX[0] / 2., shSMSizeY / 2., shSMSizeZ / 2.);
    logSM1x[npl] = new G4LogicalVolume(boxSM1x[npl], mAl, "SM1x", nullptr,
                                       nullptr, nullptr);
    physSM1x[npl] = new G4PVPlacement(
        nullptr, G4ThreeVector(SM1PosX, xSM1PosY1, SM1PosZ), logSM1x[npl],
        "SM1x", expHall_log, false, npl, fCheckOverlaps);

    xSM2PosX1 = SM2PosX - 3. * cm - 3. * 6. * cm + npl * 6. * cm - 1. * nm;
    boxSM2x[npl] =
        new G4Box("SM2x", lgSMSizeX / 2., lgSMSizeY[0] / 2., lgSMSizeZ / 2.);
    logSM2x[npl] = new G4LogicalVolume(boxSM2x[npl], mAl, "SM2x", nullptr,
                                       nullptr, nullptr);
    physSM2x[npl] = new G4PVPlacement(
        nullptr, G4ThreeVector(xSM2PosX1, SM2PosY, SM2PosZ), logSM2x[npl],
        "SM2x", expHall_log, false, npl, fCheckOverlaps);

    xSM3PosX1 = SM3PosX - 3. * cm - 3. * 6. * cm + npl * 6. * cm - 1. * nm;
    boxSM3x[npl] =
        new G4Box("SM3x", lgSMSizeX / 2., lgSMSizeY[0] / 2., lgSMSizeZ / 2.);
    logSM3x[npl] = new G4LogicalVolume(boxSM3x[npl], mAl, "SM3x", nullptr,
                                       nullptr, nullptr);
    physSM3x[npl] = new G4PVPlacement(
        nullptr, G4ThreeVector(xSM3PosX1, SM3PosY, SM3PosZ), logSM3x[npl],
        "SM3x", expHall_log, false, npl, fCheckOverlaps);

    xSM4PosX1 = SM4PosX - 3. * cm - 3. * 6. * cm + npl * 6. * cm - 1. * nm;
    boxSM4x[npl] =
        new G4Box("SM4x", lgSMSizeX / 2., lgSMSizeY[0] / 2., lgSMSizeZ / 2.);
    logSM4x[npl] = new G4LogicalVolume(boxSM4x[npl], mAl, "SM4x", nullptr,
                                       nullptr, nullptr);
    physSM4x[npl] = new G4PVPlacement(
        nullptr, G4ThreeVector(xSM4PosX1, SM4PosY, SM4PosZ), logSM4x[npl],
        "SM4x", expHall_log, false, npl, fCheckOverlaps);

    xSM5PosX1 = SM5PosX - 3. * cm - 3. * 6. * cm + npl * 6. * cm - 1. * nm;
    boxSM5x[npl] =
        new G4Box("SM5x", lgSMSizeX / 2., lgSMSizeY[0] / 2., lgSMSizeZ / 2.);
    logSM5x[npl] = new G4LogicalVolume(boxSM5x[npl], mAl, "SM5x", nullptr,
                                       nullptr, nullptr);
    physSM5x[npl] = new G4PVPlacement(
        nullptr, G4ThreeVector(xSM5PosX1, SM5PosY, SM5PosZ), logSM5x[npl],
        "SM5x", expHall_log, false, npl, fCheckOverlaps);

    xSM6PosY1 = SM6PosY - 3. * cm - 3. * 6. * cm + npl * 6. * cm - 1. * nm;
    boxSM6x[npl] =
        new G4Box("SM6x", shSMSizeX[0] / 2., shSMSizeY / 2., shSMSizeZ / 2.);
    logSM6x[npl] = new G4LogicalVolume(boxSM6x[npl], mAl, "SM6x", nullptr,
                                       nullptr, nullptr);
    physSM6x[npl] = new G4PVPlacement(
        nullptr, G4ThreeVector(SM6PosX, xSM6PosY1, SM6PosZ), logSM6x[npl],
        "SM6x", expHall_log, false, npl, fCheckOverlaps);

    xSM7PosY1 = SM7PosY - 3. * cm - 3. * 6. * cm + npl * 6. * cm - 1. * nm;
    boxSM7x[npl] =
        new G4Box("SM7x", shSMSizeX[0] / 2., shSMSizeY / 2., shSMSizeZ / 2.);
    logSM7x[npl] = new G4LogicalVolume(boxSM7x[npl], mAl, "SM7x", nullptr,
                                       nullptr, nullptr);
    physSM7x[npl] = new G4PVPlacement(
        nullptr, G4ThreeVector(SM7PosX, xSM7PosY1, SM7PosZ), logSM7x[npl],
        "SM7x", expHall_log, false, npl, fCheckOverlaps);

    // Y strips
    ySM0PosY1 = SM0PosY - 3. * cm - 3. * 6. * cm + npl * 6. * cm +
                1. * nm; // gap = 60 mm
    boxSM0y[npl] =
        new G4Box("SM0y", shSMSizeX[1] / 2., shSMSizeY / 2., shSMSizeZ / 2.);
    logSM0y[npl] = new G4LogicalVolume(boxSM0y[npl], mAl, "SM0y", nullptr,
                                       nullptr, nullptr);
    physSM0y[npl] = new G4PVPlacement(
        nullptr, G4ThreeVector(SM0PosX, ySM0PosY1, SM0PosZ), logSM0y[npl],
        "SM0y", expHall_log, false, npl, fCheckOverlaps);

    ySM1PosY1 = SM1PosY - 3. * cm - 3. * 6. * cm + npl * 6. * cm + 1. * nm;
    boxSM1y[npl] =
        new G4Box("SM1y", shSMSizeX[1] / 2., shSMSizeY / 2., shSMSizeZ / 2.);
    logSM1y[npl] = new G4LogicalVolume(boxSM1y[npl], mAl, "SM1y", nullptr,
                                       nullptr, nullptr);
    physSM1y[npl] = new G4PVPlacement(
        nullptr, G4ThreeVector(SM1PosX, ySM1PosY1, SM1PosZ), logSM1y[npl],
        "SM1y", expHall_log, false, npl, fCheckOverlaps);

    ySM2PosX1 = SM2PosX - 3. * cm - 3. * 6. * cm + npl * 6. * cm + 1. * nm;
    boxSM2y[npl] =
        new G4Box("SM2y", lgSMSizeX / 2., lgSMSizeY[1] / 2., lgSMSizeZ / 2.);
    logSM2y[npl] = new G4LogicalVolume(boxSM2y[npl], mAl, "SM2y", nullptr,
                                       nullptr, nullptr);
    physSM2y[npl] = new G4PVPlacement(
        nullptr, G4ThreeVector(ySM2PosX1, SM2PosY, SM2PosZ), logSM2y[npl],
        "SM2y", expHall_log, false, npl, fCheckOverlaps);

    ySM3PosX1 = SM3PosX - 3. * cm - 3. * 6. * cm + npl * 6. * cm + 1. * nm;
    boxSM3y[npl] =
        new G4Box("SM3y", lgSMSizeX / 2., lgSMSizeY[1] / 2., lgSMSizeZ / 2.);
    logSM3y[npl] = new G4LogicalVolume(boxSM3y[npl], mAl, "SM3y", nullptr,
                                       nullptr, nullptr);
    physSM3y[npl] = new G4PVPlacement(
        nullptr, G4ThreeVector(ySM3PosX1, SM3PosY, SM3PosZ), logSM3y[npl],
        "SM3y", expHall_log, false, npl, fCheckOverlaps);

    ySM4PosX1 = SM4PosX - 3. * cm - 3. * 6. * cm + npl * 6. * cm + 1. * nm;
    boxSM4y[npl] =
        new G4Box("SM4y", lgSMSizeX / 2., lgSMSizeY[1] / 2., lgSMSizeZ / 2.);
    logSM4y[npl] = new G4LogicalVolume(boxSM4y[npl], mAl, "SM4y", nullptr,
                                       nullptr, nullptr);
    physSM4y[npl] = new G4PVPlacement(
        nullptr, G4ThreeVector(ySM4PosX1, SM4PosY, SM4PosZ), logSM4y[npl],
        "SM4y", expHall_log, false, npl, fCheckOverlaps);

    ySM5PosX1 = SM5PosX - 3. * cm - 3. * 6. * cm + npl * 6. * cm + 1. * nm;
    boxSM5y[npl] =
        new G4Box("SM5y", lgSMSizeX / 2., lgSMSizeY[1] / 2., lgSMSizeZ / 2.);
    logSM5y[npl] = new G4LogicalVolume(boxSM5y[npl], mAl, "SM5y", nullptr,
                                       nullptr, nullptr);
    physSM5y[npl] = new G4PVPlacement(
        nullptr, G4ThreeVector(ySM5PosX1, SM5PosY, SM5PosZ), logSM5y[npl],
        "SM5y", expHall_log, false, npl, fCheckOverlaps);

    ySM6PosY1 = SM6PosY - 3. * cm - 3. * 6. * cm + npl * 6. * cm + 1. * nm;
    boxSM6y[npl] =
        new G4Box("SM6y", shSMSizeX[1] / 2., shSMSizeY / 2., shSMSizeZ / 2.);
    logSM6y[npl] = new G4LogicalVolume(boxSM6y[npl], mAl, "SM6y", nullptr,
                                       nullptr, nullptr);
    physSM6y[npl] = new G4PVPlacement(
        nullptr, G4ThreeVector(SM6PosX, ySM6PosY1, SM6PosZ), logSM6y[npl],
        "SM6y", expHall_log, false, npl, fCheckOverlaps);

    ySM7PosY1 = SM7PosY - 3. * cm - 3. * 6. * cm + npl * 6. * cm + 1. * nm;
    boxSM7y[npl] =
        new G4Box("SM7y", shSMSizeX[1] / 2., shSMSizeY / 2., shSMSizeZ / 2.);
    logSM7y[npl] = new G4LogicalVolume(boxSM7y[npl], mAl, "SM7y", nullptr,
                                       nullptr, nullptr);
    physSM7y[npl] = new G4PVPlacement(
        nullptr, G4ThreeVector(SM7PosX, ySM7PosY1, SM7PosZ), logSM7y[npl],
        "SM7y", expHall_log, false, npl, fCheckOverlaps);
  }

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

  auto *OpSurface = new G4OpticalSurface("BasseinSurface");
  auto *Surface =
      new G4LogicalSkinSurface("BasseinSurface", bassein_l, OpSurface);
  OpSurface->SetType(dielectric_dielectric);
  OpSurface->SetModel(unified);
  OpSurface->SetFinish(groundfrontpainted);

  const G4int NUM = 2;
  G4double pp[NUM] = {2.038 * eV, 4.144 * eV};
  G4double reflectivity[NUM] = {0.05, 0.05};

  auto *OpSurfaceProperty = new G4MaterialPropertiesTable();
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

  auto *krishaVisAtt = new G4VisAttributes(black);
  krishaVisAtt->SetVisibility(true);
  krishaVisAtt->SetForceWireframe(true);
  KrishaKer_log->SetVisAttributes(krishaVisAtt);
  KrishaBet_log->SetVisAttributes(krishaVisAtt);

  auto *bassVisAtt = new G4VisAttributes(grey);
  bassVisAtt->SetVisibility(true);
  bassVisAtt->SetForceWireframe(true);
  bassein_l->SetVisAttributes(bassVisAtt);

  auto *krishkaVisAtt = new G4VisAttributes(lbrown);
  krishkaVisAtt->SetVisibility(true);
  krishkaVisAtt->SetForceWireframe(true);
  krishka_l->SetVisAttributes(krishkaVisAtt);

  auto *steniVisAtt = new G4VisAttributes(black);
  steniVisAtt->SetVisibility(true);
  steniVisAtt->SetForceWireframe(true);
  StX_log->SetVisAttributes(steniVisAtt);
  StY_log->SetVisAttributes(steniVisAtt);

  auto *watVisAtt = new G4VisAttributes(blue);
  watVisAtt->SetVisibility(true);
  watVisAtt->SetForceWireframe(true);
  water_l->SetVisAttributes(G4VisAttributes::GetInvisible());

  auto *airVisAtt = new G4VisAttributes(cyan);
  airVisAtt->SetVisibility(true);
  airVisAtt->SetForceWireframe(true);
  air_l->SetVisAttributes(airVisAtt);

  auto *modVisAtt = new G4VisAttributes(grey);
  modVisAtt->SetVisibility(true);
  modVisAtt->SetForceSolid(true);

  auto *modaVisAtt = new G4VisAttributes(cyan);
  modaVisAtt->SetVisibility(true);
  modaVisAtt->SetForceSolid(true);

  auto *tubVisAtt = new G4VisAttributes(grey);
  tubVisAtt->SetVisibility(true);
  tubVisAtt->SetForceSolid(true);

  auto *atubVisAtt = new G4VisAttributes(cyan);
  atubVisAtt->SetVisibility(true);
  atubVisAtt->SetForceSolid(true);

  auto *illum1VisAtt = new G4VisAttributes(green);
  illum1VisAtt->SetVisibility(true);
  illum1VisAtt->SetForceSolid(true);

  auto *illum2VisAtt = new G4VisAttributes(dgreen);
  illum2VisAtt->SetVisibility(true);
  illum2VisAtt->SetForceSolid(true);

  auto *silicVisAtt = new G4VisAttributes(dyellow);
  silicVisAtt->SetVisibility(true);
  silicVisAtt->SetForceSolid(true);

  auto *glassVisAtt = new G4VisAttributes(orange);
  glassVisAtt->SetVisibility(true);
  glassVisAtt->SetForceSolid(true);

  auto *photVisAtt = new G4VisAttributes(magenta);
  photVisAtt->SetVisibility(true);
  photVisAtt->SetForceSolid(true);

  for (G4int k = 0; k < 6; k++)
    logCtrlNVD[k]->SetVisAttributes(G4VisAttributes::GetInvisible());

  auto *detVisAtt = new G4VisAttributes(grey);
  detVisAtt->SetVisibility(true);
  detVisAtt->SetForceWireframe(true);

  auto *adetVisAtt = new G4VisAttributes(cyan);
  adetVisAtt->SetVisibility(true);
  adetVisAtt->SetForceWireframe(true);

  auto *scineVisAtt = new G4VisAttributes(green);
  scineVisAtt->SetVisibility(true);
  scineVisAtt->SetForceWireframe(true);

  auto *buildVisAtt = new G4VisAttributes(brown);
  buildVisAtt->SetVisibility(true);
  buildVisAtt->SetForceWireframe(true);

  return expHall_phys;
}