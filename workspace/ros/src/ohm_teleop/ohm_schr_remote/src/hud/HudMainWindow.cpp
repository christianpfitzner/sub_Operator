/*
 * HudMainWindow.cpp
 *
 *  Created on: Jun 4, 2018
 *      Author: phil
 */

#include "HudMainWindow.h"
#include "Hud.h"
#include <QMessageBox>
#include "DialogLastWarning.h"

HudMainWindow::HudMainWindow(Hud& hud):
_hudUi(new Ui::MainWindow),
_hud(hud)
{
  _hudUi->setupUi(this);
  this->show();
  connect(_hudUi->actionReference_Mode, SIGNAL(triggered()), &_hud, SLOT(referenceDriveTriggered()));
  connect(_hudUi->actionSpread, SIGNAL(triggered()), &_hud, SLOT(spreadGripper()));
  connect(_hudUi->actionReset_Drives, SIGNAL(triggered()), &_hud, SLOT(resetTrackDrives()));
}

HudMainWindow::~HudMainWindow()
{
  delete _hudUi;
}

void HudMainWindow::errorMessage(const QString& message)
{
  QMessageBox msgBox;
  msgBox.setText(message);
  msgBox.exec();
}

bool HudMainWindow::referenceDriveMenu(void)
{
  DialogLastWarning lastWarning;
  int res = lastWarning.exec();
  if(res == QDialog::Accepted)
    return true;
  else
    return false;
}
