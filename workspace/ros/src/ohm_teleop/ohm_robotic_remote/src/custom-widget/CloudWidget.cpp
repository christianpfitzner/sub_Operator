#include "CloudWidget.h"

#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkVertexGlyphFilter.h>
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include <vtkCamera.h>

#include <QDebug>
#include <QImage>

#include <geometry_msgs/Point32.h>

CloudWidget::CloudWidget(QWidget* parent)
    : QVTKWidget(parent),
      m_renderer(vtkRenderer::New()),
      m_polyData(vtkSmartPointer<vtkPolyData>::New()),
      m_points(vtkSmartPointer<vtkPoints>::New()),
      m_colors(vtkSmartPointer<vtkUnsignedCharArray>::New())
{
    m_renderer->SetBackground(.0, .0, .0);
    m_renderer->GetActiveCamera()->Yaw(180);
    this->GetRenderWindow()->AddRenderer(m_renderer);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(m_polyData->GetProducerPort());
//    mapper->SetInputData(m_polyData);

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetPointSize(1);

    m_renderer->AddActor(actor);
    m_renderer->ResetCamera();
}

CloudWidget::~CloudWidget(void)
{
    // Maybe i should delete m_polyData, m_points, ...
}

void CloudWidget::setCloud(const ohm_perception_msgs::Victim& victim, const QImage* image)
{
    m_polyData->Reset();
    m_points->Reset();
    m_colors->Reset();
    m_colors->SetNumberOfComponents(3);
    /*
    for (std::vector<geometry_msgs::Point32>::const_iterator point(victim.cloud.begin()); point < victim.cloud.end(); ++point)
        m_points->InsertNextPoint(point->x, point->y, point->z);

    if (image)
    {
        for (std::vector<ohm_victim::Point>::const_iterator point(victim.points.begin()); point < victim.points.end(); ++point)
        {
            if (point->x > image->width() || point->y > image->height())
                continue;

            QColor pixel(image->pixel(point->x, point->y));
            unsigned char temp[3];
            temp[0] = static_cast<unsigned char>(pixel.red());
            temp[1] = static_cast<unsigned char>(pixel.green());
            temp[2] = static_cast<unsigned char>(pixel.blue());

            m_colors->InsertNextTupleValue(temp);
        }
    }
    else
    {
        for (unsigned int i = 0; i < victim.cloud.size(); i++)
        {
            unsigned char temp[3];
            temp[0] = 0xff;
            temp[1] = 0xff;
            temp[2] = 0xff;

            m_colors->InsertNextTupleValue(temp);
        }
    }
    */
    m_polyData->GetPointData()->SetNormals(NULL);
    m_polyData->SetPoints(m_points);

    vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter =  vtkSmartPointer<vtkVertexGlyphFilter>::New();
    glyphFilter->SetInputConnection(m_polyData->GetProducerPort());
//    glyphFilter->SetInputData(m_polyData);
    glyphFilter->Update();

    m_polyData->ShallowCopy(glyphFilter->GetOutput());
    m_polyData->GetPointData()->SetScalars(m_colors);
    m_points->Modified();
    this->update();
}
