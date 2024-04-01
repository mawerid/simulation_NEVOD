#include "FileReader.hh"

namespace NEVOD {

EventInit::EventInit(G4long runNum_, G4long eventNum_, G4double startX_,
                     G4double startY_, G4double startZ_, G4double endX_,
                     G4double endY_, G4double endZ_, G4double energy_,
                     G4int particle_num_)
    : runNum(runNum_), eventNum(eventNum_), startX(startX_), startY(startY_),
      startZ(startZ_), endX(endX_), endY(endY_), endZ(endZ_), energy(energy_),
      particle_num(particle_num_) {}

FileReader::FileReader(G4String &fileName, size_t shift) {
  inputFile.open(fileName.data(), std::ifstream::in);

  if (!inputFile.is_open()) {
    throw std::ios_base::failure("Cannot open input file.");
  }

  EventInit event;
  for (size_t i = 0; i < shift; i++) {
    inputFile >> event.runNum >> event.eventNum >> event.startX >>
        event.startY >> event.startZ >> event.endX >> event.endY >> event.endZ;
  }
}

FileReader::~FileReader() { inputFile.close(); }

EventInit FileReader::getEvent() {

  if (eventList.empty()) {
    for (int i = 0; i < EVENT_BUFFER_SIZE; i++) {
      EventInit event;
      inputFile >> event.runNum >> event.eventNum >> event.startX >>
          event.startY >> event.startZ >> event.endX >> event.endY >>
          event.endZ;

      event.energy = 101.0;
      event.particle_num = 5;

      eventList.push_back(event);
    }
  }
  EventInit event = eventList.front();
  eventList.pop_front();
  return event;
}

} // namespace NEVOD