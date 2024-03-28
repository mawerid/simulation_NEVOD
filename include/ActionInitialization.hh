#ifndef ACTION_HH
#define ACTION_HH

#include "G4VUserActionInitialization.hh"

namespace NEVOD {

class ActionInitialization : public G4VUserActionInitialization {
public:
  ActionInitialization() = default;
  ~ActionInitialization() override = default;

  void BuildForMaster() const override;
  void Build() const override;
};

} // namespace NEVOD

#endif // ACTION_HH
