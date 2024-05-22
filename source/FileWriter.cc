#include "FileWriter.hh"

namespace NEVOD {

FileWriter::FileWriter(G4String &fileName) {
  outputFile.open(fileName.data(), std::ofstream::out | std::ofstream::app);

  if (!outputFile.is_open())
    throw std::ios_base::failure("Cannot open output file.");
}

FileWriter::~FileWriter() { outputFile.close(); }

void FileWriter::writeEvent(const EventOutput &event,
                            const G4double (&amplitudeKSM)[7][4][4][6],
                            const G4long (&hSM)[8],
                            const G4long (&nTrackSMX)[8],
                            const G4long (&nTrackSMY)[8],
                            const G4long (&nTrackSM)[8],
                            const G4double (&edepCountSCT)[9][5][2]) {
  outputFile << event.eventCount << ',' << event.runNum << ',' << event.eventNum
             << ',' << event.theta << ',' << event.phi << ',' << event.energyDep
             << ',' << event.particleCount << ',' << event.trackLength << ','
             << event.muonCount << ',' << event.energyStart << ','
             << event.energyEnd << ',' << event.muDCR << ',' << event.muSM
             << ',' << event.nSM << ',' << event.muDCRw << ',' << event.muSMw
             << ',' << event.nSMw << ',';

  G4int nms[7] = {4, 3, 4, 3, 4, 3, 4}, plane_num = 7, feu_num = 6;

  for (G4int plane = 0; plane < plane_num; plane++)
    for (G4int stride = 0; stride < nms[plane]; stride++)
      for (G4int module = 0; module < nms[plane]; module++)
        for (G4int feu_amplitude = 0; feu_amplitude < feu_num; feu_amplitude++)
          outputFile //<< "1,"
                     << amplitudeKSM[plane][stride][module][feu_amplitude]
                     << ',';

  for (const auto &i : hSM)
    outputFile << i << ',';

  for (const auto &i : nTrackSMX)
    outputFile << i << ',';

  for (const auto &i : nTrackSMY)
    outputFile << i << ',';

  for (const auto &i : nTrackSM)
    outputFile << i << ',';

  for (const auto &item : edepCountSCT)
    for (const auto &kItem : item)
      for (const auto &edep : kItem)
        outputFile << edep << ',';

  outputFile << -1 << std::endl;
}
} // namespace NEVOD