#ifndef CWALLMANAGER_H
#define CWALLMANAGER_H

#include <QMainWindow>

#include "ui_cwall_manager.h"
#include "dialogdatabase.h"
#include "dialogconfigure.h"

namespace Ui {
  class CWallManager;
}

class CWallManager : public QMainWindow
{
Q_OBJECT
public:
    explicit CWallManager(QWidget* parent = 0, Qt::WindowFlags flags = 0);

protected:
    virtual bool event(QEvent* event);
	void setConnections();

public:
    virtual ~CWallManager();

	Ui::CWallManager 	*ui;
	DialogDatabase		*dialogDatabase;
};

#endif // CWALLMANAGER_H
