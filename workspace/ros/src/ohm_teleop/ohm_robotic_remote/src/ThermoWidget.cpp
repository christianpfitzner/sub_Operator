#include "ThermoWidget.h"

#include "TextSpinBox.h"
#include "ImageWidget.h"
#include "IntSpinBox.h"
#include "Co2Widget.h"
#include "CloudWidget.h"

#include "JoyStickInterface.h"

#include <QVector>
#include <QPainter>
#include <QVariant>
#include <QDebug>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QImage>

#include <cv_bridge/cv_bridge.h>


ThermoWidget::ThermoWidget(QWidget* parent)
    : StarTrekLayout(StarTrekLayout::BottomToTop, parent),
      _nh(0),
      _palette(optris::eIron),
      _thermoView(new ImageWidget),
      _rgbView(new ImageWidget),
      _armView(new ImageWidget),
      _co2Widget(new Co2Widget),
      _cloudView(new CloudWidget),
      _currentViewerIdx(0)
{
    this->setColor(QColor(0xfe, 0x99, 0x00));
    this->setText("Thermo View");
    this->setMainWidget(_thermoView);
    this->setMainWidget(_armView);

    /* setup scaling method things */
    IntSpinBox* intSpin = new IntSpinBox(0, QColor(0xf2, 0xcb, 0x40));
    _spinMin = intSpin;
    intSpin->setMin(-20);
    intSpin->setMax(120);
    intSpin->setValue(20);
    intSpin->setDisabled(true);
    this->addSideWidget(intSpin);
    this->connect(intSpin, SIGNAL(valueChanged(const int)), this, SLOT(scalingMinMaxChanged(const int)));

    intSpin = new IntSpinBox(0, QColor(0xf2, 0xcb, 0x40));
    _spinMax = intSpin;
    intSpin->setMin(-20);
    intSpin->setMax(120);
    intSpin->setValue(50);
    intSpin->setDisabled(true);
    this->addSideWidget(intSpin);
    this->connect(_spinMax, SIGNAL(valueChanged(const int)), this, SLOT(scalingMinMaxChanged(const int)));

    TextSpinBox* spinBox = new TextSpinBox(0, QColor(0xf2, 0xcb, 0x40));
    spinBox->addItem("Manual", QVariant(optris::eManual));
    spinBox->addItem("MinMax", QVariant(optris::eMinMax));
    spinBox->addItem("Sigma1", QVariant(optris::eSigma1));
    spinBox->addItem("Sigma3", QVariant(optris::eSigma3));
    _builder.setPaletteScalingMethod(optris::eMinMax);
    _minMaxThermo = spinBox;
    spinBox->setCurrentIndex(1);
    this->addSideWidget(spinBox);
//    this->addSeparator();
    this->connect(spinBox, SIGNAL(indexChanged(const int)), this, SLOT(scalingChanged(const int)));

    /* setup color palette things */
    spinBox = new TextSpinBox(0, QColor(0xf2, 0xcb, 0x40));
    spinBox->addItem("AlarmBlue"  , QVariant(optris::eAlarmBlue));
    spinBox->addItem("AlarmBlueHi", QVariant(optris::eAlarmBlueHi));
    spinBox->addItem("GrayBW"     , QVariant(optris::eGrayBW));
    spinBox->addItem("GrayWB"     , QVariant(optris::eGrayWB));
    spinBox->addItem("AlarmGreen" , QVariant(optris::eAlarmGreen));
    spinBox->addItem("Iron"       , QVariant(optris::eIron));
    spinBox->addItem("IronHi"     , QVariant(optris::eIronHi));
    spinBox->addItem("Medical"    , QVariant(optris::eMedical));
    spinBox->addItem("Rainbow"    , QVariant(optris::eRainbow));
    spinBox->addItem("RainbowHi"  , QVariant(optris::eRainbowHi));
    spinBox->addItem("AlarmRed"   , QVariant(optris::eAlarmRed));
    spinBox->setCurrentIndex(5);
    _colorThermo = spinBox;
    this->addSideWidget(spinBox);
    this->connect(spinBox, SIGNAL(indexChanged(const int)), this, SLOT(paletteChanged(const int)));

    /* switch button for switching between kinect and thermo cam */
    spinBox = new TextSpinBox(0, QColor(0xf2, 0xcb, 0x40));
    spinBox->addItem("Thermo");                                         // _currentViewerIdx = 0;
//    spinBox->addItem("Cloud");                                        // _currentViewerIdx = 1;
    spinBox->addItem("Manipulator");                                    // _currentViewerIdx = 1;
    spinBox->addItem("Driver");                                         // _currentViewerIdx = 2;
    this->addSeparator();
    this->addSideWidget(spinBox);
    _spinCamera = spinBox;
    this->connect(spinBox, SIGNAL(indexChanged(const int)), this, SLOT(viewChanged(const int)));

    this->addSeparator();
    _co2Widget->setColor(QColor(0xf2, 0xcb, 0x40));
    this->addSideWidget(_co2Widget);


    // connection to joystick interface via singleton
    this->connect(JoyStickInterface::getInstance(), SIGNAL(nextImageInViewer()), this, SLOT(nextViewer()));
}

