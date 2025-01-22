#include "detector/DetectorConstruction.hh"

namespace nevod {

int dXY(int DS, int det, double& x, double& y);

DetectorConstruction::DetectorConstruction(Communicator* communicator): communicator_(communicator), check_overlaps_(true) {
  experimental_hall_x_ = experimental_hall_y_ = 200. * m;
  experimental_hall_z_ = 50 * m;

  auto params = communicator_->GetSimulationParams();
  construction_flags_ = params.construction_flags;

  switch (params.config_qsm) {
    case CherenkovConfig::OLD_CONFIGURATION:
      cwd_plane_number_ = 7;
      qsm_config_.resize(cwd_plane_number_);
      stride_config_.resize(cwd_plane_number_);
      for (size_t i = 0; i < cwd_plane_number_; ++i) {
        if (i % 2 == 0) {
          qsm_config_[i] = 4;
          stride_config_[i] = 4;
        } else {
          qsm_config_[i] = 3;
          stride_config_[i] = 3;
        }
      }
      break;
    case CherenkovConfig::NEW_CONFIGURATION:
      cwd_plane_number_ = 6;
      qsm_config_.resize(cwd_plane_number_);
      stride_config_.resize(cwd_plane_number_);
      for (auto& qsm: qsm_config_)
        qsm = 4;
      for (auto& stride: stride_config_)
        stride = 4;
      break;
  }

  switch (params.config_sct) {
    case SCTConfig::OLD_CONFIGURATION:
      sct_plane_number_ = {5, 4};
      break;
    case SCTConfig::NEW_CONFIGURATION:
      sct_plane_number_ = {7, 6};
      break;
  }
  sct_counter_number_ = {4, 5};

  G4int counter_number = (sct_plane_number_.first * sct_counter_number_.first + sct_plane_number_.second * sct_counter_number_.first) * 2;
  sct_counter_box_.resize(counter_number);
  sct_counter_log_.resize(counter_number);
  sct_counter_phys_.resize(counter_number);

  sct_scint_box_.resize(counter_number);
  sct_scint_log_.resize(counter_number);
  sct_scint_phys_.resize(counter_number);

  super_module_box_.resize(DECOR_COUNT);
  super_module_log_.resize(DECOR_COUNT);
  super_module_phys_.resize(DECOR_COUNT);

  for (auto& super_module: super_module_box_)
    super_module.resize(DECOR_CHAMBER_COUNT);
  for (auto& super_module: super_module_log_)
    super_module.resize(DECOR_CHAMBER_COUNT);
  for (auto& super_module: super_module_phys_)
    super_module.resize(DECOR_CHAMBER_COUNT);

  control_nevod_box_.resize(BOX_SIDE_COUNT);
  control_nevod_log_.resize(BOX_SIDE_COUNT);
  control_nevod_phys_.resize(BOX_SIDE_COUNT);
}

G4VPhysicalVolume* DetectorConstruction::Construct() {
  GenerateMaterials();

  //============================================================================
  // Materials
  //============================================================================

  auto nist_manager = G4NistManager::Instance();
  auto air = nist_manager->FindOrBuildMaterial("Air");

  //============================================================================
  // Experimental hall
  //============================================================================

  auto world_box = new G4Box("World", experimental_hall_x_, experimental_hall_y_, experimental_hall_z_);
  auto world_log = new G4LogicalVolume(world_box, air, "World");

  G4VPhysicalVolume* world_phys = new G4PVPlacement(nullptr, G4ThreeVector(), world_log, "World", nullptr, false, 0);

  // Set visibility for experimental hall
  world_log->SetVisAttributes(G4VisAttributes::GetInvisible());

  //============================================================================
  // Buildings
  //============================================================================

  G4LogicalVolume* water_log = BuildNEVOD(world_log);
  if (!construction_flags_.build_nevod_only) BuildOtherBuildings(world_log);

  //============================================================================
  // Detectors
  //============================================================================

  // TODO Rewrite PRISMA, URAN and EAS detectors
  // if (construction_flags_.build_prisma) BuildPRISMA();
  // if (construction_flags_.build_uran) BuildURAN();

  if (construction_flags_.build_cwd) BuildCWD(water_log);
  if (construction_flags_.build_sct) BuildSCT(world_log, water_log);
  if (construction_flags_.build_decor) BuildDECOR(world_log);

  // if (construction_flags_.build_eas) BuildEAS();

  return world_phys;
}

void DetectorConstruction::ConstructSDandField() {}

void DetectorConstruction::GenerateMaterials() {
  G4NistManager* nist_manager = G4NistManager::Instance();

  //============================================================================
  // Elements
  //============================================================================

  auto Si = new G4Element("Silicon", "Si", 14, 28.09 * g / mole);
  auto H = new G4Element("Hydrogen", "H", 1, 1.01 * g / mole);
  auto O = new G4Element("Oxygen", "O", 8, 16.00 * g / mole);
  auto C = new G4Element("Carbon", "C", 6, 12.01 * g / mole);
  auto Na = new G4Element("Sodium", "Na", 11, 22.99 * g / mole);
  auto Hg = new G4Element("Mercury", "Hg", 80, 200.59 * g / mole);
  auto Al = new G4Element("Aluminium", "Al", 13, 26.98 * g / mole);
  auto K = new G4Element("Potassium", "K", 19, 39.1 * g / mole);
  auto N = new G4Element("Nitrogen", "N", 7, 14.01 * g / mole);
  auto Ca = new G4Element("Calcium", "Ca", 31, 69.72 * g / mole);
  auto Fe = new G4Element("Iron", "Fe", 26, 55.85 * g / mole);
  auto B = new G4Element("Boron", "B", 5., 10.811 * g / mole);

  //============================================================================
  // Materials
  //============================================================================

  // auto air = nist_manager->FindOrBuildMaterial("G4_AIR");
  // auto ferrum = nist_manager->FindOrBuildMaterial("G4_Fe");
  // auto steel = nist_manager->FindOrBuildMaterial("G4_STAINLESS-STEEL");
  // auto rock = nist_manager->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  // auto rubber = nist_manager->FindOrBuildMaterial("G4_RUBBER_NATURAL");

  // Air
  auto air = new G4Material("Air", 1.29 * mg / cm3, 2);
  air->AddElement(N, 70. * perCent);
  air->AddElement(O, 30. * perCent);

  // Claydite
  auto claydite = new G4Material("Claydite", 0.5 * g / cm3, 2);
  claydite->AddElement(Si, 1);
  claydite->AddElement(O, 2);

  // Brick
  auto brick = new G4Material("Brick", 1.8 * g / cm3, 2);
  brick->AddElement(Si, 1);
  brick->AddElement(O, 2);

  // Concrete
  auto concrete = new G4Material("Concrete", 2.3 * g / cm3, 10);
  concrete->AddElement(H, 0.01);
  concrete->AddElement(O, 0.529);
  concrete->AddElement(Na, 0.016);
  concrete->AddElement(Hg, 0.002);
  concrete->AddElement(Al, 0.034);
  concrete->AddElement(Si, 0.337);
  concrete->AddElement(K, 0.013);
  concrete->AddElement(Ca, 0.044);
  concrete->AddElement(Fe, 0.014);
  concrete->AddElement(C, 0.001);

  // Water
  auto water = new G4Material("Water", 1.0 * g / cm3, 2);
  water->AddElement(H, 2);
  water->AddElement(O, 1);

  // Aluminium
  auto aluminium = new G4Material("Aluminium", 13., 26.98 * g / mole, 2.7 * g / cm3);

  auto plexiglass = new G4Material("Plexiglass", 1.19 * g / cm3, 3);
  plexiglass->AddElement(C, 0.60);
  plexiglass->AddElement(O, 0.32);
  plexiglass->AddElement(H, 0.08);

  // Silikone
  auto silicone = new G4Material("Silicone", 0.98 * g / cm3, 4);
  silicone->AddElement(C, 6);
  silicone->AddElement(H, 18);
  silicone->AddElement(O, 1);
  silicone->AddElement(Si, 2);

  // Glass
  auto glass = new G4Material("Glass", 2.23 * g / cm3, 4);
  glass->AddElement(Si, 80);
  glass->AddElement(B, 24);
  glass->AddElement(Na, 16);
  glass->AddElement(O, 204);

  // Scintillator
  auto scintillator = new G4Material("Scintillator", 1.05 * g / cm3, 2);
  scintillator->AddElement(H, 8);
  scintillator->AddElement(C, 8);

  // NEVOD-EAS
  // auto C9H10 = nist_manager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

  // PRISMA-URAN
  // auto barrel = nist_manager->FindOrBuildMaterial("G4_POLYETHYLENE");
  // auto floor_material = nist_manager->FindOrBuildMaterial("G4_CONCRETE");
  auto floor_material_2 = new G4Material("FloorMaterial", 0.9 * g / cm3, 3);
  floor_material_2->AddElement(H, 4);
  floor_material_2->AddElement(O, 1);
  floor_material_2->AddElement(C, 2);

  //============================================================================
  // Generate & Add Material Properties Table
  //============================================================================

  const G4int entry_num = 32;

  G4double photon_energy[entry_num] = {
      2.034 * eV, 2.068 * eV, 2.103 * eV, 2.139 * eV, 2.177 * eV, 2.216 * eV, 2.256 * eV, 2.298 * eV, 2.341 * eV, 2.386 * eV, 2.433 * eV,
      2.481 * eV, 2.532 * eV, 2.585 * eV, 2.640 * eV, 2.697 * eV, 2.757 * eV, 2.820 * eV, 2.885 * eV, 2.954 * eV, 3.026 * eV, 3.102 * eV,
      3.181 * eV, 3.265 * eV, 3.353 * eV, 3.446 * eV, 3.545 * eV, 3.649 * eV, 3.760 * eV, 3.877 * eV, 4.002 * eV, 4.136 * eV,
  };

  // Water
  G4double refractive_index_water[entry_num] = {
      1.3435, 1.344,  1.3445, 1.345, 1.3455, 1.346, 1.3465, 1.347, 1.3475, 1.348,  1.3485, 1.3492, 1.35,  1.3505, 1.351, 1.3518,
      1.3522, 1.3530, 1.3535, 1.354, 1.3545, 1.355, 1.3555, 1.356, 1.3568, 1.3572, 1.358,  1.3585, 1.359, 1.3595, 1.36,  1.3608,
  };

  G4double absorption_water[entry_num] = {
      3.448 * m,  4.082 * m,  6.329 * m,  9.174 * m,  12.346 * m, 13.889 * m, 15.152 * m, 17.241 * m, 18.868 * m, 20.000 * m, 26.316 * m,
      35.714 * m, 45.455 * m, 47.619 * m, 52.632 * m, 52.632 * m, 55.556 * m, 52.632 * m, 52.632 * m, 47.619 * m, 45.455 * m, 41.667 * m,
      37.037 * m, 33.333 * m, 30.000 * m, 28.500 * m, 27.000 * m, 24.500 * m, 22.000 * m, 19.500 * m, 17.500 * m, 14.500 * m,
  };

  G4double energy_water[] = {
      1.56962 * eV, 1.58974 * eV, 1.61039 * eV, 1.63157 * eV, 1.65333 * eV, 1.67567 * eV, 1.69863 * eV, 1.72222 * eV, 1.74647 * eV, 1.77142 * eV,
      1.7971 * eV,  1.82352 * eV, 1.85074 * eV, 1.87878 * eV, 1.90769 * eV, 1.93749 * eV, 1.96825 * eV, 1.99999 * eV, 2.03278 * eV, 2.06666 * eV,
      2.10169 * eV, 2.13793 * eV, 2.17543 * eV, 2.21428 * eV, 2.25454 * eV, 2.29629 * eV, 2.33962 * eV, 2.38461 * eV, 2.43137 * eV, 2.47999 * eV,
      2.53061 * eV, 2.58333 * eV, 2.63829 * eV, 2.69565 * eV, 2.75555 * eV, 2.81817 * eV, 2.88371 * eV, 2.95237 * eV, 3.02438 * eV, 3.09999 * eV,
      3.17948 * eV, 3.26315 * eV, 3.35134 * eV, 3.44444 * eV, 3.54285 * eV, 3.64705 * eV, 3.75757 * eV, 3.87499 * eV, 3.99999 * eV, 4.13332 * eV,
      4.27585 * eV, 4.42856 * eV, 4.59258 * eV, 4.76922 * eV, 4.95999 * eV, 5.16665 * eV, 5.39129 * eV, 5.63635 * eV, 5.90475 * eV, 6.19998 * eV,
  };

  // assume 100 times larger than the rayleigh scattering for now
  G4double mie_water[] = {
      167024.4 * m, 158726.7 * m, 150742 * m,   143062.5 * m, 135680.2 * m, 128587.4 * m, 121776.3 * m, 115239.5 * m, 108969.5 * m, 102958.8 * m,
      97200.35 * m, 91686.86 * m, 86411.33 * m, 81366.79 * m, 76546.42 * m, 71943.46 * m, 67551.29 * m, 63363.36 * m, 59373.25 * m, 55574.61 * m,
      51961.24 * m, 48527.00 * m, 45265.87 * m, 42171.94 * m, 39239.39 * m, 36462.50 * m, 33835.68 * m, 31353.41 * m, 29010.30 * m, 26801.03 * m,
      24720.42 * m, 22763.36 * m, 20924.88 * m, 19200.07 * m, 17584.16 * m, 16072.45 * m, 14660.38 * m, 13343.46 * m, 12117.33 * m, 10977.70 * m,
      9920.416 * m, 8941.407 * m, 8036.711 * m, 7202.470 * m, 6434.927 * m, 5730.429 * m, 5085.425 * m, 4496.467 * m, 3960.210 * m, 3473.413 * m,
      3032.937 * m, 2635.746 * m, 2278.907 * m, 1959.588 * m, 1675.064 * m, 1422.710 * m, 1200.004 * m, 1004.528 * m, 833.9666 * m, 686.1063 * m,
  };

  G4double cos_theta = 0.99;
  G4double cos_theta_back = 0.99;
  G4double f_ratio = 0.8;

  auto mat_prop_table_water = new G4MaterialPropertiesTable();
  mat_prop_table_water->AddProperty("RINDEX", photon_energy, refractive_index_water, entry_num);
  mat_prop_table_water->AddProperty("ABSLENGTH", photon_energy, absorption_water, entry_num);
  mat_prop_table_water->AddProperty("MIEHG", energy_water, mie_water, 60);
  mat_prop_table_water->AddConstProperty("MIEHG_FORWARD", cos_theta);
  mat_prop_table_water->AddConstProperty("MIEHG_BACKWARD", cos_theta);
  mat_prop_table_water->AddConstProperty("MIEHG_FORWARD_RATIO", f_ratio);
  water->SetMaterialPropertiesTable(mat_prop_table_water);

  // Plexiglass
  G4double refractive_index_plexiglass[entry_num];
  for (double& i: refractive_index_plexiglass)
    i = 1.474;

  G4double absorption_plexiglass[entry_num];
  for (double& i: absorption_plexiglass)
    i = 40.0 * m;

  auto mat_prop_table_plexiglass = new G4MaterialPropertiesTable();
  mat_prop_table_plexiglass->AddProperty("RINDEX", photon_energy, refractive_index_plexiglass, entry_num);
  mat_prop_table_plexiglass->AddProperty("ABSLENGTH", photon_energy, absorption_plexiglass, entry_num);
  plexiglass->SetMaterialPropertiesTable(mat_prop_table_plexiglass);

  // Glass
  G4double refractive_index_glass[entry_num];
  for (double& i: refractive_index_glass)
    i = 1.49;

  G4double absorption_glass[entry_num];
  for (double& i: absorption_glass)
    i = 40.0 * m;

  auto mat_prop_table_glass = new G4MaterialPropertiesTable();
  mat_prop_table_glass->AddProperty("RINDEX", photon_energy, refractive_index_glass, entry_num);
  mat_prop_table_glass->AddProperty("ABSLENGTH", photon_energy, absorption_glass, entry_num);
  glass->SetMaterialPropertiesTable(mat_prop_table_glass);

  // Silicone
  G4double refractive_index_silicone[entry_num];
  for (double& i: refractive_index_silicone)
    i = 1.4325;
  G4double absorption_silicone[entry_num];
  for (double& i: absorption_silicone)
    i = 40.0 * m;

  auto mat_prop_table_silicone = new G4MaterialPropertiesTable();
  mat_prop_table_silicone->AddProperty("RINDEX", photon_energy, refractive_index_silicone, entry_num);
  mat_prop_table_silicone->AddProperty("ABSLENGTH", photon_energy, absorption_silicone, entry_num);
  silicone->SetMaterialPropertiesTable(mat_prop_table_silicone);

  // Air
  G4double refractive_index_air[entry_num];
  for (double& i: refractive_index_air)
    i = 1.0;

  auto mat_prop_table_air = new G4MaterialPropertiesTable();
  mat_prop_table_air->AddProperty("RINDEX", photon_energy, refractive_index_air, entry_num);
  air->SetMaterialPropertiesTable(mat_prop_table_air);
}

G4LogicalVolume* DetectorConstruction::BuildNEVOD(G4LogicalVolume* world_log) {
  //============================================================================
  // Materials
  //============================================================================

  auto nist_manager = G4NistManager::Instance();
  auto claydite = nist_manager->FindOrBuildMaterial("Claydite");
  auto brick = nist_manager->FindOrBuildMaterial("Brick");
  auto concrete = nist_manager->FindOrBuildMaterial("Concrete");
  auto water = nist_manager->FindOrBuildMaterial("Water");
  auto air = nist_manager->FindOrBuildMaterial("Air");
  auto ferrum = nist_manager->FindOrBuildMaterial("G4_Fe");

  //============================================================================
  // Roof
  //============================================================================

  G4double roof_x = 17.95 / 2 * m;
  G4double roof_y = 15.45 * m;
  G4double claydite_roof_z = 0.25 * m;
  G4double brick_roof_z = 0.05 * m;
  G4ThreeVector claydite_roof_pos = G4ThreeVector(-4.45 / 2.0 * m, 0. * m, 11.675 * m);
  G4ThreeVector brick_roof_pos = G4ThreeVector(-4.45 / 2.0 * m, 0. * m, 11.5 * m);

  auto roof_claydite_box = new G4Box("RoofClaydite", roof_x, roof_y, claydite_roof_z);
  auto roof_claydite_log = new G4LogicalVolume(roof_claydite_box, claydite, "RoofClaydite");
  G4VPhysicalVolume* roof_claydite_phys =
      new G4PVPlacement(nullptr, claydite_roof_pos, roof_claydite_log, "RoofClaydite", world_log, false, 0, check_overlaps_);

  auto roof_brick_box = new G4Box("RoofBrick", roof_x, roof_y, brick_roof_z);
  auto roof_brick_log = new G4LogicalVolume(roof_brick_box, brick, "RoofBrick");
  G4VPhysicalVolume* roof_brick_phys = new G4PVPlacement(nullptr, brick_roof_pos, roof_brick_log, "RoofBrick", world_log, false, 0, check_overlaps_);

  //============================================================================
  // Walls
  //============================================================================

  G4double wall_y_x = 17.95 / 2 * m;
  G4double wall_y_y = 0.55 / 2 * m;
  G4double wall_y_z = (9 + 0.6 + 6.65) / 2 * m;
  G4ThreeVector wall_y_pos_min = G4ThreeVector(-4.45 / 2 * m, -15.175 * m, 3.325 * m);
  G4ThreeVector wall_y_pos_max = G4ThreeVector(-4.45 / 2 * m, 15.175 * m, 3.325 * m);

  G4double wall_x_x = 0.55 / 2 * m;
  G4double wall_x_y = 14.9 * m;
  G4double wall_x_z = (9 + 0.6 + 6.65) / 2 * m;
  G4ThreeVector wall_x_pos_min = G4ThreeVector(-10.925 * m, 0 * m, 3.325 * m);
  G4ThreeVector wall_x_pos_max = G4ThreeVector(6.475 * m, 0 * m, 3.325 * m);

  auto wall_y_box = new G4Box("Walls", wall_y_x, wall_y_y, wall_y_z);
  auto wall_y_log = new G4LogicalVolume(wall_y_box, concrete, "Walls");
  G4VPhysicalVolume* wall_y_min_phys = new G4PVPlacement(nullptr, wall_y_pos_min, wall_y_log, "Walls", world_log, false, 0, check_overlaps_);
  G4VPhysicalVolume* wall_y_max_phys = new G4PVPlacement(nullptr, wall_y_pos_max, wall_y_log, "Walls", world_log, false, 0, check_overlaps_);

  auto wall_x_box = new G4Box("Walls", wall_x_x, wall_x_y, wall_x_z);
  auto wall_x_log = new G4LogicalVolume(wall_x_box, concrete, "Walls");
  G4VPhysicalVolume* wall_x_min_phys = new G4PVPlacement(nullptr, wall_x_pos_min, wall_x_log, "Walls", world_log, false, 0, check_overlaps_);
  G4VPhysicalVolume* wall_x_max_phys = new G4PVPlacement(nullptr, wall_x_pos_max, wall_x_log, "Walls", world_log, false, 0, check_overlaps_);

  //============================================================================
  // The concrete pool
  //============================================================================

  G4double pool_x = 10.2 / 2. * m;
  G4double pool_y = 27.2 / 2. * m;
  G4double pool_z = 9.6 / 2. * m;
  G4double pool_cap_z = 0.005 / 2. * m;
  G4ThreeVector pool_pos = G4ThreeVector(0 * m, 0 * m, 0 * m);
  G4ThreeVector pool_cap_pos = G4ThreeVector(0 * m, 0 * m, (9.6 / 2 + 0.005 / 2) * m);

  auto pool_box = new G4Box("Pool", pool_x, pool_y, pool_z);
  auto pool_log = new G4LogicalVolume(pool_box, concrete, "Pool");
  G4VPhysicalVolume* pool_phys = new G4PVPlacement(nullptr, pool_pos, pool_log, "Pool", world_log, false, 0, check_overlaps_);

  auto pool_cap_box = new G4Box("PoolCap", pool_x, pool_y, pool_cap_z);
  auto pool_cap_log = new G4LogicalVolume(pool_cap_box, ferrum, "PoolCap");
  G4VPhysicalVolume* pool_cap_phys = new G4PVPlacement(nullptr, pool_cap_pos, pool_cap_log, "PoolCap", world_log, false, 0, check_overlaps_);

  //============================================================================
  // The water box
  //============================================================================

  G4double h_walls = 0.6 * m;
  G4double water_x = (pool_x - 2. * h_walls) / 2;
  G4double water_y = (pool_y - 2. * h_walls) / 2;
  G4double water_z = (pool_z - h_walls) / 2;
  G4ThreeVector water_pos = G4ThreeVector(0 * m, 0 * m, h_walls / 2);

  auto water_box = new G4Box("WaterBox", water_x, water_y, water_z);
  auto water_log = new G4LogicalVolume(water_box, water, "WaterBox");
  G4VPhysicalVolume* water_phys = new G4PVPlacement(nullptr, water_pos, water_log, "WaterBox", pool_log, false, 0, check_overlaps_);

  auto optical_surface = new G4OpticalSurface("PoolSurface");
  auto surface = new G4LogicalSkinSurface("BasseinSurface", water_log, optical_surface);
  optical_surface->SetType(dielectric_dielectric);
  optical_surface->SetModel(unified);
  optical_surface->SetFinish(groundfrontpainted);

  const G4int entry_num = 2;
  G4double photon_energy[entry_num] = {2.038 * eV, 4.144 * eV};
  G4double reflectivity[entry_num] = {0.05, 0.05};

  auto optical_surface_property = new G4MaterialPropertiesTable();
  optical_surface_property->AddProperty("REFLECTIVITY", photon_energy, reflectivity, entry_num);
  optical_surface->SetMaterialPropertiesTable(optical_surface_property);

  //============================================================================
  // The air
  //============================================================================

  G4double air_x = water_x;
  G4double air_y = water_y;
  G4double air_z = 0.5;
  G4ThreeVector air_pos = G4ThreeVector(0 * m, 0 * m, water_z / 2 - air_z / 2);

  auto air_box = new G4Box("AirBox", air_x / 2. * m, air_y / 2. * m, air_z / 2. * m);
  auto air_log = new G4LogicalVolume(air_box, air, "AirBox");
  G4VPhysicalVolume* air_phys = new G4PVPlacement(nullptr, air_pos, air_log, "AirBox", water_log, false, 0, check_overlaps_);

  //============================================================================
  // NEVOD muon tracks control
  //============================================================================

  G4double control_nevod_size_x[BOX_SIDE_COUNT] = {
      (9. - 2. * 1.E-6) * m,
      1.E-6 * m,
      (9. - 2. * 1.E-6) * m,
      1.E-6 * m,
      (9. - 2. * 1.E-6) * m,
      (9. - 2. * 1.E-6) * m,
  };
  G4double control_nevod_size_y[BOX_SIDE_COUNT] = {
      1.E-6 * m,
      26. * m,
      1.E-6 * m,
      26. * m,
      (26. - 2. * 1.E-6) * m,
      (26. - 2. * 1.E-6) * m,
  };
  G4double control_nevod_size_z[BOX_SIDE_COUNT] = {
      8.5 * m,
      8.5 * m,
      8.5 * m,
      8.5 * m,
      1.E-6 * m,
      1.E-6 * m,
  };
  G4ThreeVector control_nevod_position[BOX_SIDE_COUNT] = {
      G4ThreeVector(0, (13. - 1.E-6 / 2.) * m, -0.25 * m),
      G4ThreeVector((4.5 - 1.E-6 / 2.) * m, 0, -0.25 * m),
      G4ThreeVector(0, -(13. - 1.E-6 / 2.) * m, -0.25 * m),
      G4ThreeVector(-(4.5 - 1.E-6 / 2.) * m, 0, -0.25 * m),
      G4ThreeVector(0, 0, (-4.5 + 1.E-6 / 2.) * m),
      G4ThreeVector(0, 0, (-4.5 + 1.E-6 / 2.) * m),
  };

  for (size_t i = 0; i < BOX_SIDE_COUNT; ++i) {
    control_nevod_box_[i] = new G4Box("ControlNEVOD", control_nevod_size_x[i] / 2., control_nevod_size_y[i] / 2., control_nevod_size_z[i] / 2.);
    control_nevod_log_[i] = new G4LogicalVolume(control_nevod_box_[i], water, "ControlNEVOD");

    control_nevod_phys_[i] =
        new G4PVPlacement(nullptr, control_nevod_position[i], control_nevod_log_[i], "ControlNEVOD", water_log, false, i, check_overlaps_);
  }

  //============================================================================
  // Set visibility
  //============================================================================

  // colors
  G4Colour grey(0.5, 0.5, 0.5);
  G4Colour black(0.0, 0.0, 0.0);
  G4Colour lbrown(153. / 255., 102. / 255., 0. / 255.);
  G4Colour blue(0.0, 0.0, 1.0);
  G4Colour cyan(0.0, 1.0, 1.0);

  // settings
  auto krishaVisAtt = new G4VisAttributes(black);
  krishaVisAtt->SetVisibility(true);
  krishaVisAtt->SetForceWireframe(true);
  roof_claydite_log->SetVisAttributes(krishaVisAtt);
  roof_brick_log->SetVisAttributes(krishaVisAtt);

  auto bassVisAtt = new G4VisAttributes(grey);
  bassVisAtt->SetVisibility(true);
  bassVisAtt->SetForceWireframe(true);
  pool_log->SetVisAttributes(bassVisAtt);

  auto krishkaVisAtt = new G4VisAttributes(lbrown);
  krishkaVisAtt->SetVisibility(true);
  krishkaVisAtt->SetForceWireframe(true);
  pool_cap_log->SetVisAttributes(krishkaVisAtt);

  auto steniVisAtt = new G4VisAttributes(black);
  steniVisAtt->SetVisibility(true);
  steniVisAtt->SetForceWireframe(true);
  wall_x_log->SetVisAttributes(steniVisAtt);
  wall_y_log->SetVisAttributes(steniVisAtt);

  auto watVisAtt = new G4VisAttributes(blue);
  watVisAtt->SetVisibility(true);
  watVisAtt->SetForceWireframe(true);
  water_log->SetVisAttributes(G4VisAttributes::GetInvisible());

  auto airVisAtt = new G4VisAttributes(cyan);
  airVisAtt->SetVisibility(true);
  airVisAtt->SetForceWireframe(true);
  air_log->SetVisAttributes(airVisAtt);

  for (size_t i = 0; i < 6; ++i)
    control_nevod_log_[i]->SetVisAttributes(G4VisAttributes::GetInvisible());

  return water_log;
}

void DetectorConstruction::BuildOtherBuildings(G4LogicalVolume* world_log) {
  //============================================================================
  // Materials
  //============================================================================

  auto nist_manager = G4NistManager::Instance();
  auto concrete = nist_manager->FindOrBuildMaterial("Concrete");
  auto air = nist_manager->FindOrBuildMaterial("Air");

  //============================================================================
  // Buildings construction
  //============================================================================

  // TODO: rewrite this part of the code for full match

  G4double Build47sizeX, Build47sizeY, Build47sizeZ;
  G4double Build47posX, Build47posY, Build47posZ;
  Build47sizeX = 16;
  Build47sizeY = 52;
  Build47sizeZ = 10.28 + 0.79;
  Build47posX = -32 - 4.45 / 2.;
  Build47posY = 24.5 + 30. / 2. - 52. / 2.;
  Build47posZ = 11.8 - 6.68 - Build47sizeZ / 2.;

  auto boxBuild47 = new G4Box("boxBuild47", Build47sizeX / 2. * m, Build47sizeY / 2. * m, Build47sizeZ / 2. * m);
  auto logBuild47 = new G4LogicalVolume(boxBuild47, concrete, "logBuild47");
  G4VPhysicalVolume* physBuild47 = new G4PVPlacement(
      nullptr, G4ThreeVector(Build47posX * m, Build47posY * m, Build47posZ * m), logBuild47, "physBuild47", world_log, false, 0, check_overlaps_);

  auto boxBuildAir47 = new G4Box("boxBuildAir47", (Build47sizeX - 1.) / 2. * m, (Build47sizeY - 1.) / 2. * m, (Build47sizeZ - 1.) / 2. * m);
  auto logBuildAir47 = new G4LogicalVolume(boxBuildAir47, air, "logBuildAir47");
  G4VPhysicalVolume* physBuildAir47 =
      new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logBuildAir47, "physBuildAir47", logBuild47, false, 0, check_overlaps_);

