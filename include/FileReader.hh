#ifndef FILEREADER_HH
#define FILEREADER_HH

#include "globals.hh"
#include <exception>
#include <fstream>
#include <list>

namespace NEVOD {

#define EVENT_BUFFER_SIZE 100

struct EventInit {
public:
  G4long runNum{};
  G4long eventNum{};

  G4double startX{}, startY{}, startZ{};
  G4double endX{}, endY{}, endZ{};

  G4double energy{};
  G4int particle_num{};

  EventInit(G4long runNum_, G4long eventNum_, G4double startX_, G4double startY_,
        G4double startZ_, G4double endX_, G4double endY_, G4double endZ_,
        G4double energy_, G4int particle_num_);

  EventInit() = default;

  ~EventInit() = default;
};

class FileReader {
public:
  explicit FileReader(G4String &fileName, size_t shift = 0);

  ~FileReader();

  EventInit getEvent();

private:
  std::ifstream inputFile;
  std::list<EventInit> eventList;
};

} // namespace NEVOD

#endif