ThermoWidget::~ThermoWidget(void)
{
    delete _rgbView;
    delete _thermoView;
    delete _armView;
}

void ThermoWidget::setNodeHandle(ros::NodeHandle* nh, const QByteArray& ns)
{
    if (!nh)
    {
        qDebug() << "Node Handle is NULL.";
        return;
    }

    _nh = nh;
    _ns = ns;

    /* first shutdown all topics, because there is no operator= implemented. */
    _subThermal.shutdown();
    _subManipCam.shutdown();
    _subDriverCam.shutdown();
    _subCo2.shutdown();
    _subVictim.shutdown();

    _subCo2       = _nh->subscribe(QByteArray(ns).append("victim_co2_int").data(),
                                   2,
                                   &ThermoWidget::callbackCo2,
                                   this);

    _subVictim    = _nh->subscribe(QByteArray(ns).append("ohm_victims").data(),
                                   2,
                                   &ThermoWidget::callbackVictim,
                                   this);


    _inspectorStateSub = _nh->subscribe("/inspector_cmd/state",
                                        2,
                                        &ThermoWidget::callbackInspectorState,
                                        this);

    this->viewChanged(0);
}

void ThermoWidget::callbackVictim(const ohm_perception_msgs::VictimArray& victims)
{
    _thermoView->setVictims(victims);
}

void ThermoWidget::callbackThermal(const sensor_msgs::ImageConstPtr& image)
{
   if(_currentViewerIdx == Thermal)
   {
      if (_imageRawData.size() != static_cast<int>(image->width * image->height))
        _imageRawData.resize(image->width * image->height);

      const unsigned short* data = reinterpret_cast<const unsigned short*>(&image->data[0]);
      QVector<unsigned short>::iterator it = _imageRawData.begin();

      while (it < _imageRawData.end())
        *it++ = *data++;

      if (_imgThermal.cols != static_cast<int>(image->width) || _imgThermal.rows != static_cast<int>(image->height))
        _imgThermal.create(image->height, image->width, CV_8UC3);

      _builder.setSize(image->width, image->height, false);
      this->convertDataToImage();
   }
}

void ThermoWidget::callbackInspectorState(const std_msgs::String& state)
{
   static std_msgs::String old_state;

   if(old_state.data != state.data) {
      if(state.data == "STATE_RELEASED") {
         this->viewChanged(Manipulator);
      }
   }
   old_state = state;
}


void ThermoWidget::callbackDriverCam(const sensor_msgs::ImageConstPtr& image)
{
   if(_currentViewerIdx == Driver)
   {
      _imgRgb = cv_bridge::toCvShare(image, "rgb8")->image;
      _rgbView->setImage(_imgRgb);
   }
}

void ThermoWidget::callbackManipulator(const sensor_msgs::ImageConstPtr& image)
{
   if(_currentViewerIdx == Manipulator) {
      _imgManipulatorCam = cv_bridge::toCvShare(image, "rgb8")->image;
      _armView->setImage(_imgManipulatorCam);
   }
}


void ThermoWidget::paletteChanged(const int index)
{
    TextSpinBox* spinBox = qobject_cast<TextSpinBox*>(this->sender());

    if (!spinBox || index < 0) return;

    _palette = static_cast<optris::EnumOptrisColoringPalette>(spinBox->itemData(index).toInt());
    this->convertDataToImage();
}