  G4double Build31sizeX, Build31sizeY, Build31sizeZ;
  G4double Build31posX, Build31posY, Build31posZ;
  Build31sizeX = 16.;
  Build31sizeY = 65.;
  Build31sizeZ = 10.28 + 0.79;
  Build31posX = -32. - 4.45 / 2.;
  Build31posY = (-65. / 2. + 30. / 2.) - 27.5;
  Build31posZ = 11.8 - 7.37 - Build31sizeZ / 2.;

  auto boxBuild31 = new G4Box("boxBuild31", Build31sizeX / 2. * m, Build31sizeY / 2. * m, Build31sizeZ / 2. * m);
  auto logBuild31 = new G4LogicalVolume(boxBuild31, concrete, "logBuild31");
  G4VPhysicalVolume* physBuild31 = new G4PVPlacement(
      nullptr, G4ThreeVector(Build31posX * m, Build31posY * m, Build31posZ * m), logBuild31, "physBuild31", world_log, false, 0, check_overlaps_);

  auto boxBuildAir31 = new G4Box("boxBuildAir31", (Build31sizeX - 1.) / 2. * m, (Build31sizeY - 1.) / 2. * m, (Build31sizeZ - 1.) / 2. * m);
  auto logBuildAir31 = new G4LogicalVolume(boxBuildAir31, air, "logBuildAir31");
  G4VPhysicalVolume* physBuildAir31 =
      new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logBuildAir31, "physBuildAir31", logBuild31, false, 0, check_overlaps_);

  G4double Build32lsizeX, Build32lsizeY, Build32lsizeZ;
  G4double Build32lposX, Build32lposY, Build32lposZ;
  Build32lsizeX = 12.;
  Build32lsizeY = 25.;
  Build32lsizeZ = 2.77 + 1.35;
  Build32lposX = -(35.5 + 17. / 2. - 12. / 2.) - 4.45 / 2.;
  Build32lposY = 60. + 30. / 2. - 25. / 2.;
  Build32lposZ = Build32lsizeZ / 2. - 11.8 / 2.;

  auto boxBuild32l = new G4Box("boxBuild32l", Build32lsizeX / 2. * m, Build32lsizeY / 2. * m, Build32lsizeZ / 2. * m);
  auto logBuild32l = new G4LogicalVolume(boxBuild32l, concrete, "logBuild32l");
  G4VPhysicalVolume* physBuild32l = new G4PVPlacement(
      nullptr,
      G4ThreeVector(Build32lposX * m, Build32lposY * m, Build32lposZ * m),
      logBuild32l,
      "physBuild32l",
      world_log,
      false,
      0,
      check_overlaps_);

  auto boxBuildAir32l = new G4Box("boxBuildAir32l", (Build32lsizeX - 1.) / 2. * m, (Build32lsizeY - 1.) / 2. * m, (Build32lsizeZ - 1.) / 2. * m);
  auto logBuildAir32l = new G4LogicalVolume(boxBuildAir32l, air, "logBuildAir32l");
  G4VPhysicalVolume* physBuildAir32l =
      new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logBuildAir32l, "physBuildAir32l", logBuild32l, false, 0, check_overlaps_);

  G4double Build32hsizeX, Build32hsizeY, Build32hsizeZ;
  G4double Build32hposX, Build32hposY, Build32hposZ;
  Build32hsizeX = 12.;
  Build32hsizeY = 25.;
  Build32hsizeZ = 4.35 + 2.04;
  Build32hposX = -(35.5 + 17. / 2. - 12. / 2.) - 4.45 / 2.;
  Build32hposY = 85. + 30. / 2. - 25. / 2.;
  Build32hposZ = Build32hsizeZ / 2. - 11.8 / 2.;

  auto boxBuild32h = new G4Box("boxBuild32h", Build32hsizeX / 2. * m, Build32hsizeY / 2. * m, Build32hsizeZ / 2. * m);
  auto logBuild32h = new G4LogicalVolume(boxBuild32h, concrete, "logBuild32h");
  G4VPhysicalVolume* physBuild32h = new G4PVPlacement(
      nullptr,
      G4ThreeVector(Build32hposX * m, Build32hposY * m, Build32hposZ * m),
      logBuild32h,
      "physBuild32h",
      world_log,
      false,
      0,
      check_overlaps_);

  auto boxBuildAir32h = new G4Box("boxBuildAir32h", (Build32hsizeX - 1.) / 2. * m, (Build32hsizeY - 1.) / 2. * m, (Build32hsizeZ - 1.) / 2. * m);
  auto logBuildAir32h = new G4LogicalVolume(boxBuildAir32h, air, "logBuildAir32h");
  G4VPhysicalVolume* physBuildAir32h =
      new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logBuildAir32h, "physBuildAir32h", logBuild32h, false, 0, check_overlaps_);

  G4double Build33sizeX, Build33sizeY, Build33sizeZ;
  G4double Build33posX, Build33posY, Build33posZ;
  Build33sizeX = 15.;
  Build33sizeY = 43.;
  Build33sizeZ = 12.29 + 1.91;
  Build33posX = (50. - 17. / 2. + 15. / 2.) - 4.45 / 2.;
  Build33posY = 21.5 + 30. / 2. - 43. / 2.;
  Build33posZ = Build33sizeZ / 2. - 11.8 / 2.;

  auto boxBuild33 = new G4Box("boxBuild33", Build33sizeX / 2. * m, Build33sizeY / 2. * m, Build33sizeZ / 2. * m);
  auto logBuild33 = new G4LogicalVolume(boxBuild33, concrete, "logBuild33");
  G4VPhysicalVolume* physBuild33 = new G4PVPlacement(
      nullptr, G4ThreeVector(Build33posX * m, Build33posY * m, Build33posZ * m), logBuild33, "physBuild33", world_log, false, 0, check_overlaps_);

  auto boxBuildAir33 = new G4Box("boxBuildAir33", (Build33sizeX - 1.) / 2. * m, (Build33sizeY - 1.) / 2. * m, (Build33sizeZ - 1.) / 2. * m);
  auto logBuildAir33 = new G4LogicalVolume(boxBuildAir33, air, "logBuildAir33");
  G4VPhysicalVolume* physBuildAir33 =
      new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logBuildAir33, "physBuildAir33", logBuild33, false, 0, check_overlaps_);

  G4double Build6sizeX, Build6sizeY, Build6sizeZ;
  G4double Build6posX, Build6posY, Build6posZ;
  Build6sizeX = 15.;
  Build6sizeY = 65.;
  Build6sizeZ = 12.29 + 1.91;
  Build6posX = (50. - 17. / 2. + 15. / 2.) - 4.45 / 2.;
  Build6posY = 86.5 + 30. / 2. - 65. / 2.;
  Build6posZ = Build6sizeZ / 2. - 11.8 / 2.;

  auto boxBuild6 = new G4Box("boxBuild6", Build6sizeX / 2. * m, Build6sizeY / 2. * m, Build6sizeZ / 2. * m);
  auto logBuild6 = new G4LogicalVolume(boxBuild6, concrete, "logBuild6");
  G4VPhysicalVolume* physBuild6 = new G4PVPlacement(
      nullptr, G4ThreeVector(Build6posX * m, Build6posY * m, Build6posZ * m), logBuild6, "physBuild6", world_log, false, 0, check_overlaps_);

  auto boxBuildAir6 = new G4Box("boxBuildAir6", (Build6sizeX - 1.) / 2. * m, (Build6sizeY - 1.) / 2. * m, (Build6sizeZ - 1.) / 2. * m);
  auto logBuildAir6 = new G4LogicalVolume(boxBuildAir6, air, "logBuildAir6");
  G4VPhysicalVolume* physBuildAir6 =
      new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logBuildAir6, "physBuildAir6", logBuild6, false, 0, check_overlaps_);

  G4double Build6asizeX, Build6asizeY, Build6asizeZ;
  G4double Build6aposX, Build6aposY, Build6aposZ;
  Build6asizeX = 17.;
  Build6asizeY = 13.;
  Build6asizeZ = 6.97 + 1.8;
  Build6aposX = (29. + 17. / 2. - 17. / 2.) - 4.45 / 2.;
  Build6aposY = 87 + 30. / 2 - 13. / 2.;
  Build6aposZ = Build6asizeZ / 2. - 11.8 / 2.;

  auto boxBuild6a = new G4Box("boxBuild6a", Build6asizeX / 2. * m, Build6asizeY / 2. * m, Build6asizeZ / 2. * m);
  auto logBuild6a = new G4LogicalVolume(boxBuild6a, concrete, "logBuild6a");
  G4VPhysicalVolume* physBuild6a = new G4PVPlacement(
      nullptr, G4ThreeVector(Build6aposX * m, Build6aposY * m, Build6aposZ * m), logBuild6a, "physBuild6a", world_log, false, 0, check_overlaps_);

  auto boxBuildAir6a = new G4Box("boxBuildAir6a", (Build6asizeX - 1.) / 2. * m, (Build6asizeY - 1.) / 2. * m, (Build6asizeZ - 1.) / 2. * m);
  auto logBuildAir6a = new G4LogicalVolume(boxBuildAir6a, air, "logBuildAir6a");
  G4VPhysicalVolume* physBuildAir6a =
      new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logBuildAir6a, "physBuildAir6a", logBuild6a, false, 0, check_overlaps_);

  G4double Build7sizeX, Build7sizeY, Build7sizeZ;
  G4double Build7posX, Build7posY, Build7posZ;
  Build7sizeX = 29.;
  Build7sizeY = 30.;
  Build7sizeZ = 4.78 + 1.51;
  Build7posX = -(12. + 17. / 2. - 29. / 2.) - 4.45 / 2.;
  Build7posY = 85. + 30. / 2. - 30. / 2.;
  Build7posZ = Build7sizeZ / 2. - 11.8 / 2.;

  auto boxBuild7 = new G4Box("boxBuild7", Build7sizeX / 2. * m, Build7sizeY / 2. * m, Build7sizeZ / 2. * m);
  auto logBuild7 = new G4LogicalVolume(boxBuild7, concrete, "logBuild7");
  G4VPhysicalVolume* physBuild7 = new G4PVPlacement(
      nullptr, G4ThreeVector(Build7posX * m, Build7posY * m, Build7posZ * m), logBuild7, "physBuild7", world_log, false, 0, check_overlaps_);

  auto boxBuildAir7 = new G4Box("boxBuildAir7", (Build7sizeX - 1.) / 2. * m, (Build7sizeY - 1.) / 2. * m, (Build7sizeZ - 1.) / 2. * m);
  auto logBuildAir7 = new G4LogicalVolume(boxBuildAir7, air, "logBuildAir7");
  G4VPhysicalVolume* physBuildAir7 =
      new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logBuildAir7, "physBuildAir7", logBuild7, false, 0, check_overlaps_);

  G4double Build9sizeX, Build9sizeY, Build9sizeZ;
  G4double Build9posX, Build9posY, Build9posZ;
  Build9sizeX = 11.;
  Build9sizeY = 11.;
  Build9sizeZ = 4.78 + 1.51;  //???
  Build9posX = (-2. + 17. / 2. - 11. / 2.) - 4.45 / 2.;
  Build9posY = 40. + 30. / 2. - 11. / 2.;
  Build9posZ = Build9sizeZ / 2. - 11.8 / 2.;

  auto boxBuild9 = new G4Box("boxBuild9", Build9sizeX / 2. * m, Build9sizeY / 2. * m, Build9sizeZ / 2. * m);
  auto logBuild9 = new G4LogicalVolume(boxBuild9, concrete, "logBuild9");
  G4VPhysicalVolume* physBuild9 = new G4PVPlacement(
      nullptr, G4ThreeVector(Build9posX * m, Build9posY * m, Build9posZ * m), logBuild9, "physBuild9", world_log, false, 0, check_overlaps_);

  auto boxBuildAir9 = new G4Box("boxBuildAir9", (Build9sizeX - 1.) / 2. * m, (Build9sizeY - 1.) / 2. * m, (Build9sizeZ - 1.) / 2. * m);
  auto logBuildAir9 = new G4LogicalVolume(boxBuildAir9, air, "logBuildAir9");
  G4VPhysicalVolume* physBuildAir9 =
      new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logBuildAir9, "physBuildAir9", logBuild9, false, 0, check_overlaps_);

  G4double BuildDsizeX, BuildDsizeY, BuildDsizeZ;
  G4double BuildDposX, BuildDposY, BuildDposZ;
  BuildDsizeX = 60.;
  BuildDsizeY = 15.5;
  BuildDsizeZ = 18.21 + 1.33;
  BuildDposX = (89. - 17. / 2. + 60. / 2.) - 4.45 / 2.;
  BuildDposY = -23. + 30. / 2. - 15.5 / 2.;
  BuildDposZ = BuildDsizeZ / 2. - 11.8 / 2.;

  auto boxBuildD = new G4Box("boxBuildD", BuildDsizeX / 2. * m, BuildDsizeY / 2. * m, BuildDsizeZ / 2. * m);
  auto logBuildD = new G4LogicalVolume(boxBuildD, concrete, "logBuildD");
  G4VPhysicalVolume* physBuildD = new G4PVPlacement(
      nullptr, G4ThreeVector(BuildDposX * m, BuildDposY * m, BuildDposZ * m), logBuildD, "physBuildD", world_log, false, 0, check_overlaps_);

  auto boxBuildAirD = new G4Box("boxBuildAirD", (BuildDsizeX - 1.) / 2. * m, (BuildDsizeY - 1.) / 2. * m, (BuildDsizeZ - 1.) / 2. * m);
  auto logBuildAirD = new G4LogicalVolume(boxBuildAirD, air, "logBuildAirD");
  G4VPhysicalVolume* physBuildAirD =
      new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logBuildAirD, "physBuildAirD", logBuildD, false, 0, check_overlaps_);

  G4double BuildNewsizeX, BuildNewsizeY, BuildNewsizeZ;
  G4double BuildNewposX, BuildNewposY, BuildNewposZ;
  BuildNewsizeX = 63.;
  BuildNewsizeY = 104.;
  BuildNewsizeZ = 18.21 + 1.33;
  BuildNewposX = (37. - 17. / 2. + 63. / 2.) - 4.45 / 2.;
  BuildNewposY = -72. + 30. / 2. - 104. / 2.;
  BuildNewposZ = BuildNewsizeZ / 2. - 11.8 / 2.;

  auto boxBuildNew = new G4Box("boxBuildNew", BuildNewsizeX / 2. * m, BuildNewsizeY / 2. * m, BuildNewsizeZ / 2. * m);
  auto logBuildNew = new G4LogicalVolume(boxBuildNew, concrete, "logBuildNew");
  G4VPhysicalVolume* physBuildNew = new G4PVPlacement(
      nullptr,
      G4ThreeVector(BuildNewposX * m, BuildNewposY * m, BuildNewposZ * m),
      logBuildNew,
      "physBuildNew",
      world_log,
      false,
      0,
      check_overlaps_);

  auto boxBuildAirNew = new G4Box("boxBuildAirNew", (BuildNewsizeX - 1.) / 2. * m, (BuildNewsizeY - 1.) / 2. * m, (BuildNewsizeZ - 1.) / 2. * m);
  auto logBuildAirNew = new G4LogicalVolume(boxBuildAirNew, air, "logBuildAirNew");
  G4VPhysicalVolume* physBuildAirNew =
      new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logBuildAirNew, "physBuildAirNew", logBuildNew, false, 0, check_overlaps_);

  G4double ReactsizeX, ReactsizeY, ReactsizeZ;
  G4double ReactposX, ReactposY, ReactposZ;
  ReactsizeX = 34.;
  ReactsizeY = 25.;
  ReactsizeZ = 16.9 + 0.92;
  ReactposX = -(25. + 17. / 2. - 34. / 2) - 4.45 / 2.;
  ReactposY = -109. + 30. / 2. - 25. / 2.;
  ReactposZ = ReactsizeZ / 2. - 11.8 / 2.;

  auto boxReact = new G4Box("boxReact", ReactsizeX / 2. * m, ReactsizeY / 2. * m, ReactsizeZ / 2. * m);
  auto logReact = new G4LogicalVolume(boxReact, concrete, "logReact");
  G4VPhysicalVolume* physReact = new G4PVPlacement(
      nullptr, G4ThreeVector(ReactposX * m, ReactposY * m, ReactposZ * m), logReact, "physReact", world_log, false, 0, check_overlaps_);

  auto boxReactAir = new G4Box("boxReactAir", (ReactsizeX - 1.) / 2. * m, (ReactsizeY - 1.) / 2. * m, (ReactsizeZ - 1.) / 2. * m);
  auto logReactAir = new G4LogicalVolume(boxReactAir, air, "logReactAir");
  G4VPhysicalVolume* physReactAir =
      new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logReactAir, "physReactAir", logReact, false, 0, check_overlaps_);

  G4Colour brown(102. / 255., 51. / 255., 0. / 255.);

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
}

