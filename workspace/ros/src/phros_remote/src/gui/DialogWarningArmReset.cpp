#include "DialogWarningArmReset.h"

namespace phros_remote
{
  DialogWarningArmReset::DialogWarningArmReset():
  _guiUi(std::unique_ptr<Ui::DialogWarningArmReset>(new Ui::DialogWarningArmReset()))
  {
    _guiUi->setupUi(this);
  }

}