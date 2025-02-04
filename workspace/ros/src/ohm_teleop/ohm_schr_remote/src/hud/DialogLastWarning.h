/*
 * DialogLastWarning.h
 *
 *  Created on: Sep 12, 2018
 *      Author: phil
 */

#ifndef OHM_SCHR_REMOTE_SRC_HUD_DIALOGLASTWARNING_H_
#define OHM_SCHR_REMOTE_SRC_HUD_DIALOGLASTWARNING_H_

#include <QDialog>
#include "ohm_schr_remote/ui_last_warning.h"

class DialogLastWarning: public QDialog
{
  Q_OBJECT
public:
  DialogLastWarning();
  virtual ~DialogLastWarning();
private:
  Ui::LastWarningDialog* _ui;
};

#endif /* OHM_SCHR_REMOTE_SRC_HUD_DIALOGLASTWARNING_H_ */
