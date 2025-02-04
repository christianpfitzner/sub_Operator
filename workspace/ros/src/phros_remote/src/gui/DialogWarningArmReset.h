#ifndef DIALOG_WARNING_ARM_RESET_H_ 
#define DIALOG_WARNING_ARM_RESET_H_ 

#include <QtWidgets/QDialog>
#include "ui_dialog_warning_armreset.h"
#include <memory>

namespace phros_remote
{

class DialogWarningArmReset : public QDialog
{
  public:
  DialogWarningArmReset();
  virtual ~DialogWarningArmReset(){}  
  private:
    std::unique_ptr<Ui::DialogWarningArmReset> _guiUi;

};

}

#endif