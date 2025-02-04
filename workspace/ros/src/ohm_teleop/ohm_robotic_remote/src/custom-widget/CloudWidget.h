#ifndef __CLOUD_WIDGET__
#define __CLOUD_WIDGET__

#include <vector>

#include <QVTKWidget.h>
#include <vtkSmartPointer.h>
#include "ohm_perception_msgs/Victim.h"

class vtkRenderer;
class vtkPolyData;
class vtkPoints;
class vtkUnsignedCharArray;
class QImage;

class CloudWidget : public QVTKWidget
{
    Q_OBJECT

public:
    CloudWidget(QWidget* parent = 0);
    ~CloudWidget(void);

public slots:
    void setCloud(const ohm_perception_msgs::Victim& victim, const QImage* image = 0);

private:
    vtkSmartPointer<vtkRenderer>          m_renderer;
    vtkSmartPointer<vtkPolyData>          m_polyData;
    vtkSmartPointer<vtkPoints>            m_points;
    vtkSmartPointer<vtkUnsignedCharArray> m_colors;
};


#endif
