#include "cwallmanager.h"

CWallManager::CWallManager(QWidget* parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags)
  , ui(new Ui::CWallManager)
  , dialogDatabase(new DialogDatabase(this))
{
  ui->setupUi(this);

  setConnections();
}

void CWallManager::setConnections()
{
  connect(ui->actionManagerDatabase, SIGNAL(activated()), dialogDatabase, SLOT(exec()));
  connect(ui->actionManagerConfiguration, SIGNAL(activated()), DialogConfigure::cfg(this), SLOT(exec()));
  connect(ui->actionManagerExit, SIGNAL(activated()), this, SLOT(close()));
}

bool CWallManager::event(QEvent* event)
{
    return QMainWindow::event(event);
}

CWallManager::~CWallManager()
{
  delete dialogDatabase;
}

#include "cwallmanager.moc"