// NEVOD
void DetectorConstruction::BuildCWD(G4LogicalVolume* water_log) {
  //============================================================================
  // Materials
  //============================================================================

  auto nist_manager = G4NistManager::Instance();
  auto glass = nist_manager->FindOrBuildMaterial("Glass");
  auto air = nist_manager->FindOrBuildMaterial("Air");
  auto aluminium = nist_manager->FindOrBuildMaterial("Aluminium");
  auto plexiglass = nist_manager->FindOrBuildMaterial("Plexiglass");
  auto silicone = nist_manager->FindOrBuildMaterial("Silicone");

  //============================================================================
  // Tubes
  //============================================================================

  G4double pmt_size_x = 120.0 * mm;
  G4double pmt_size_y = 120.0 * mm;
  G4double pmt_size_z = 120.0 * mm;

  G4double start_angle = 0. * deg;
  G4double spanning_angle = 360. * deg;

  G4double inner_rad_glass = 0.0 * mm;
  G4double outer_rad_glass = 7.5 * cm;
  G4double height_glass = 6.0 / 2.0 * mm;

  G4double inner_rad_photocathode = 0.0 * mm;
  G4double outer_rad_photocathode = 150.0 / 2.0 * mm;
  G4double height_photocathode = 0.1 / 2.0 * mm;

  auto m_box_tube = new G4Box("MBox", pmt_size_x, pmt_size_y, pmt_size_z);
  auto m_box_a_tube = new G4Box("MBoxA", pmt_size_x - 5. * mm, pmt_size_y - 5. * mm, pmt_size_z - 5. * mm);

  auto aluminium_tube = new G4Tubs("Tube", 0. * mm, 102.5 * mm, 157. / 2. * mm, start_angle, spanning_angle);

  auto air_tube = new G4Tubs("AirTube", 0. * mm, 97.5 * mm, 157. / 2. * mm, start_angle, spanning_angle);

  // auto illuminator_tube = new G4Tubs("Illuminator", 0. * mm, 102.5 * mm, 8. / 2. * mm, start_angle, spanning_angle);  // a) 1/2 in water

  auto illuminator_tube = new G4Tubs("Illuminator", 0. * mm, 97.5 * mm, 16. / 2. * mm, start_angle, spanning_angle);  // b) 1 in water

  auto silicone_tube = new G4Tubs("Silicone", inner_rad_glass, outer_rad_glass, 3. / 2. * mm, start_angle, spanning_angle);

  auto glass_tube = new G4Tubs("Glass", inner_rad_glass, outer_rad_glass, height_glass, start_angle, spanning_angle);

  auto photocathode_tube =
      new G4Tubs("Photocathode", inner_rad_photocathode, outer_rad_photocathode, height_photocathode, start_angle, spanning_angle);

  //============================================================================
  // Buildings construction
  //============================================================================

  std::vector<PMTId> id_qsm;
  G4int pmt_count = 0;

  G4double pos_x, pos_y, pos_z, distance;
  G4ThreeVector position, null_position(0.0 * mm, 0.0 * mm, 0.0 * mm);

  G4int tube_config[PMT_PER_QSM][3] = {
      {0, 1, 0},
      {1, 0, 0},
      {0, -1, 0},
      {-1, 0, 0},
      {0, 0, -1},
      {0, 0, 1},
  };
  G4RotationMatrix* rot_matrices[PMT_PER_QSM];

  for (auto& i: rot_matrices)
    i = new G4RotationMatrix();

  rot_matrices[0]->rotateX(90.0 * deg);
  rot_matrices[1]->rotateY(270.0 * deg);
  rot_matrices[2]->rotateX(270.0 * deg);
  rot_matrices[3]->rotateY(90.0 * deg);
  rot_matrices[4]->rotateY(180.0 * deg);
  rot_matrices[5]->rotateX(0.0 * deg);

  auto m_box_log = initVector3D<G4LogicalVolume*>(cwd_plane_number_, 4, 4);
  auto m_box_a_log = initVector3D<G4LogicalVolume*>(cwd_plane_number_, 4, 4);
  auto aluminium_tube_log = initVector4D<G4LogicalVolume*>(cwd_plane_number_, 4, 4, PMT_PER_QSM);
  auto air_tube_log = initVector4D<G4LogicalVolume*>(cwd_plane_number_, 4, 4, PMT_PER_QSM);
  auto illuminator_log = initVector4D<G4LogicalVolume*>(cwd_plane_number_, 4, 4, PMT_PER_QSM);
  auto silicone_log = initVector4D<G4LogicalVolume*>(cwd_plane_number_, 4, 4, PMT_PER_QSM);
  auto glass_log = initVector4D<G4LogicalVolume*>(cwd_plane_number_, 4, 4, PMT_PER_QSM);
  auto photocathode_log = initVector4D<G4LogicalVolume*>(cwd_plane_number_, 4, 4, PMT_PER_QSM);

  auto m_box_phys = initVector3D<G4VPhysicalVolume*>(cwd_plane_number_, 4, 4);
  auto m_box_a_phys = initVector3D<G4VPhysicalVolume*>(cwd_plane_number_, 4, 4);
  auto aluminium_tube_phys = initVector4D<G4VPhysicalVolume*>(cwd_plane_number_, 4, 4, PMT_PER_QSM);
  auto air_tube_phys = initVector4D<G4VPhysicalVolume*>(cwd_plane_number_, 4, 4, PMT_PER_QSM);
  auto illuminator_phys = initVector4D<G4VPhysicalVolume*>(cwd_plane_number_, 4, 4, PMT_PER_QSM);
  auto silicone_phys = initVector4D<G4VPhysicalVolume*>(cwd_plane_number_, 4, 4, PMT_PER_QSM);
  auto glass_phys = initVector4D<G4VPhysicalVolume*>(cwd_plane_number_, 4, 4, PMT_PER_QSM);
  auto photocathode_phys = initVector4D<G4VPhysicalVolume*>(cwd_plane_number_, 4, 4, PMT_PER_QSM);

  for (size_t plane = 0; plane < cwd_plane_number_; plane++) {
    for (size_t stripe = 0; stripe < stride_config_[plane]; stripe++) {
      for (size_t module = 0; module < qsm_config_[plane]; module++) {
        if ((plane == 0) || (plane == 2) || (plane == 4) || (plane == 6)) {
          // for fours
          pos_x = -3.0 + 2.0 * stripe * m;
          pos_z = 3.0 - 2.0 * module * m;
        } else {
          // for threes
          pos_x = -2.0 + 2.0 * stripe * m;
          pos_z = 2.0 - 2.0 * module * m;
        }
        pos_y = -6.125 + 1.25 * plane * m;
        position = G4ThreeVector(pos_x, pos_y, pos_z);

        m_box_log[plane][stripe][module] = new G4LogicalVolume(m_box_tube, aluminium, "MBox");

        m_box_phys[plane][stripe][module] =
            new G4PVPlacement(nullptr, position, m_box_log[plane][stripe][module], "MBox", water_log, false, 0, check_overlaps_);

        m_box_a_log[plane][stripe][module] = new G4LogicalVolume(m_box_a_tube, air, "MBoxA");

        m_box_a_phys[plane][stripe][module] = new G4PVPlacement(
            nullptr, null_position, m_box_a_log[plane][stripe][module], "MBoxA", m_box_log[plane][stripe][module], false, 0, check_overlaps_);

        for (G4int i = 0; i < PMT_PER_QSM; i++) {
          distance = (120. + 157. / 2.) * mm;
          position = G4ThreeVector(distance * tube_config[i][0] + pos_x, distance * tube_config[i][1] + pos_y, distance * tube_config[i][2] + pos_z);
          aluminium_tube_log[plane][stripe][module][i] = new G4LogicalVolume(aluminium_tube, aluminium, "Tube");

          aluminium_tube_phys[plane][stripe][module][i] = new G4PVPlacement(
              rot_matrices[i], position, aluminium_tube_log[plane][stripe][module][i], "Tube", water_log, false, 0, check_overlaps_);

          air_tube_log[plane][stripe][module][i] = new G4LogicalVolume(air_tube, air, "AirTube");

          air_tube_phys[plane][stripe][module][i] = new G4PVPlacement(
              nullptr,
              null_position,
              air_tube_log[plane][stripe][module][i],
              "AirTube",
              aluminium_tube_log[plane][stripe][module][i],
              false,
              0,
              check_overlaps_);

          distance = (120. + 157. + 8. / 2.) * mm;
          position = G4ThreeVector(distance * tube_config[i][0] + pos_x, distance * tube_config[i][1] + pos_y, distance * tube_config[i][2] + pos_z);

          position = G4ThreeVector(0. * m, 0. * m, (157. / 2. - 16. / 2.) * mm);
          illuminator_log[plane][stripe][module][i] = new G4LogicalVolume(illuminator_tube, plexiglass, "Illuminator");
          illuminator_phys[plane][stripe][module][i] = new G4PVPlacement(
              nullptr,
              position,
              illuminator_log[plane][stripe][module][i],
              "Illuminator",
              air_tube_log[plane][stripe][module][i],
              false,
              0,
              check_overlaps_);

          silicone_log[plane][stripe][module][i] = new G4LogicalVolume(silicone_tube, silicone, "Silicone");

          silicone_phys[plane][stripe][module][i] = new G4PVPlacement(
              nullptr,
              G4ThreeVector(0. * mm, 0. * mm, (157. / 2. - 16. - 3. / 2.) * mm),
              silicone_log[plane][stripe][module][i],
              "Silicone",
              air_tube_log[plane][stripe][module][i],
              false,
              0,
              check_overlaps_);

          glass_log[plane][stripe][module][i] = new G4LogicalVolume(glass_tube, glass, "Glass");

          glass_phys[plane][stripe][module][i] = new G4PVPlacement(
              nullptr,
              G4ThreeVector(0. * mm, 0. * mm, (157. / 2. - 16. - 3. - 6. / 2.) * mm),
              glass_log[plane][stripe][module][i],
              "Glass",
              air_tube_log[plane][stripe][module][i],
              false,
              0,
              check_overlaps_);

          photocathode_log[plane][stripe][module][i] = new G4LogicalVolume(photocathode_tube, aluminium, "Photocathode");

          photocathode_phys[plane][stripe][module][i] = new G4PVPlacement(
              nullptr,
              G4ThreeVector(0. * mm, 0. * mm, (157. / 2. - 16. - 3. - 6. - 0.1 / 2.) * mm),

              photocathode_log[plane][stripe][module][i],
              "Photocathode",
              air_tube_log[plane][stripe][module][i],
              false,
              pmt_count,
              check_overlaps_);

          id_qsm[pmt_count].plane = plane;
          id_qsm[pmt_count].stripe = stripe;
          id_qsm[pmt_count].module = module;
          id_qsm[pmt_count].tube = i;

          pmt_count++;
        }
      }
    }
  }

  //============================================================================
  // Sending configuration to Communicator
  //============================================================================

  communicator_->SetCountPMT(pmt_count);
  communicator_->SetQSMId(id_qsm);

  //============================================================================
  // Optical surfaces
  //============================================================================

  G4OpticalSurface* optical_plexiglass_tube_surface;
  // G4OpticalSurface* optical_plexiglass_water_surface;

  optical_plexiglass_tube_surface = new G4OpticalSurface("OpPlGlassTubeSurf");
  optical_plexiglass_tube_surface->SetType(dielectric_metal);
  optical_plexiglass_tube_surface->SetFinish(polished);
  optical_plexiglass_tube_surface->SetModel(unified);

  auto glass_pmt_surface = initVector4D<G4LogicalBorderSurface*>(cwd_plane_number_, 4, 4, PMT_PER_QSM);
  // auto plexiglass_tube_surface = initVector4D<G4LogicalBorderSurface*>(cwd_plane_number_, 4, 4, PMT_PER_QSM);
  // auto plexiglass_water_surface = initVector4D<G4LogicalBorderSurface*>(cwd_plane_number_, 4, 4, PMT_PER_QSM);
  // auto plexiglass_glass_surface = initVector4D<G4LogicalBorderSurface*>(cwd_plane_number_, 4, 4, PMT_PER_QSM);
  // auto glass_tube_surface = initVector4D<G4LogicalBorderSurface*>(cwd_plane_number_, 4, 4, PMT_PER_QSM);
  // auto water_tube_surface = initVector4D<G4LogicalBorderSurface*>(cwd_plane_number_, 4, 4, PMT_PER_QSM);
  // auto water_m_box_surface = initVector4D<G4LogicalBorderSurface*>(cwd_plane_number_, 4, 4, PMT_PER_QSM);

  for (size_t plane = 0; plane < cwd_plane_number_; plane++) {
    for (size_t stripe = 0; stripe < stride_config_[plane]; stripe++) {
      for (size_t module = 0; module < qsm_config_[plane]; module++) {
        for (size_t i = 0; i < PMT_PER_QSM; i++) {
          glass_pmt_surface[plane][stripe][module][i] = new G4LogicalBorderSurface(
              "GlassPMTSurface", glass_phys[plane][stripe][module][i], photocathode_phys[plane][stripe][module][i], optical_plexiglass_tube_surface);
        }
      }
    }
  }
}

