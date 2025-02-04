/********************************************************************************
** Form generated from reading UI file 'JoyConfig.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_JOYCONFIG_H
#define UI_JOYCONFIG_H

#include <QtCore/QVariant>
#include <QAction> //changed to QtGui/... to ... for all wollowing includes
#include <QApplication>
#include <QButtonGroup>
#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QSpacerItem>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

QT_BEGIN_NAMESPACE

class Ui_JoyConfig
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pushButton_load;
    QPushButton *pushButton_save;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout_Left;
    QFrame *line;
    QVBoxLayout *verticalLayout_Right;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *JoyConfig)
    {
        if (JoyConfig->objectName().isEmpty())
            JoyConfig->setObjectName(QString::fromUtf8("JoyConfig"));
        JoyConfig->resize(282, 423);
        centralwidget = new QWidget(JoyConfig);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setSizeConstraint(QLayout::SetDefaultConstraint);
        pushButton_load = new QPushButton(centralwidget);
        pushButton_load->setObjectName(QString::fromUtf8("pushButton_load"));
        pushButton_load->setMaximumSize(QSize(80, 16777215));

        verticalLayout_2->addWidget(pushButton_load);

        pushButton_save = new QPushButton(centralwidget);
        pushButton_save->setObjectName(QString::fromUtf8("pushButton_save"));
        pushButton_save->setMaximumSize(QSize(80, 16777215));

        verticalLayout_2->addWidget(pushButton_save);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout_2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        verticalLayout_Left = new QVBoxLayout();
        verticalLayout_Left->setObjectName(QString::fromUtf8("verticalLayout_Left"));

        horizontalLayout->addLayout(verticalLayout_Left);

        line = new QFrame(centralwidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);

        horizontalLayout->addWidget(line);

        verticalLayout_Right = new QVBoxLayout();
        verticalLayout_Right->setObjectName(QString::fromUtf8("verticalLayout_Right"));

        horizontalLayout->addLayout(verticalLayout_Right);

        JoyConfig->setCentralWidget(centralwidget);
        menubar = new QMenuBar(JoyConfig);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 282, 25));
        JoyConfig->setMenuBar(menubar);
        statusbar = new QStatusBar(JoyConfig);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        JoyConfig->setStatusBar(statusbar);

        retranslateUi(JoyConfig);

        QMetaObject::connectSlotsByName(JoyConfig);
    } // setupUi

    void retranslateUi(QMainWindow *JoyConfig)
    {
    	// removed ", QApplication::UnicodeUTF8" at the end of each line before "));"
        JoyConfig->setWindowTitle(QApplication::translate("JoyConfig", "MainWindow", 0));
        pushButton_load->setText(QApplication::translate("JoyConfig", "Load Config", 0));
        pushButton_save->setText(QApplication::translate("JoyConfig", "Save Config", 0));
    } // retranslateUi

};

namespace Ui {
    class JoyConfig: public Ui_JoyConfig {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_JOYCONFIG_H
