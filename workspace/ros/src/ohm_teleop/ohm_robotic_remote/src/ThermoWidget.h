/*******************************************************************************************/
/* Class ThermoWidget is more a victim cam widget as a thermo widget. You can show a       */
/* optris driver thermal image, rgb image from any webcam, co2 sensor ouput and also       */
/* victims and there distance and id.                                                      */
/*                                                                                         */
/* Author: Christian Merkl                                                                 */
/* Date  : 5. June 2013                                                                    */
/*******************************************************************************************/
#ifndef __THERMO_WIDGET__
#define __THERMO_WIDGET__

#include "ImageBuilder.h"
#include "StarTrekLayout.h"
#include "ImageWidget.h"
#include "Co2Widget.h"

#include <QWidget>
#include <QVector>
#include <QByteArray>

#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>
#include <image_transport/image_transport.h>
#include <std_msgs/Int16.h>
#include <std_msgs/String.h>

#include <ohm_perception_msgs/VictimArray.h>

class IntSpinBox;
class TextSpinBox;
class Button;
class QImage;
class CloudWidget;

//! victim cam widget
/*!
 * this class is more a victim cam widget as a thermo widget. You can show a optris driver thermal image,
 * rgb image from any webcam, co2 sensor ouput and also victims and there distance and id.
 */
class ThermoWidget : public StarTrekLayout
{
    Q_OBJECT
public:
    /**
     * @enum   ActiveImage
     */
    enum ActiveImage
    {
       Thermal,    //!< Thermal
//       Cloud
       Manipulator,//!< Manipulator
       Driver      //!< Driver
    };


    //! default constructor.
    explicit ThermoWidget(QWidget* parent = 0);
    //! destructor.
    ~ThermoWidget(void);

    //! set ros node handle.
    /*!
     * this method must be called bevor any subscribers can work.
     */
    void setNodeHandle(ros::NodeHandle* nh, const QByteArray& ns = QByteArray());

public slots:
   //! switch between thermal and rgb image.
   void viewChanged(const int index);

private slots:
    //! change the color palette of thermal image.
    void paletteChanged(const int index);
    //! change the scaling method.
    void scalingChanged(const int index);
    //! change the limits for manual operation mode.
    void scalingMinMaxChanged(const int value);

    void nextViewer(void);



protected:
    //! overwrite
    virtual void keyPressEvent(QKeyEvent* event);
    //! overwirte
    virtual void keyReleaseEvent(QKeyEvent* event);

private:
    //! internal use
    void convertDataToImage(void);
    //! ros subscriber callback for thermal image.
    void callbackThermal(const sensor_msgs::ImageConstPtr& image);
    /**
     * Callback function for image from driving camera
     */
       void callbackDriverCam(const sensor_msgs::ImageConstPtr& image);
    /**
     * Callback function for image from manipulator camera
     * @param[in] image    image of manipulator camera
     */
    void callbackManipulator(const sensor_msgs::ImageConstPtr& image);

    //! ros subscriber callback for co2 sensor output.
    void callbackCo2(const std_msgs::Int16& value);
    //! ros subscriber callback for victims.
    void callbackVictim(const ohm_perception_msgs::VictimArray& victims);

    /**
     * Callback function for inspector state
     * @param state
     */
    void callbackInspectorState(const std_msgs::String& state);

    //! holds the ros node handle.
    ros::NodeHandle* _nh;
    QByteArray       _ns;

    image_transport::Subscriber _subThermal;          //!< holds subscriber for thremal image.
    image_transport::Subscriber _subDriverCam;        //!< subscriber for driver camera
    image_transport::Subscriber _subManipCam;         //!< subscriber for manipulator camera

    ros::Subscriber             _subCo2;              //!< holds subscriber for co2 sensor output.
    ros::Subscriber             _subVictim;           //!< holds subscriber for victims.
    ros::Subscriber             _inspectorStateSub;   //!< subscriber for inspector


    //! thermal image.
    cv::Mat       _imgThermal;
    cv::Mat       _imgRgb;
    cv::Mat       _imgManipulatorCam;

    IntSpinBox*   _spinMax;                           //!< lower limit for thermal image.
    IntSpinBox*   _spinMin;                           //!< upper limut for thermal image.
    TextSpinBox*  _spinCamera;                        //!< spin box to select image type.
    TextSpinBox*  _minMaxThermo;
    TextSpinBox*  _colorThermo;

    QVector<unsigned short>           _imageRawData;  //!< internal use.
    optris::EnumOptrisColoringPalette _palette;       //!< holds color palette for thermal image.
    optris::ImageBuilder              _builder;


    ImageWidget*     _thermoView;                     //!< image widget for thermal image.
    ImageWidget*     _rgbView;                        //!< image widget for rgb image.
    ImageWidget*     _armView;
    Co2Widget*       _co2Widget;                      //!< widget to show co2 sensor output.

    unsigned char*   _imgDataRgb;
    CloudWidget*     _cloudView;

    unsigned int     _currentViewerIdx;
};

#endif
