/*
 * DialogLastWarning.cpp
 *
 *  Created on: Sep 12, 2018
 *      Author: phil
 */

#include "DialogLastWarning.h"

DialogLastWarning::DialogLastWarning():
_ui(new Ui::LastWarningDialog)
{
  _ui->setupUi(this);

}

DialogLastWarning::~DialogLastWarning()
{
  delete _ui;
}

