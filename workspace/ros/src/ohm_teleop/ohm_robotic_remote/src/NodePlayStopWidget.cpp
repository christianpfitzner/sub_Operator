#include "NodePlayStopWidget.h"
#include "Button.h"
//#include "ohm_common/PlayStop.h"

#include <QDomDocument>
#include <QFile>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>

#define MSG(x) (qDebug() << "NodePlayStopWidget: " << (x))

NodePlayStopWidget::NodePlayStopWidget(QWidget* parent)
    : QWidget(parent),
      _nh(0)
{
    this->setLayout(new QVBoxLayout);

    QPalette pal = this->palette();
    pal.setColor(this->backgroundRole(), Qt::black);
    this->setPalette(pal);
    this->setWindowTitle("Node Play Stop");
}

NodePlayStopWidget::~NodePlayStopWidget(void)
{
    for (unsigned int i = 0; i < _clients.size(); ++i)
        delete _clients[i];
}

void NodePlayStopWidget::setNodeHandle(ros::NodeHandle* nh)
{
    if (!nh)
        return;

    _nh = nh;
}

void NodePlayStopWidget::buildByXmlFile(const QString& fileName)
{
    if (!_nh)
        return;

    QDomDocument document("config");
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        MSG("can't open file ") << fileName << ".";
        return;
    }

    if (!document.setContent(&file))
    {
        MSG("can't set content to dom xml parser.");
        return;
    }

    QDomElement docElem = document.documentElement();
    QDomNode n = docElem.firstChild();

    while (!n.isNull())
    {
        QDomElement e = n.toElement();

        if (!e.isNull())
        {
            const bool value = e.attribute("value", "false") == QString("true") ? true : false;
            const QString service(e.attribute("service", e.tagName()));

            QHBoxLayout* layout = new QHBoxLayout;
            Button* button = new Button(0, QColor(0xff, 0x99, 0x66), value ? Button::Normal : Button::Triangle);
            button->setFixedSize(48, 48);
            layout->addWidget(button);
            this->connect(button, SIGNAL(clicked()), this, SLOT(callService()));
            _buttons.push_back(button);

            button = new Button(0, QColor(0xff, 0x99, 0x66));
            button->setClickable(false);
            button->setFixedHeight(48);
            button->setText(e.attribute("name", e.tagName()));
            layout->addWidget(button);

            reinterpret_cast<QVBoxLayout*>(this->layout())->addLayout(layout);
            _states.push_back(value);

            ros::ServiceClient* client = new ros::ServiceClient;
//            *client = _nh->serviceClient<ohm_common::PlayStop>(service.toUtf8().data());
            _clients.push_back(client);
        }

        n = n.nextSibling();
    }
}

void NodePlayStopWidget::callService(void)
{
    Button* button = qobject_cast<Button*>(this->sender());

    if (!button)
        return;

    unsigned int i = 0;

    while (i < _buttons.size())
    {
        if (_buttons[i] == button)
            break;

        ++i;
    }
    /*
    ohm_common::PlayStop message;
    message.request.play = !_states[i];

    if (!_clients[i]->call(message))
    {
        MSG("can't call service.");
        return;
    }

    _buttons[i]->setType(message.response.state ? Button::Normal : Button::Triangle);
    _states[i] = message.response.state;
    */
}
