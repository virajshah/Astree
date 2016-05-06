// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef DOCKLIGHTPROPERTIES_H
#define DOCKLIGHTPROPERTIES_H

#include <QDockWidget>

class OpticalDevice;

namespace Ui
{
    class DockLightProperties;
}

class DockLightProperties : public QDockWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DockLightProperties)
public:
    explicit DockLightProperties(QWidget *parent = 0);
    virtual ~DockLightProperties();

    void device_changed(OpticalDevice* pDevice, int iReason);

//signals:
  //  void device_changed(void* pSender);

public slots:
    void OnLightChange();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::DockLightProperties *m_ui;
    OpticalDevice* _pDevice;
};

#endif // DOCKLIGHTPROPERTIES_H
