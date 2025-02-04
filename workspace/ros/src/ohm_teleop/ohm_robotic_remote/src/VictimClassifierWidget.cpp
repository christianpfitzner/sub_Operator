#include "VictimClassifierWidget.h"

#include <QPainter>

VictimClassifierWidget::VictimClassifierWidget(QWidget* parent)
    : QWidget(parent),
      _nh(0)
{
    QPalette pal = this->palette();
    pal.setColor(this->backgroundRole(), Qt::black);
    this->setPalette(pal);
    this->setWindowTitle("Victim Classifier");

    this->setMinimumSize(400, 300);
    this->resize(400, 300);
}

void VictimClassifierWidget::setNodeHandle(ros::NodeHandle* nh)
{
    if (!nh)
        return;

    _nh = nh;

    _subReport.shutdown();
    _subReport = _nh->subscribe("/georg/victim_classifier_report",
                                2,
                                &VictimClassifierWidget::callbackReport,
                                this);
}

void VictimClassifierWidget::callbackReport(const std_msgs::String& msg)
{
    _report = QString(msg.data.c_str());
    this->update();
    this->show();
}

void VictimClassifierWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    QFont font("United Federation of Planets.", 10);
    painter.setFont(font);
    QPen pen;
    pen.setColor(Qt::green);
    painter.setPen(pen);

    painter.drawText(this->rect(), Qt::AlignTop | Qt::AlignLeft, _report);
}
