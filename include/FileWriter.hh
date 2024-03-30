#ifndef FILEWRITER_HH
#define FILEWRITER_HH

#include "globals.hh"
#include <exception>
#include <fstream>

namespace NEVOD {

struct EventOutput {
public:
  G4long runNum{};
  G4long eventNum{};

  G4double theta{}, phi{};
  G4double endX{}, endY{}, endZ{};

  G4double energy{};
  G4int particle_num{};

  EventOutput() = default;

  ~EventOutput() = default;
};

class FileWriter {
public:
  explicit FileWriter(G4String &fileName);

  ~FileWriter();

  void WriteEvent(EventOutput &event);

private:
  std::ofstream inputFile;
};

} // namespace NEVOD

#endif