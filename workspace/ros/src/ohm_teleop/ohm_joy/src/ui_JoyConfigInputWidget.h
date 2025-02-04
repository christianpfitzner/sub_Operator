/********************************************************************************
** Form generated from reading UI file 'JoyConfigInputWidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_JOYCONFIGINPUTWIDGET_H
#define UI_JOYCONFIGINPUTWIDGET_H

#include <QtCore/QVariant>
#include <QAction> //changed to QtGui/... to ... for all following includes
#include <QApplication>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

QT_BEGIN_NAMESPACE

class Ui_JoyConfigInputWidget
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLabel *label_2;
    QPushButton *pushButton;

    void setupUi(QWidget *JoyConfigInputWidget)
    {
        if (JoyConfigInputWidget->objectName().isEmpty())
            JoyConfigInputWidget->setObjectName(QString::fromUtf8("JoyConfigInputWidget"));
        JoyConfigInputWidget->resize(400, 45);
        horizontalLayout = new QHBoxLayout(JoyConfigInputWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(JoyConfigInputWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMaximumSize(QSize(16777215, 30));
        label->setFrameShape(QFrame::NoFrame);

        horizontalLayout->addWidget(label);

        label_2 = new QLabel(JoyConfigInputWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setMaximumSize(QSize(120, 30));
        label_2->setFrameShape(QFrame::Panel);
        label_2->setFrameShadow(QFrame::Sunken);
        label_2->setAlignment(Qt::AlignHCenter|Qt::AlignTop);

        horizontalLayout->addWidget(label_2);

        pushButton = new QPushButton(JoyConfigInputWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setMaximumSize(QSize(60, 30));

        horizontalLayout->addWidget(pushButton);


        retranslateUi(JoyConfigInputWidget);

        QMetaObject::connectSlotsByName(JoyConfigInputWidget);
    } // setupUi

    void retranslateUi(QWidget *JoyConfigInputWidget)
    {
    	// removed ", QApplication::UnicodeUTF8" at the end of each line before "));"
        JoyConfigInputWidget->setWindowTitle(QApplication::translate("JoyConfigInputWidget", "Form", 0));
        label->setText(QApplication::translate("JoyConfigInputWidget", "TextLabel", 0));
        label_2->setText(QApplication::translate("JoyConfigInputWidget", "<not set>", 0));
        pushButton->setText(QApplication::translate("JoyConfigInputWidget", "Set", 0));
    } // retranslateUi

};

namespace Ui {
    class JoyConfigInputWidget: public Ui_JoyConfigInputWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_JOYCONFIGINPUTWIDGET_H