// DECOR
void DetectorConstruction::BuildDECOR(G4LogicalVolume* world_log) {
  //============================================================================
  // Materials
  //============================================================================

  auto nist_manager = G4NistManager::Instance();
  auto aluminium = nist_manager->FindOrBuildMaterial("Aluminium");

  //============================================================================
  // DECOR construction
  //============================================================================

  G4double short_sm_size_x[2] = {3430 * mm, 3120 * mm};
  G4double short_sm_size_y = 1 * nm;
  G4double short_sm_size_z = 2708 * mm;

  G4double long_sm_size_x = 1 * nm;
  G4double long_sm_size_y[2] = {3430 * mm, 3120 * mm};
  G4double long_sm_size_z = 2708 * mm;

  G4double sm_shift_x = 4.5 * m;
  G4double sm_shift_y = 13. * m;
  G4double sm_shift_z = (4.5 - 0.3) * m;

  G4double super_module_pos_x[DECOR_COUNT] = {
      (2310.38 - sm_shift_x) * mm,
      (6727.38 - sm_shift_x) * mm,
      (9960 - sm_shift_x) * mm,
      (9961 - sm_shift_x) * mm,
      (9958 - sm_shift_x) * mm,
      (9960 - sm_shift_x) * mm,
      (6748.62 - sm_shift_x) * mm,
      (2348.62 - sm_shift_x) * mm,
  };

  G4double super_module_pos_y[DECOR_COUNT] = {
      (-1011 - sm_shift_y) * mm,
      (-1011 - sm_shift_y) * mm,
      (3002.38 - sm_shift_y) * mm,
      (8115.38 - sm_shift_y) * mm,
      (13243.4 - sm_shift_y) * mm,
      (18362.4 - sm_shift_y) * mm,
      (27022 - sm_shift_y) * mm,
      (27022 - sm_shift_y) * mm,
  };

  G4double super_module_pos_z[DECOR_COUNT] = {
      (3926.62 - sm_shift_z) * mm,
      (3923.62 - sm_shift_z) * mm,
      (3920.62 - sm_shift_z) * mm,
      (3916.62 - sm_shift_z) * mm,
      (3914.62 - sm_shift_z) * mm,
      (3923.62 - sm_shift_z) * mm,
      (3916.62 - sm_shift_z) * mm,
      (3919.62 - sm_shift_z) * mm,
  };

  G4double size_x, size_y, size_z;
  G4double pos_x, pos_y, pos_z;
  G4String name;

  for (size_t super_module = 0; super_module < DECOR_COUNT; ++super_module) {
    for (size_t plane = 0; plane < DECOR_CHAMBER_COUNT; ++plane) {
      for (size_t i = 0; i < 2; ++i) {
        // check short or long SM
        if (super_module >= 2 && super_module <= 5) {
          size_x = long_sm_size_x;
          size_z = long_sm_size_z;

          pos_x = super_module_pos_x[super_module] - 3 * cm - 3 * 6 * cm + plane * 6 * cm;
          pos_y = super_module_pos_y[super_module];
          pos_z = super_module_pos_z[super_module];

          if (i == 0) {
            // X strips
            size_y = long_sm_size_y[0];
            pos_x -= 1 * nm;
            name = "SuperModule" + std::to_string(super_module) + "x";
          } else {
            // Y strips
            size_y = long_sm_size_y[1];
            pos_x += 1 * nm;
            name = "SuperModule" + std::to_string(super_module) + "y";
          }
        } else {
          size_y = short_sm_size_y;
          size_z = short_sm_size_z;

          pos_x = super_module_pos_x[super_module];
          pos_y = super_module_pos_y[super_module] - 3 * cm - 3 * 6 * cm + plane * 6 * cm;
          pos_z = super_module_pos_z[super_module];

          if (i == 0) {
            // X strips
            size_x = short_sm_size_x[0];
            pos_y -= 1 * nm;
            name = "SuperModule" + std::to_string(super_module) + "x";
          } else {
            // Y strips
            size_x = short_sm_size_x[1];
            pos_y += 1 * nm;
            name = "SuperModule" + std::to_string(super_module) + "y";
          }
        }
        super_module_box_[super_module][plane][i] = new G4Box(name, size_x / 2, size_y / 2, size_z / 2);
        super_module_log_[super_module][plane][i] = new G4LogicalVolume(super_module_box_[super_module][plane][i], aluminium, name);
        super_module_phys_[super_module][plane][i] = new G4PVPlacement(
            nullptr, G4ThreeVector(pos_x, pos_y, pos_z), super_module_log_[super_module][plane][i], name, world_log, false, plane, check_overlaps_);
      }
    }
  }
}

