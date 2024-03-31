#ifndef FILEWRITER_HH
#define FILEWRITER_HH

#include "globals.hh"
#include <exception>
#include <fstream>

namespace NEVOD {

struct EventOutput {
public:
  G4long eventCount{};
  G4long runNum{};
  G4long eventNum{};

  G4double theta{}, phi{};

  G4double energyDep{};
  G4long particleCount{};
  G4double trackLength{};
  G4long muonCount{};
  G4double energyStart{};
  G4double energyEnd{};

  G4long muDCR, muSM, nSM, muDCRw, muSMw, nSMw;

  EventOutput() = default;

  ~EventOutput() = default;
};

class FileWriter {
public:
  explicit FileWriter(G4String &fileName);

  ~FileWriter();

  void writeEvent(const EventOutput &event,
                  const G4long (&amplitudeKSM)[7][4][4][6],
                  const G4long (&hSM)[8], const G4long (&nTrackSMX)[8],
                  const G4long (&nTrackSMY)[8], const G4long (&nTrackSM)[8],
                  const G4double (&edepCountSCT)[9][5][2]);

private:
  std::ofstream outputFile;
};

} // namespace NEVOD

#endif