void ThermoWidget::scalingChanged(const int index)
{
    TextSpinBox* spinBox = qobject_cast<TextSpinBox*>(this->sender());

    if (!spinBox || index < 0)
        return;

    optris::EnumOptrisPaletteScalingMethod method = static_cast<optris::EnumOptrisPaletteScalingMethod>(spinBox->itemData(index).toInt());
    _builder.setPaletteScalingMethod(method);

    if (method == optris::eManual)
    {
        _spinMin->setEnabled(true);
        _spinMax->setEnabled(true);
        this->scalingMinMaxChanged(0.0);

        return;
    }

    _spinMin->setDisabled(true);
    _spinMax->setDisabled(true);
    this->convertDataToImage();
}

void ThermoWidget::scalingMinMaxChanged(const int value)
{
    _builder.setManualTemperatureRange(static_cast<float>(_spinMin->value()), static_cast<float>(_spinMax->value()));
    this->convertDataToImage();
}

void ThermoWidget::viewChanged(const int index)
{
   // shut down all image subscribers
   _subThermal.shutdown();
   _subManipCam.shutdown();
   _subDriverCam.shutdown();

   image_transport::ImageTransport it(*_nh);

    switch (index)
    {
    case Thermal:
       _subThermal   = it.subscribe(  QByteArray(_ns).append("image/thermal").data(),
                                      2,
                                      &ThermoWidget::callbackThermal,
                                      this);

        this->setMainWidget(_thermoView);
        _rgbView->setHidden(true);
        _thermoView->setHidden(false);
        _armView->setHidden(true);
        _cloudView->setHidden(true);
        _spinMin->setDisabled(false);
        _spinMax->setDisabled(false);
        _minMaxThermo->setDisabled(false);
        _colorThermo->setDisabled(false);
        break;
//    case 1:
//        this->setMainWidget(_cloudView);
//        _rgbView->setHidden(true);
//        _thermoView->setHidden(true);
//        _cloudView->setHidden(false);
//        _spinMin->setDisabled(true);
//        _spinMax->setDisabled(true);
//        break;
    case Manipulator:
       _subManipCam  = it.subscribe(  QByteArray(_ns).append("image/arm").data(),
                                      2,
                                      &ThermoWidget::callbackManipulator,
                                      this);

        this->setMainWidget(_armView);
        _rgbView->setHidden(true);
        _armView->setHidden(false);
        _thermoView->setHidden(true);
        _cloudView->setHidden(true);
        _spinMin->setDisabled(true);
        _spinMax->setDisabled(true);
        _minMaxThermo->setDisabled(true);
        _colorThermo->setDisabled(true);
        break;
    case Driver:
       _subDriverCam = it.subscribe(  QByteArray(_ns).append("image/driver").data(),
                                      2,
                                      &ThermoWidget::callbackDriverCam,
                                      this);

       this->setMainWidget(_rgbView);
       _rgbView->setHidden(false);
       _thermoView->setHidden(true);
       _cloudView->setHidden(true);
       _spinMin->setDisabled(true);
       _spinMax->setDisabled(true);
       _minMaxThermo->setDisabled(true);
       _colorThermo->setDisabled(true);
       break;
    }

    _currentViewerIdx = index;

    this->update();
}

void ThermoWidget::convertDataToImage(void)
{
    cv::Mat image;

    _builder.setPalette(_palette);
    _builder.convertTemperatureToPaletteImage(const_cast<unsigned short*>(_imageRawData.data()), _imgThermal.data);

    _thermoView->setImage(_imgThermal);
}

void ThermoWidget::nextViewer(void)
{
   static int idx=0;
   idx++;

   if(idx > 3)   idx = 0;
   std::cout << __PRETTY_FUNCTION__ <<  "id: " << idx << std::endl;


   this->viewChanged(idx);
}


void ThermoWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_A)      _spinCamera->decrement();
    else if (event->key() == Qt::Key_D) _spinCamera->increment();
}

void ThermoWidget::keyReleaseEvent(QKeyEvent* event)
{

}

void ThermoWidget::callbackCo2(const std_msgs::Int16& value)
{
    _co2Widget->setCo2Value(value.data);
}
