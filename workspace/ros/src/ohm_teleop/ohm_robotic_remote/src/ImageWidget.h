/*******************************************************************************************/
/* Class ImageWidget shows a QImage and can draw victim position in there.                 */
/*                                                                                         */
/* Author: Christian Merkl                                                                 */
/* Date  : 5. June 2013                                                                    */
/*******************************************************************************************/
#ifndef __IMAGE_WIDGET__
#define __IMAGE_WIDGET__

#include <QWidget>
#include <QVector>
#include <QRect>
#include <QPainterPath>

#include <opencv2/opencv.hpp>

#include <ohm_perception_msgs/VictimArray.h>

class ImageWidget : public QWidget
{
    Q_OBJECT

public:
    //! default constructor
    ImageWidget(QWidget* parent = 0) : QWidget(parent) { }

public slots:
    //! set image
    void setImage(const cv::Mat& image, const bool setMinSize = false);
    //! set victims
    void setVictims(const ohm_perception_msgs::VictimArray& victims);

protected:
    //! overwrite
    virtual void paintEvent(QPaintEvent* event);
    //! overwrite
    virtual void resizeEvent(QResizeEvent* event);
    //! overwrite
    virtual void keyPressEvent(QKeyEvent* event);

private:
    //! internal use
    void calcPaths(void);

    //! holds shown QImage
    cv::Mat _image;
    //! holds all shown victims
    QVector<ohm_perception_msgs::Victim> _victims;
    //! internal use
    QVector<QPainterPath> _victimPaths;
    //! internal use
    QVector<QRect> _victimLabels;
    //! holds victim channel
    unsigned int _victimChannel;
};

#endif