// SCT
void DetectorConstruction::BuildSCT(G4LogicalVolume* world_log, G4LogicalVolume* water_log) {
  //============================================================================
  // Materials
  //============================================================================

  auto nist_manager = G4NistManager::Instance();
  auto aluminium = nist_manager->FindOrBuildMaterial("Aluminium");
  auto air = nist_manager->FindOrBuildMaterial("Air");
  auto scintillator = nist_manager->FindOrBuildMaterial("Scintillator");

  //============================================================================
  // SCT construction
  //============================================================================

  std::pair<G4double, G4double> counter_size_x = {0.23 * m, 0.22 * m};
  std::pair<G4double, G4double> counter_size_y = {0.63 * m, 0.62 * m};
  std::pair<G4double, G4double> counter_size_z = {0.055 * m, 0.045 * m};
  G4double scint_size_x = 0.2 * m;
  G4double scint_size_y = 0.4 * m;
  G4double scint_size_z = 0.02 * m;
  G4double counter_pos_x, counter_pos_y;
  G4double counter_pos_z_upper = (9.6 / 2. + 0.005 + 0.055 / 2.) * m;
  G4double counter_pos_z_down = (-4.5 + 0.055 / 2. + 1.E-6) * m;

  G4int middle_id = (sct_plane_number_.first * sct_counter_number_.first + sct_plane_number_.second * sct_counter_number_.first);
  G4int counter_per_iter = sct_counter_number_.first + sct_counter_number_.second;

  G4int additional_counter_id;
  G4double counter_pos_z;

  G4int counter_id;
  std::vector<CounterId> id_sct;

  G4ThreeVector null_vector(0.0 * m, 0.0 * m, 0.0 * m);

  for (size_t side_count = 0; side_count < 2; ++side_count) {
    // top or bottom
    if (side_count == 0) {
      additional_counter_id = 0;
      counter_pos_z = counter_pos_z_upper;
    } else {
      additional_counter_id = middle_id;
      counter_pos_z = counter_pos_z_down;
    }

    for (size_t j = 0; j < sct_plane_number_.first; j++) {
      counter_id = counter_per_iter * j + additional_counter_id;
      for (size_t i = 0; i < sct_counter_number_.first; i++) {
        counter_pos_x = (-4.5 + 1.5 + 2. * i) * m;
        counter_pos_y = (-13.0 + 5.625 + 2.5 * j) * m;

        sct_counter_box_[counter_id].first = new G4Box("Counter", counter_size_x.first / 2., counter_size_y.first / 2., counter_size_z.first / 2.);
        sct_counter_log_[counter_id].first = new G4LogicalVolume(sct_counter_box_[counter_id].first, aluminium, "Counter");
        sct_counter_phys_[counter_id].first = new G4PVPlacement(
            nullptr,
            G4ThreeVector(counter_pos_x, counter_pos_y, counter_pos_z),
            sct_counter_log_[counter_id].first,
            "Counter",
            world_log,
            false,
            counter_id,
            check_overlaps_);

        sct_counter_box_[counter_id].second =
            new G4Box("CounterInner", counter_size_x.second / 2., counter_size_y.second / 2., counter_size_z.second / 2.);
        sct_counter_log_[counter_id].second = new G4LogicalVolume(sct_counter_box_[counter_id].second, air, "CounterInner");
        sct_counter_phys_[counter_id].second = new G4PVPlacement(
            nullptr,
            null_vector,
            sct_counter_log_[counter_id].second,
            "CounterInner",
            sct_counter_log_[counter_id].first,
            false,
            counter_id,
            check_overlaps_);

        sct_scint_box_[counter_id] = new G4Box("ScintillatorSCT", scint_size_x / 2., scint_size_y / 2., scint_size_z / 2.);
        sct_scint_log_[counter_id] = new G4LogicalVolume(sct_scint_box_[counter_id], scintillator, "ScintillatorSCT");
        sct_scint_phys_[counter_id] = new G4PVPlacement(
            nullptr,
            null_vector,
            sct_scint_log_[counter_id],
            "ScintillatorSCT",
            sct_counter_log_[counter_id].second,
            false,
            counter_id,
            check_overlaps_);

        id_sct[counter_id].side = side_count;
        id_sct[counter_id].plane = j;
        id_sct[counter_id].row = i;

        counter_id++;
      }

      for (size_t j = 0; j < sct_plane_number_.second; j++) {
        counter_id = sct_counter_number_.first + counter_per_iter * j + additional_counter_id;
        for (size_t i = 0; i < sct_counter_number_.second; i++) {
          counter_pos_x = (-4.5 + 0.5 + 2. * i) * m;
          counter_pos_y = (-13.0 + 6.875 + 2.5 * j) * m;

          sct_counter_box_[counter_id].first = new G4Box("Counter", counter_size_x.first / 2., counter_size_y.first / 2., counter_size_z.first / 2.);
          sct_counter_log_[counter_id].first = new G4LogicalVolume(sct_counter_box_[counter_id].first, aluminium, "Counter");
          sct_counter_phys_[counter_id].first = new G4PVPlacement(
              nullptr,
              G4ThreeVector(counter_pos_x, counter_pos_y, counter_pos_z),
              sct_counter_log_[counter_id].first,
              "Counter",
              world_log,
              false,
              counter_id,
              check_overlaps_);

          sct_counter_box_[counter_id].second =
              new G4Box("CounterInner", counter_size_x.second / 2., counter_size_y.second / 2., counter_size_z.second / 2.);
          sct_counter_log_[counter_id].second = new G4LogicalVolume(sct_counter_box_[counter_id].second, air, "CounterInner");
          sct_counter_phys_[counter_id].second = new G4PVPlacement(
              nullptr,
              null_vector,
              sct_counter_log_[counter_id].second,
              "CounterInner",
              sct_counter_log_[counter_id].first,
              false,
              counter_id,
              check_overlaps_);

          sct_scint_box_[counter_id] = new G4Box("ScintillatorSCT", scint_size_x / 2., scint_size_y / 2., scint_size_z / 2.);
          sct_scint_log_[counter_id] = new G4LogicalVolume(sct_scint_box_[counter_id], scintillator, "ScintillatorSCT");
          sct_scint_phys_[counter_id] = new G4PVPlacement(
              nullptr,
              null_vector,
              sct_scint_log_[counter_id],
              "ScintillatorSCT",
              sct_counter_log_[counter_id].second,
              false,
              counter_id,
              check_overlaps_);

          id_sct[counter_id].side = side_count;
          id_sct[counter_id].plane = j;
          id_sct[counter_id].row = i;

          counter_id++;
        }
      }
    }
  }

  //============================================================================
  // Sending configuration to Communicator
  //============================================================================

  communicator_->SetCountSCT(counter_id);
  communicator_->SetCounterId(id_sct);
}

