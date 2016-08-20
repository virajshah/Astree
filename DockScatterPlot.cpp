// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "DockScatterPlot.h"
#include "MainWindow.h"
#include "ui_DockScatterPlot.h"

#include <QGraphicsSceneWheelEvent>
#include <QGraphicsItem>

#include "OpticalDevice.h"
#include "Light.h"
#include "Photon.h"
#include "AstreeDefines.h"

#include <cassert>
#include <algorithm>
#define isnan(a) (false) //TODO

#ifndef NB_POINTS_SCATTER_PLOT
#define NB_POINTS_SCATTER_PLOT 81
#endif

void DockScatterPlot::enlarge(QRectF& r,double dRatio)
{
    double dX=r.width();
    double dY=r.height();
    QPointF c=r.center();
    r.setCoords( c.x()-dX*dRatio/2., c.y()-dY*dRatio/2., c.x()+dX*dRatio/2., c.y()+dY*dRatio/2.);
}

class myScene : public QGraphicsScene
{
protected:
    virtual void wheelEvent ( QGraphicsSceneWheelEvent * wheelEvent )
    {
        QGraphicsView* v=views()[0];
        v->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

        if (wheelEvent->delta()>0)
            v->scale(1.25,1.25);
        else
            v->scale(0.8,0.8);

        wheelEvent->accept();
    }
};


class ScatterPlot : public QGraphicsItem
{
public:
    ScatterPlot()
    {
        minX=0;
        minY=0;
        maxX=0;
        maxY=0;
    }

    void addPoint(double x,double y,int icolor)
    {
        if(vx.empty())
        {
            minX=x;
            minY=y;
            maxX=x;
            maxY=y;
        }
        else
        {
            minX=std::min<double>(minX,x);
            maxX=std::max<double>(maxX,x);
            minY=std::min<double>(minY,y);
            maxY=std::max<double>(maxY,y);
        }

        vx.push_back(x);
        vy.push_back(y);
        vcolor.push_back(icolor);
    }


    QRectF boundingRect() const
    {
        return QRectF(minX,minY,maxX-minX+1.e-16,maxY-minY+1.e-16);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        (void)option;
        (void)widget;

        QPen qp;
        qp.setCosmetic(true);

        for(unsigned int i=0;i<vx.size();i++)
        {
            qp.setColor(vcolor[i]);
            painter->setPen(qp);
            painter->drawPoint(QPointF(vx[i],vy[i]));
        }
    }

private:
    double minX,minY,maxX,maxY;

    vector<double> vx;
    vector<double> vy;
    vector<int> vcolor;
};

////////////////////////////////////////////////////////////////////////////////

DockScatterPlot::DockScatterPlot(QWidget *parent) :
    QDockWidget(parent),
    m_ui(new Ui::DockScatterPlot)
{
    _bBlockSignals=true;

    m_ui->setupUi(this);
    m_ui->graphicsView->setScene(new myScene); //TODO deleter la scene?

    m_ui->graphicsView->setDragMode (QGraphicsView::ScrollHandDrag);
    m_ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_ui->graphicsView->setRenderHints( QPainter::Antialiasing );

    _bBlockSignals=false;
}

DockScatterPlot::~DockScatterPlot()
{
    delete m_ui;
}