// PRISMA-URAN
void DetectorConstruction::BuildPRISMA() {
  // // Floor (1-Ymin)
  // G4double floor1_sizeX = 10.2 * m;
  // G4double floor1_sizeY = 1.3 * m;
  // G4double floor1_sizeZ = 0.2 * m;
  // auto solidFloor1 = new G4Box(
  //     "Floor",  // its name
  //     0.5 * floor1_sizeX,
  //     0.5 * floor1_sizeY,
  //     0.5 * floor1_sizeZ);  // its size

  // auto logicFloor1 = new G4LogicalVolume(
  //     solidFloor1,  // its solid
  //     floor_mat,    // its material
  //     "Floor");     // its name

  // G4double floor1_posX = 0. * m;
  // G4double floor1_posY = (-15.175 + 0.55 / 2. + 1.3 / 2.) * m;
  // G4double floor1_posZ = (9.6 / 2. - 0.2 / 2. + 0.005) * m;
  // G4VPhysicalVolume* physFloor1 = new G4PVPlacement(
  //     nullptr,
  //     G4ThreeVector(floor1_posX, floor1_posY, floor1_posZ),
  //     // logicFloor1,     // its logical volume "Floor",         // its name
  //     world_log,  // its mother  volume
  //     false,      // no boolean operation
  //     0,
  //     check_overlaps_);

  // // Floor (2-Ymax)
  // G4double floor2_posY = (15.175 - 0.55 / 2. - 1.3 / 2.) * m;
  // auto solidFloor2 = new G4Box(
  //     "Floor",  // its name
  //     0.5 * floor1_sizeX,
  //     0.5 * floor1_sizeY,
  //     0.5 * floor1_sizeZ);  // its size
  // auto logicFloor2 = new G4LogicalVolume(
  //     solidFloor2,  // its solid
  //     floor_mat,    // its material
  //     "Floor");     // its name
  // G4VPhysicalVolume* physFloor2 = new G4PVPlacement(
  //     nullptr,
  //     G4ThreeVector(floor1_posX, floor2_posY, floor1_posZ),
  //     // logicFloor2,     // its logical volume "Floor",         // its name
  //     world_log,  // its mother  volume
  //     false,      // no boolean operation
  //     0,
  //     check_overlaps_);

  // // Floor (3-Xmin) devide on 5 part
  // G4double floor31_sizeX = (5.55 - 4.15) * m;
  // G4double floor31_sizeY = (5.9) * m;
  // auto solidFloor31 = new G4Box(
  //     "Floor",  // its name
  //     0.5 * floor31_sizeX,
  //     0.5 * floor31_sizeY,
  //     0.5 * floor1_sizeZ);  // its size

  // auto logicFloor31 = new G4LogicalVolume(
  //     solidFloor31,  // its solid
  //     floor_mat,     // its material
  //     "Floor");      // its name

  // G4double floor31_posX = (-10.925 + 0.55 / 2. + 4.15 + (5.55 - 4.15) / 2.) * m;
  // G4double floor31_posY = (29.8 / 2. - 5.9 / 2.) * m;
  // G4VPhysicalVolume* physFloor31 = new G4PVPlacement(
  //     nullptr,
  //     G4ThreeVector(floor31_posX, floor31_posY, floor1_posZ),  //
  //     logicFloor31,                                            // its logical volume
  //     "Floor",                                                 // its name
  //     world_log,                                               // its mother  volume
  //     false,                                                   // no boolean operation
  //     0,
  //     check_overlaps_);

  // G4double floor32_sizeX = (5.55 - 4.15) * m;
  // G4double floor32_sizeY = (5.9) * m;
  // auto solidFloor32 = new G4Box(
  //     "Floor",  // its name
  //     0.5 * floor32_sizeX,
  //     0.5 * floor32_sizeY,
  //     0.5 * floor1_sizeZ);  // its size

  // auto logicFloor32 = new G4LogicalVolume(
  //     solidFloor32,  // its solid
  //     floor_mat,     // its material
  //     "Floor");      // its name

  // G4double floor32_posX = (-10.925 + 0.55 / 2. + 4.15 + (5.55 - 4.15) / 2.) * m;
  // G4double floor32_posY = (-29.8 / 2. + 5.9 / 2.) * m;
  // G4VPhysicalVolume* physFloor32 = new G4PVPlacement(
  //     nullptr,
  //     G4ThreeVector(floor32_posX, floor32_posY, floor1_posZ),  //
  //     logicFloor32,                                            // its logical volume
  //     "Floor",                                                 // its name
  //     world_log,                                               // its mother  volume
  //     false,                                                   // no boolean operation
  //     0,
  //     check_overlaps_);

  // G4double floor33_sizeX = (5.55) * m;
  // G4double floor33_sizeY = (18) * m;
  // auto solidFloor33 = new G4Box(
  //     "Floor",  // its name
  //     0.5 * floor33_sizeX,
  //     0.5 * floor33_sizeY,
  //     0.5 * floor1_sizeZ);  // its size

  // auto logicFloor33 = new G4LogicalVolume(
  //     solidFloor33,  // its solid
  //     floor_mat,     // its material
  //     "Floor");      // its name

  // G4double floor33_posX = (-10.925 + 0.55 / 2. + 5.55 / 2.) * m;
  // G4double floor33_posY = 0 * m;
  // G4VPhysicalVolume* physFloor33 = new G4PVPlacement(
  //     nullptr,
  //     G4ThreeVector(floor33_posX, floor33_posY, floor1_posZ),  //
  //     logicFloor33,                                            // its logical volume
  //     "Floor",                                                 // its name
  //     world_log,                                               // its mother  volume
  //     false,                                                   // no boolean operation
  //     0,
  //     check_overlaps_);

  // G4double floor34_sizeX = (4.15) * m;
  // G4double floor34_sizeY = (5.9) * m;
  // G4double floor3_sizeZ = (0.02) * m;
  // auto solidFloor34 = new G4Box(
  //     "Floor",  // its name
  //     0.5 * floor34_sizeX,
  //     0.5 * floor34_sizeY,
  //     0.5 * floor3_sizeZ);  // its size

  // auto logicFloor34 = new G4LogicalVolume(
  //     solidFloor34,  // its solid
  //     floor_mat2,    // its material
  //     "Floor");      // its name

  // G4double floor34_posX = (-10.925 + 0.55 / 2. + 4.15 / 2.) * m;
  // G4double floor34_posY = (29.8 / 2. - 5.9 / 2.) * m;
  // G4double floor34_posZ = (9.6 / 2. - 0.2 / 2. + 0.005 - 0.18) * m;
  // G4VPhysicalVolume* physFloor34 = new G4PVPlacement(
  //     nullptr,
  //     G4ThreeVector(floor34_posX, floor34_posY, floor34_posZ),  //
  //     logicFloor34,                                             // its logical volume
  //     "Floor",                                                  // its name
  //     world_log,                                                // its mother  volume
  //     false,                                                    // no boolean operation
  //     0,
  //     check_overlaps_);

  // G4double floor35_sizeX = (4.15) * m;
  // G4double floor35_sizeY = (5.9) * m;

  // auto solidFloor35 = new G4Box(
  //     "Floor",  // its name
  //     0.5 * floor35_sizeX,
  //     0.5 * floor35_sizeY,
  //     0.5 * floor3_sizeZ);  // its size

  // auto logicFloor35 = new G4LogicalVolume(
  //     solidFloor35,  // its solid
  //     floor_mat2,    // its material
  //     "Floor");      // its name

  // G4double floor35_posX = (-10.925 + 0.55 / 2. + 4.15 / 2.) * m;
  // G4double floor35_posY = (-29.8 / 2. + 5.9 / 2.) * m;

  // G4VPhysicalVolume* physFloor35 = new G4PVPlacement(
  //     nullptr,
  //     G4ThreeVector(floor35_posX, floor35_posY, floor34_posZ),  //
  //     logicFloor35,                                             // its logical volume
  //     "Floor",                                                  // its name
  //     world_log,                                                // its mother  volume
  //     false,                                                    // no boolean operation
  //     0,
  //     check_overlaps_);

  // // Floor (4-Xmax)
  // G4double floor4_sizeX = 1.1 * m;
  // G4double floor4_sizeY = 29.8 * m;
  // auto solidFloor4 = new G4Box(
  //     "Floor",  // its name
  //     0.5 * floor4_sizeX,
  //     0.5 * floor4_sizeY,
  //     0.5 * floor1_sizeZ);  // its size

  // auto logicFloor4 = new G4LogicalVolume(
  //     solidFloor4,  // its solid
  //     floor_mat,    // its material
  //     "Floor");     // its name

  // G4double floor4_posX = (6.475 - 0.55 / 2. - 1.1 / 2.) * m;
  // G4double floor4_posY = 0 * m;
  // G4VPhysicalVolume* physFloor4 = new G4PVPlacement(
  //     nullptr,
  //     G4ThreeVector(floor4_posX, floor4_posY, floor1_posZ),
  //     // logicFloor4,     // its logical volume "Floor",         // its name
  //     world_log,  // its mother  volume
  //     false,      // no boolean operation
  //     0,
  //     check_overlaps_);
  // //-----

  // // Material Prisma

  // auto Zn = nist_manager->FindOrBuildElement("Zn");
  // auto S = nist_manager->FindOrBuildElement("S");
  // auto Ag = nist_manager->FindOrBuildElement("Ag");

  // auto ZnS = new G4Material("ZnS", 4.09 * g / cm3, 3, kStateSolid, 293 * kelvin, 1 * atmosphere);
  // ZnS->AddElement(Zn, 49.995 * perCent);
  // ZnS->AddElement(S, 49.995 * perCent);
  // ZnS->AddElement(Ag, 0.01 * perCent);

  // auto Li6 = new G4Isotope("Li6", 3, 6, 6.02 * g / mole);
  // auto Li7 = new G4Isotope("Li7", 3, 7, 7.02 * g / mole);
  // auto Li = new G4Element("Li", "Li", 2);
  // Li->AddIsotope(Li6, 90.000 * perCent);
  // Li->AddIsotope(Li7, 10.000 * perCent);

  // auto F = nist_manager->FindOrBuildElement("F");
  // auto LiF = new G4Material("LiF", 2.64 * g / cm3, 2, kStateSolid, 293 * kelvin, 1 * atmosphere);
  // LiF->AddElement(Li, 1);
  // LiF->AddElement(F, 1);

  // auto LiF_ZnS = new G4Material("LiF_ZnS", 3.51 * g / cm3, 2, kStateSolid, 293 * kelvin, 1 * atmosphere);
  // LiF_ZnS->AddMaterial(LiF, 40 * perCent);
  // LiF_ZnS->AddMaterial(ZnS, 60 * perCent);

  // auto B1 = nist_manager->FindOrBuildElement("B");
  // auto B2O3 = new G4Material("B2O3", 2.4 * g / cm3, 2, kStateSolid, 293 * kelvin, 1 * atmosphere);
  // B2O3->AddElement(B1, 2);
  // B2O3->AddElement(O, 3);

  // auto B2O3_ZnS = new G4Material("B2O3_ZnS", 3.53 * g / cm3, 2, kStateSolid, 293 * kelvin, 1 * atmosphere);
  // B2O3_ZnS->AddMaterial(B2O3, 33 * perCent);
  // B2O3_ZnS->AddMaterial(ZnS, 67 * perCent);

  // auto matTarget = nist_manager->FindOrBuildMaterial("G4_POLYETHYLENE");  // !!!###
  // auto matModerate = nist_manager->FindOrBuildMaterial("G4_Pb");          // !!!###

  // // Detector(en)
  // G4double posXPr[32] = {-9.993 * m,  -7.633 * m, -4.783 * m, -9.883 * m,        -7.633 * m, -4.783 * m, -9.703 * m, -7.383 * m,
  //                        -4.783 * m,  -7.383 * m, -4.783 * m, -7.383 * m,        -4.783 * m, -9.963 * m, -7.383 * m, -4.783 * m,
  //                        -10.621 * m, -8.011 * m, -4.783 * m, -2.411 * m,        0.409 * m,  2.369 * m,  4.979 * m,  4.979 * m,
  //                        4.979 * m,   4.979 * m,  4.979 * m,  (5.779 - 0.3) * m, 4.979 * m,  2.317 * m,  0.347 * m,  -1.653 * m};

  // G4double posYPr[32] = {13.902 * m,          13.902 * m,          13.919 * m,          7.922 * m,           7.922 * m,           8.949 * m,
  //                        5.582 * m,           5.582 * m,           4.399 * m,           -1.118 * m,          -0.961 * m,          -3.778 * m,
  //                        -4.481 * m,          -9.868 * m,          -9.878 * m,          -9.691 * m,          (-15.118 + 0.1) * m, (-15.118 + 0.1)
  //                        * m,
  //                        (-15.118 + 0.1) * m, (-15.118 + 0.1) * m, (-15.118 + 0.1) * m, (-15.118 + 0.1) * m, (-15.118 + 0.1) * m, -9.848 * m,
  //                        -4.758 * m,          -1.168 * m,          3.512 * m,           10.032 * m,          13.919 * m,          13.919 * m,
  //                        13.919 * m,          13.919 * m};

  // G4double posYU[72] = {(14.32) * m, (14.32) * m,  (14.32) * m,  (14.32) * m,  (10.745) * m, (10.74) * m,  (10.74) * m,  (10.745) * m, (7.16) *
  // m,
  //                       (7.16) * m,  (7.16) * m,   (7.16) * m,   (3.58) * m,   (3.58) * m,   (3.58) * m,   (3.58) * m,   0. * m,       0. * m,
  //                       0. * m,      0. * m,       (-3.58) * m,  (-3.58) * m,  (-3.58) * m,  (-3.58) * m,  (-7.16) * m,  (-7.16) * m,  (-7.16) *
  //                       m,
  //                       (-7.16) * m, (-10.74) * m, (-10.74) * m, (-10.74) * m, (-10.74) * m, (-14.32) * m, (-14.32) * m, (-14.32) * m, (-14.32) *
  //                       m, 25.981 * m,  25.981 * m,   25.981 * m,   25.981 * m,   19.881 * m,   19.881 * m,   19.881 * m,   19.881 * m,   13.639
  //                       * m, 13.639 * m,  13.639 * m,   13.639 * m,   7.539 * m,    7.539 * m,    7.539 * m,    7.539 * m,    1.439 * m,    1.439
  //                       * m, 1.439 * m,   0.499 * m,    -4.661 * m,   -4.661 * m,   -4.661 * m,   -4.661 * m,   -10.761 * m,  -10.761 * m,
  //                       -10.761 * m, -10.761 * m, -16.861 * m,  -16.861 * m,  -16.861 * m,  -17.131 * m,  -22.961 * m,  -22.961 * m,  -22.961 *
  //                       m,  -22.961 * m};

  // G4double posXU[72] = {7.880 * m,   2.430 * m,   -2.980 * m,  -8.350 * m,  7.880 * m,   2.430 * m,   -2.980 * m,  -8.350 * m,  7.880 * m,
  //                       2.430 * m,   -2.980 * m,  -8.350 * m,  7.880 * m,   2.430 * m,   -2.980 * m,  -8.350 * m,  7.880 * m,   2.430 * m,
  //                       -2.980 * m,  -8.350 * m,  7.880 * m,   2.430 * m,   -2.980 * m,  -8.350 * m,  7.880 * m,   2.430 * m,   -2.980 * m,
  //                       -8.350 * m,  7.880 * m,   2.430 * m,   -2.980 * m,  -8.350 * m,  7.880 * m,   2.430 * m,   -2.980 * m,  -8.350 * m,
  //                       -24.880 * m, -29.290 * m, -33.700 * m, -38.110 * m, -24.880 * m, -29.290 * m, -33.700 * m, -38.110 * m, -24.880 * m,
  //                       -29.290 * m, -33.700 * m, -36.570 * m, -24.880 * m, -29.290 * m, -33.700 * m, -38.110 * m, -24.880 * m, -29.290 * m,
  //                       -33.700 * m, -38.110 * m, -24.880 * m, -29.290 * m, -33.700 * m, -38.110 * m, -24.880 * m, -29.290 * m, -33.700 * m,
  //                       -38.110 * m, -24.880 * m, -29.290 * m, -33.700 * m, -38.110 * m, -24.880 * m, -29.290 * m, -33.700 * m, -38.110 * m};

  // for (G4int i = 0; i < 32; i++) {
  //   posXPr[i] = posXPr[i] + 0.345 * m;
  //   posYPr[i] = posYPr[i] + 0.5 * m;
  // }

  // for (G4int i = 0; i < 72; i++) {
  //   posXU[i] = posXU[i] - 1.8 * m;
  //   posYU[i] = posYU[i] - 0.3 * m;
  // }

  // for (G4int i = 35; i < 72; i++) {
  //   posYU[i] = posYU[i] + 13.2 * m;
  // }

  // G4double posZPr = (9.6 / 2. + 0.005 + 0.2 + 0.0001 / 2. + 0.005 + 0.005) * m;  // over the cover
  // G4double posZU = (11.8 + 0.1 + 0.0001 / 2. + 0.005) * m;                       // over the roof

  // G4double posZPrBar = (9.6 / 2. + 0.005 + 0.2 + 0.005 + 0.285) * m;  // over the cover
  // G4double posZUBar = (11.8 + 0.1 + 0.005 + 0.28) * m;                // over the roof

  // G4double posZPrBarDown = (9.6 / 2. + 0.005 + 0.2 + 0.005 + 0.005 / 2.) * m;  // over the cover
  // G4double posZUBarDown = (11.8 + 0.1 + 0.005 / 2.) * m;                       // over the roof

  // G4double posZPrBarUp = (9.6 / 2. + 0.005 + 0.2 + 0.005 + 0.005 + 0.56 + 0.005 / 2.) * m;  // over the cover
  // G4double posZUBarUp = (11.8 + 0.1 + 0.005 + 0.56 + 0.005 / 2.) * m;                       // over the roof

  // for (G4int i = 0; i < 32; i++)  // PRISMA
  // {
  //   solidDet[i] = new G4Tubs("DetPr", 0., 34. * cm, 0.5 * 0.1 * mm, 0.,
  //                            2. * M_PI);  // 0.1*mm
  //   logicDet[i] = new G4LogicalVolume(solidDet[i], LiF_ZnS, "DetPr");
  //   physDet[i] = new G4PVPlacement(nullptr, G4ThreeVector(posXPr[i], posYPr[i], posZPr), logicDet[i], "DetPr", world_log, false, i,
  //   check_overlaps_);

  //   solidBar1[i] = new G4Tubs("BarrelPr", 36.5 * cm, 37. * cm, 28 * cm, 0., 2. * M_PI);
  //   logicBar1[i] = new G4LogicalVolume(solidBar1[i], bar_mat, "BarrelPr");
  //   physBar1[i] =
  //       new G4PVPlacement(nullptr, G4ThreeVector(posXPr[i], posYPr[i], posZPrBar), logicBar1[i], "BarrelPr", world_log, false, i,
  //       check_overlaps_);

  //   solidBarDown[i] = new G4Tubs("BarDown", 0., 37. * cm, 0.5 * 5 * mm, 0., 2. * M_PI);  //
  //   logicBarDown[i] = new G4LogicalVolume(solidBarDown[i], bar_mat, "BarDown");
  //   physBarDown[i] = new G4PVPlacement(
  //       nullptr, G4ThreeVector(posXPr[i], posYPr[i], posZPrBarDown), logicBarDown[i], "BarDown", world_log, false, i, check_overlaps_);

  //   solidBarUp[i] = new G4Tubs("BarUp", 0., 37. * cm, 0.5 * 5 * mm, 0., 2. * M_PI);  //
  //   logicBarUp[i] = new G4LogicalVolume(solidBarUp[i], bar_mat, "BarUp");
  //   physBarUp[i] =
  //       new G4PVPlacement(nullptr, G4ThreeVector(posXPr[i], posYPr[i], posZPrBarUp), logicBarUp[i], "BarUp", world_log, false, i,
  //       check_overlaps_);
  // }

  // for (G4int i = 0; i < 72; i++)  // URAN
  // {
  //   solidDet2[i] = new G4Tubs("DetU", 0., 34. * cm, 0.5 * 0.1 * mm, 0., 2. * M_PI);
  //   logicDet2[i] = new G4LogicalVolume(solidDet2[i], B2O3_ZnS, "DetU");
  //   physDet2[i] = new G4PVPlacement(nullptr, G4ThreeVector(posXU[i], posYU[i], posZU), logicDet2[i], "DetU", world_log, false, i,
  //   check_overlaps_);

  //   solidBar2[i] = new G4Tubs("BarrelU", 36.5 * cm, 37. * cm, 28. * cm, 0., 2. * M_PI);
  //   logicBar2[i] = new G4LogicalVolume(solidBar2[i], bar_mat, "BarrelU");
  //   physBar2[i] =
  //       new G4PVPlacement(nullptr, G4ThreeVector(posXU[i], posYU[i], posZUBar), logicBar2[i], "BarrelU", world_log, false, i, check_overlaps_);

  //   solidBarDown2[i] = new G4Tubs("BarDown2", 0., 37. * cm, 2.5 * mm, 0., 2. * M_PI);  //
  //   logicBarDown2[i] = new G4LogicalVolume(solidBarDown2[i], bar_mat, "BarDown2");
  //   physBarDown2[i] = new G4PVPlacement(
  //       nullptr, G4ThreeVector(posXU[i], posYU[i], posZUBarDown), logicBarDown2[i], "BarDown2", world_log, false, i, check_overlaps_);

  //   solidBarUp2[i] = new G4Tubs("BarUp2", 0., 37. * cm, 2.5 * mm, 0., 2. * M_PI);  //
  //   logicBarUp2[i] = new G4LogicalVolume(solidBarUp2[i], bar_mat, "BarUp2");
  //   physBarUp2[i] =
  //       new G4PVPlacement(nullptr, G4ThreeVector(posXU[i], posYU[i], posZUBarUp), logicBarUp2[i], "BarUp2", world_log, false, i,
  //       check_overlaps_);
  // }
}
void DetectorConstruction::BuildURAN() {}