void DockScatterPlot::changeEvent(QEvent *e)
{
    QDockWidget::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
////////////////////////////////////////////////////////////////////////////////
void DockScatterPlot::device_changed(OpticalDevice* pDevice, int iReason)
{
    _pDevice=pDevice;

    if( (iReason!=PARAMETERS_CHANGED) && (iReason!=OPTICAL_DEVICE_CHANGED) && (iReason!=NEW_OPTICAL_DEVICE) )
        return;

    _bBlockSignals=true;

    double dPercentField=-1;
    if(!pDevice->get_parameter("showLightOffAxis",dPercentField))
        dPercentField=0.;

    if(iReason==NEW_OPTICAL_DEVICE)
    {
        m_ui->hsImageFieldPos->blockSignals(true);
        m_ui->hsImageFieldPos->setValue((int)dPercentField);
        m_ui->hsImageFieldPos->blockSignals(false);
    }

    QGraphicsScene* scene=m_ui->graphicsView->scene();
    scene->clear();

    if (pDevice->nb_surface()==0)
    {
        m_ui->hsImageFieldPos->setValue(0);
        m_ui->qlPsfDiameter->setText("n/a");
        m_ui->qlFD->setText("n/a");
        m_ui->qlPerfectAiry->setText("n/a");
        m_ui->qlLFro->setText("n/a");
        return;
    }

    Light light;
    double dTiltDegree=dPercentField*pDevice->half_field_of_view()/100.;
    pDevice->compute_light(&light,pDevice->nb_surface()-1, dTiltDegree,NB_POINTS_SCATTER_PLOT,NB_POINTS_SCATTER_PLOT);

    ScatterPlot* sp=new ScatterPlot;
    sp->setZValue(10);
    int iNbPhoton=light.nb_photon();
    for (int i=0;i<iNbPhoton;i++)
    {
        Photon& rP=light.get_photon(i);
        if (rP.is_valid())
        {
            double dX=rP.x;
            double dY=rP.y;

            assert(!isnan(dX));
            assert(!isnan(dY));

            sp->addPoint(-dY,dX,light.get_visual_color(rP.lambda()));
        }
    }

    double dFD=light.get_FD();
    if(dFD>=FD_VALID_MAX)
    {
        m_ui->qlPsfDiameter->setText("n/a");
        m_ui->qlFD->setText("inf");
        m_ui->qlPerfectAiry->setText("inf");
        m_ui->qlLFro->setText("inf");

        QRectF rsp=sp->boundingRect();
        enlarge(rsp,1.1);
        scene->addItem(sp);
        scene->setSceneRect(rsp);
        m_ui->graphicsView->fitInView(rsp,Qt::KeepAspectRatio);

        return;
    }

    double dCenterX,dCenterY;
    double dPsfDiameter=light.spot_size();
    double dPerfectAiryRadius=light.airy_radius();
    double dLfro=light.spot_vs_airy();
    light.get_spot_center(dCenterY,dCenterX);
    dCenterX=-dCenterX;

    QPen qp;
    qp.setCosmetic(true);
    scene->addItem(sp);
    QGraphicsItem * pEllipse=scene->addEllipse((dCenterX-dPerfectAiryRadius),(dCenterY-dPerfectAiryRadius),dPerfectAiryRadius*2.,dPerfectAiryRadius*2.,qp);
    pEllipse->setZValue(20);

    QRectF rsp=sp->boundingRect();
    QRectF rellipse((dCenterX-dPerfectAiryRadius),(dCenterY-dPerfectAiryRadius),dPerfectAiryRadius*2.,dPerfectAiryRadius*2.);
    QRectF rtotal= rsp | rellipse;
    enlarge(rtotal,1.1);

    scene->setSceneRect(rtotal);
    m_ui->graphicsView->fitInView(rtotal,Qt::KeepAspectRatio);

    m_ui->qlPsfDiameter->setText(QString::number(dPsfDiameter*1000.,'g',3)+" µm");
    m_ui->qlFD->setText(QString::number(dFD,'f',1));

    m_ui->qlPerfectAiry->setText(QString::number(dPerfectAiryRadius*2.*1000.,'g',3)+" µm");
    m_ui->qlLFro->setText(QString::number(dLfro,'g',3));

    m_ui->qlVignetting->setText(QString::number(light.vignetting(),'f',1)+" %");

    _bBlockSignals=false;
}

void DockScatterPlot::on_pushButton_clicked()
{
    m_ui->hsImageFieldPos->setValue(0);
}

void DockScatterPlot::on_pushButton_2_clicked()
{
    m_ui->hsImageFieldPos->setValue(m_ui->hsImageFieldPos->maximum()/2);
}

void DockScatterPlot::on_pushButton_3_clicked()
{
    m_ui->hsImageFieldPos->setValue(m_ui->hsImageFieldPos->maximum());
}

void DockScatterPlot::on_hsImageFieldPos_valueChanged(int value)
{
    if(_bBlockSignals)
        return;

    _pDevice->set_parameter("showLightOffAxis",value); // in percent
    device_changed(_pDevice,PARAMETERS_CHANGED);
    static_cast<MainWindow*>(parent())->device_changed(this,PARAMETERS_CHANGED,false);
}