// NEVOD-EAS
void DetectorConstruction::BuildEAS() {
  // G4double csc[9][4][3];
  // csc[0][0][0] = -25.36;
  // csc[0][0][1] = 5.89;
  // csc[0][0][2] = -6.68;
  // csc[0][1][0] = -37.61;
  // csc[0][1][1] = 5.89;
  // csc[0][1][2] = -6.68;
  // csc[0][2][0] = -37.61;
  // csc[0][2][1] = -7.32;
  // csc[0][2][2] = -6.68;
  // csc[0][3][0] = -25.36;
  // csc[0][3][1] = -7.32;
  // csc[0][3][2] = -6.68;

  // csc[1][0][0] = -25.36;
  // csc[1][0][1] = 37.34;
  // csc[1][0][2] = -6.68;
  // csc[1][1][0] = -37.61;
  // csc[1][1][1] = 37.34;
  // csc[1][1][2] = -6.68;
  // csc[1][2][0] = -37.61;
  // csc[1][2][1] = 24.14;
  // csc[1][2][2] = -6.68;
  // csc[1][3][0] = -25.36;
  // csc[1][3][1] = 24.14;
  // csc[1][3][2] = -6.68;

  // csc[2][0][0] = 6.48;
  // csc[2][0][1] = 12.65;
  // csc[2][0][2] = 0.00;
  // csc[2][1][0] = -6.87;
  // csc[2][1][1] = 12.65;
  // csc[2][1][2] = 0.00;
  // csc[2][2][0] = -6.87;
  // csc[2][2][1] = -12.63;
  // csc[2][2][2] = 0.00;
  // csc[2][3][0] = 6.48;
  // csc[2][3][1] = -12.63;
  // csc[2][3][2] = 0.00;

  // csc[3][0][0] = 37.37;
  // csc[3][0][1] = 10.96;
  // csc[3][0][2] = -14.95;
  // csc[3][1][0] = 22.47;
  // csc[3][1][1] = 10.96;
  // csc[3][1][2] = -15.27;
  // csc[3][2][0] = 22.47;
  // csc[3][2][1] = -3.95;
  // csc[3][2][2] = -15.35;
  // csc[3][3][0] = 37.37;
  // csc[3][3][1] = -3.95;
  // csc[3][3][2] = -15.08;

  // csc[4][0][0] = 37.37;
  // csc[4][0][1] = 45.57;
  // csc[4][0][2] = -16.46;
  // csc[4][1][0] = 22.98;
  // csc[4][1][1] = 45.57;
  // csc[4][1][2] = -16.18;
  // csc[4][2][0] = 22.98;
  // csc[4][2][1] = 28.48;
  // csc[4][2][2] = -16.52;
  // csc[4][3][0] = 37.37;
  // csc[4][3][1] = 28.48;
  // csc[4][3][2] = -16.52;

  // csc[5][0][0] = 6.92;  // 6.91
  // csc[5][0][1] = -56.16;
  // csc[5][0][2] = -16.15;
  // csc[5][1][0] = -2.79;
  // csc[5][1][1] = -56.16;
  // csc[5][1][2] = -15.91;
  // csc[5][2][0] = -2.79;
  // csc[5][2][1] = -70.54;
  // csc[5][2][2] = -15.93;
  // csc[5][3][0] = 6.92;  // 6.91
  // csc[5][3][1] = -70.54;
  // csc[5][3][2] = -16.10;

  // csc[6][0][0] = -9.54;
  // csc[6][0][1] = 53.62;
  // csc[6][0][2] = -17.03;
  // csc[6][1][0] = -20.56;
  // csc[6][1][1] = 64.71;
  // csc[6][1][2] = -17.80;
  // csc[6][2][0] = -20.56;
  // csc[6][2][1] = 53.62;
  // csc[6][2][2] = -17.50;
  // csc[6][3][0] = -9.54;
  // csc[6][3][1] = 42.54;
  // csc[6][3][2] = -16.93;

  // csc[7][0][0] = 42.14;
  // csc[7][0][1] = -16.80;
  // csc[7][0][2] = -15.14;
  // csc[7][1][0] = 26.94;
  // csc[7][1][1] = -16.80;
  // csc[7][1][2] = -15.46;
  // csc[7][2][0] = 26.94;
  // csc[7][2][1] = -32.10;
  // csc[7][2][2] = -15.46;
  // csc[7][3][0] = 42.14;
  // csc[7][3][1] = -32.10;
  // csc[7][3][2] = -14.69;

  // csc[8][0][0] = -25.36;
  // csc[8][0][1] = -25.75;
  // csc[8][0][2] = -7.33;
  // csc[8][1][0] = -37.26;
  // csc[8][1][1] = -25.75;
  // csc[8][1][2] = -7.36;
  // csc[8][2][0] = -37.26;
  // csc[8][2][1] = -39.55;
  // csc[8][2][2] = -7.34;
  // csc[8][3][0] = -25.36;
  // csc[8][3][1] = -39.55;
  // csc[8][3][2] = -7.28;

  // G4int icl = 0;
  // G4int ist = 0;
  // G4int idet = 0;
  // G4double dX, dY;
  // G4double RoofNEdx1, RoofNEdx2, RoofNEdy1, RoofNEdy2, RoofNEdz;
  // G4double RoofNEposX, RoofNEposY, RoofNEposZ;
  // G4double AirRoofNEdx1, AirRoofNEdx2, AirRoofNEdy1, AirRoofNEdy2, AirRoofNEdz;
  // G4double FloorNEsizeX, FloorNEsizeY, FloorNEsizeZ;
  // G4double FloorNEposX, FloorNEposY, FloorNEposZ;
  // G4double CaseNEsizeX, CaseNEsizeY, CaseNEsizeZ;
  // G4double CaseNEposX, CaseNEposY, CaseNEposZ;
  // G4double AirCaseNEsizeX, AirCaseNEsizeY, AirCaseNEsizeZ;
  // G4double AirCaseNEposX, AirCaseNEposY, AirCaseNEposZ;
  // G4double ScintNEsizeX, ScintNEsizeY, ScintNEsizeZ;
  // G4double ScintNEposX, ScintNEposY, ScintNEposZ;

  // G4double WallStatNEXsizeX, WallStatNEXsizeY, WallStatNEYsizeX, WallStatNEYsizeY, WallStatNEsizeZ;
  // G4double WallStatNEXposX, WallStatNEXposY1, WallStatNEXposY2, WallStatNEposZ;
  // G4double WallStatNEYposX1, WallStatNEYposX2, WallStatNEYposY;

  // RoofNEdz = 100.;
  // RoofNEdx1 = 225.;
  // RoofNEdx2 = 0.1;
  // RoofNEdy1 = 190.;
  // RoofNEdy2 = 190.;

  // AirRoofNEdz = 99.85;
  // AirRoofNEdx1 = 224.7;
  // AirRoofNEdx2 = 0.1;
  // AirRoofNEdy1 = 190.;
  // AirRoofNEdy2 = 190.;

  // WallStatNEXsizeX = 189.8;
  // WallStatNEXsizeY = 0.1;
  // WallStatNEYsizeX = 0.1;
  // WallStatNEYsizeY = 190.;
  // WallStatNEsizeZ = 50.;

  // FloorNEsizeX = 20.;
  // FloorNEsizeY = 25.;
  // FloorNEsizeZ = 3.;

  // ScintNEsizeX = 80.;
  // ScintNEsizeY = 80.;
  // ScintNEsizeZ = 4.;
  // CaseNEsizeX = 85.;
  // CaseNEsizeY = 85.;
  // CaseNEsizeZ = 15.;
  // AirCaseNEsizeX = 84.9;
  // AirCaseNEsizeY = 84.9;
  // AirCaseNEsizeZ = 14.9;

  // for (int ncl = 0; ncl < 9; ncl++)  // 7
  // {
  //   if (ncl == 3 || ncl == 4 || ncl == 5 || ncl == 6 || ncl == 7) {
  //     if (ncl == 3) {
  //       FloorNEposX = 37.37 - (37.37 - 22.47) / 2. - 4.45 / 2.;
  //       FloorNEposY = 10.96 - (10.96 + 3.95) / 2.;
  //       FloorNEposZ = -15.35 + 11.8 - FloorNEsizeZ / 2.;
  //     }
  //     if (ncl == 4) {
  //       FloorNEposX = 37.37 - (37.37 - 22.98) / 2. - 4.45 / 2.;
  //       FloorNEposY = 45.57 - (45.57 - 28.48) / 2.;
  //       FloorNEposZ = -16.52 + 11.8 - FloorNEsizeZ / 2.;
  //     }
  //     if (ncl == 5) {
  //       FloorNEposX = 6.92 - (6.92 + 2.79) / 2. - 4.45 / 2.;  // 6.91
  //       FloorNEposY = -56.16 - (70.54 - 56.16) / 2.;
  //       FloorNEposZ = -16.15 + 11.8 - FloorNEsizeZ / 2.;
  //     }
  //     if (ncl == 6) {
  //       FloorNEposX = -9.54 - (20.56 - 9.54) / 2. - 4.45 / 2.;
  //       FloorNEposY = 64.71 - (64.71 - 42.54) / 2.;
  //       FloorNEposZ = -17.80 + 11.8 - FloorNEsizeZ / 2.;
  //     }
  //     if (ncl == 7) {
  //       FloorNEposX = 42.14 - (42.14 - 26.94) / 2. - 4.45 / 2.;
  //       FloorNEposY = -32.10 + (32.10 - 16.80) / 2.;
  //       FloorNEposZ = -15.46 + 11.8 - FloorNEsizeZ / 2.;
  //     }

  //     boxFloorNE[icl] = new G4Box("boxFloorNE", FloorNEsizeX / 2. * m, FloorNEsizeY / 2. * m, FloorNEsizeZ / 2. * m);
  //     logFloorNE[icl] = new G4LogicalVolume(boxFloorNE[icl], Air, "logFloorNE");
  //     physFloorNE[icl] = new G4PVPlacement(
  //         nullptr,
  //         G4ThreeVector(FloorNEposX * m, FloorNEposY * m, FloorNEposZ * m),
  //         logFloorNE[icl],
  //         "physFloorNE",
  //         world_log,
  //         false,
  //         icl,
  //         check_overlaps_);

  //     icl = icl + 1;
  //   }

  //   for (int nst = 0; nst < 4; nst++) {
  //     RoofNEposX = csc[ncl][nst][0] - 4.45 / 2.;
  //     RoofNEposY = csc[ncl][nst][1];
  //     RoofNEposZ = csc[ncl][nst][2] + 11.8 + 0.2 + 0.5 + 0.1 + 1. / 2.;  //??? 47b+legsNEdet+caseNEdet+gap+roofNEdet/2

  //     trdRoofNE[ist] = new G4Trd("trdRoofNE", RoofNEdx1 / 2. * cm, RoofNEdx2 / 2. * cm, RoofNEdy1 / 2. * cm, RoofNEdy2 / 2. * cm, RoofNEdz / 2. *
  //     cm); logRoofNE[ist] = new G4LogicalVolume(trdRoofNE[ist], ferrum, "logRoofNE"); physRoofNE[ist] = new G4PVPlacement(
  //         nullptr,
  //         G4ThreeVector(RoofNEposX * m, RoofNEposY * m, RoofNEposZ * m),
  //         logRoofNE[ist],
  //         "physRoofNE",
  //         world_log,
  //         false,
  //         ist,
  //         check_overlaps_);

  //     trdAirRoofNE[ist] = new G4Trd(
  //         "trdAirRoofNE", AirRoofNEdx1 / 2. * cm, AirRoofNEdx2 / 2. * cm, AirRoofNEdy1 / 2. * cm, AirRoofNEdy2 / 2. * cm, AirRoofNEdz / 2. * cm);
  //     logAirRoofNE[ist] = new G4LogicalVolume(trdAirRoofNE[ist], Air, "logAirRoofNE");
  //     physAirRoofNE[ist] = new G4PVPlacement(
  //         nullptr,
  //         G4ThreeVector(0. * m, 0. * m, (AirRoofNEdz - RoofNEdz) / 2. * cm),
  //         logAirRoofNE[ist],
  //         "physAirRoofNE",
  //         logRoofNE[ist],
  //         false,
  //         ist,
  //         check_overlaps_);

  //     WallStatNEXposX = csc[ncl][nst][0] - 4.45 / 2.;
  //     WallStatNEXposY1 = csc[ncl][nst][1] + 1.9 / 2.;
  //     WallStatNEXposY2 = csc[ncl][nst][1] - 1.9 / 2.;
  //     WallStatNEYposX1 = csc[ncl][nst][0] - 4.45 / 2. - 1.9 / 2.;
  //     WallStatNEYposX2 = csc[ncl][nst][0] - 4.45 / 2. + 1.9 / 2.;
  //     WallStatNEYposY = csc[ncl][nst][1];
  //     WallStatNEposZ = csc[ncl][nst][2] + 11.8 + 0.2 + 0.5 + 0.1 - 0.5 / 2. - 0.02;  //

  //     boxWallStatNEX1[ist] = new G4Box("boxWallStatNEX1", WallStatNEXsizeX / 2. * cm, WallStatNEXsizeY / 2. * cm, WallStatNEsizeZ / 2. * cm);
  //     logWallStatNEX1[ist] = new G4LogicalVolume(boxWallStatNEX1[ist], steel, "logWallStatNEX1");
  //     physWallStatNEX1[ist] = new G4PVPlacement(
  //         nullptr,
  //         G4ThreeVector(WallStatNEXposX * m, WallStatNEXposY1 * m, WallStatNEposZ * m),
  //         logWallStatNEX1[ist],
  //         "physWallStatNEX1",
  //         world_log,
  //         false,
  //         idet,
  //         check_overlaps_);

  //     boxWallStatNEX2[ist] = new G4Box("boxWallStatNEX2", WallStatNEXsizeX / 2. * cm, WallStatNEXsizeY / 2. * cm, WallStatNEsizeZ / 2. * cm);
  //     logWallStatNEX2[ist] = new G4LogicalVolume(boxWallStatNEX2[ist], steel, "logWallStatNEX2");
  //     physWallStatNEX2[ist] = new G4PVPlacement(
  //         nullptr,
  //         G4ThreeVector(WallStatNEXposX * m, WallStatNEXposY2 * m, WallStatNEposZ * m),
  //         logWallStatNEX2[ist],
  //         "physWallStatNEX2",
  //         world_log,
  //         false,
  //         idet,
  //         check_overlaps_);

  //     boxWallStatNEY1[ist] = new G4Box("boxWallStatNEY1", WallStatNEYsizeX / 2. * cm, WallStatNEYsizeY / 2. * cm, WallStatNEsizeZ / 2. * cm);
  //     logWallStatNEY1[ist] = new G4LogicalVolume(boxWallStatNEY1[ist], steel, "logWallStatNEY1");
  //     physWallStatNEY1[ist] = new G4PVPlacement(
  //         nullptr,
  //         G4ThreeVector(WallStatNEYposX1 * m, WallStatNEYposY * m, WallStatNEposZ * m),
  //         logWallStatNEY1[ist],
  //         "physWallStatNEY1",
  //         world_log,
  //         false,
  //         idet,
  //         check_overlaps_);

  //     boxWallStatNEY2[ist] = new G4Box("boxWallStatNEY2", WallStatNEYsizeX / 2. * cm, WallStatNEYsizeY / 2. * cm, WallStatNEsizeZ / 2. * cm);
  //     logWallStatNEY2[ist] = new G4LogicalVolume(boxWallStatNEY2[ist], steel, "logWallStatNEY2");
  //     physWallStatNEY2[ist] = new G4PVPlacement(
  //         nullptr,
  //         G4ThreeVector(WallStatNEYposX2 * m, WallStatNEYposY * m, WallStatNEposZ * m),
  //         logWallStatNEY2[ist],
  //         "physWallStatNEY2",
  //         world_log,
  //         false,
  //         idet,
  //         check_overlaps_);

  //     ist = ist + 1;

  //     for (int ndet = 0; ndet < 4; ndet++) {
  //       dXY(nst, ndet, dX, dY);
  //       CaseNEposX = csc[ncl][nst][0] + dX - 4.45 / 2.;
  //       CaseNEposY = csc[ncl][nst][1] + dY;
  //       CaseNEposZ = csc[ncl][nst][2] + 11.8 + 0.2 + 0.5;

  //       boxCaseNE[idet] = new G4Box("boxCaseNE", CaseNEsizeX / 2. * cm, CaseNEsizeY / 2. * cm, CaseNEsizeZ / 2. * cm);
  //       logCaseNE[idet] = new G4LogicalVolume(boxCaseNE[idet], steel, "logCaseNE");
  //       physCaseNE[idet] = new G4PVPlacement(
  //           nullptr,
  //           G4ThreeVector(CaseNEposX * m, CaseNEposY * m, CaseNEposZ * m),
  //           logCaseNE[idet],
  //           "physCaseNE",
  //           world_log,
  //           false,
  //           idet,
  //           check_overlaps_);

  //       boxAirCaseNE[idet] = new G4Box("boxAirCaseNE", (CaseNEsizeX - 0.2) / 2. * cm, (CaseNEsizeY - 0.2) / 2. * cm, (CaseNEsizeZ - 0.2) / 2. *
  //       cm); logAirCaseNE[idet] = new G4LogicalVolume(boxAirCaseNE[idet], Air, "logAirCaseNE"); physAirCaseNE[idet] = new G4PVPlacement(
  //           nullptr, G4ThreeVector(0. * m, 0. * m, 0. * m), logAirCaseNE[idet], "physAirCaseNE", logCaseNE[idet], false, idet, check_overlaps_);

  //       boxScintNE[idet] = new G4Box("boxScintNE", ScintNEsizeX / 2. * cm, ScintNEsizeY / 2. * cm, ScintNEsizeZ / 2. * cm);
  //       logScintNE[idet] = new G4LogicalVolume(boxScintNE[idet], C9H10, "logScintNE");
  //       physScintNE[idet] = new G4PVPlacement(
  //           nullptr,
  //           G4ThreeVector(0. * m, 0. * m, (7.5 - 2. - 0.1 - 0.1) * cm),
  //           logScintNE[idet],
  //           "physScintNE",
  //           logAirCaseNE[idet],
  //           false,
  //           idet,
  //           check_overlaps_);

  //       idet = idet + 1;
  //     }
  //   }
  // }

  // // for (G4int k = 0; k < 144; k++)  // 112
  // // {
  // //   logCaseNE[k]->SetVisAttributes(G4VisAttributes::GetInvisible());
  // //   logAirCaseNE[k]->SetVisAttributes(G4VisAttributes::GetInvisible());
  // //   logScintNE[k]->SetVisAttributes(G4VisAttributes::GetInvisible());
  // // }

  // // for (G4int k = 0; k < 36; k++)  // 28
  // // {
  // //   logRoofNE[k]->SetVisAttributes(G4VisAttributes::GetInvisible());
  // //   logAirRoofNE[k]->SetVisAttributes(G4VisAttributes::GetInvisible());
  // // }

  // // for (auto& k: logFloorNE)  // 3
  // // {
  // //   k->SetVisAttributes(G4VisAttributes::GetInvisible());
  // // }
}

int dXY(int DS, int det, double& x, double& y) {
  double xx[] = {0.4255, 0.4255, -0.4255, -0.4255};
  double yy[] = {-0.4255, 0.4255, 0.4255, -0.4255};
  int ind0[] = {0, 3, 2, 1};
  int index;
  int dir;

  x = 0;
  y = 0;

  if (DS < 0 || DS > 3) return -1;
  if (det < 0 || det > 3) return -2;

  if (DS & 1)
    dir = -1;
  else
    dir = 1;

  index = ind0[DS] + det * dir;
  if (index < 0) index += 4;
  if (index > 3) index -= 4;

  x = xx[index];
  y = yy[index];

  return 0;
}
}  // namespace nevod
