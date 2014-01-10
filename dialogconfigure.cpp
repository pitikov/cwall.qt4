#include "dialogconfigure.h"
#include <QDebug>

DialogConfigure *DialogConfigure::self_ = NULL;

DialogConfigure::DialogConfigure(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f)
  , ui(new Ui::DialogConfigure)
  , cfg_(new QSettings(QSettings::IniFormat, QSettings::UserScope, "ClimbingWall", qAppName(), this))
{
  ui->setupUi(this);
}

void DialogConfigure::accept()
{
    QDialog::accept();
}

void DialogConfigure::reject()
{
    QDialog::reject();
}

DialogConfigure::~DialogConfigure()
{
  self_ = NULL;
}

DialogConfigure* DialogConfigure::cfg(QWidget* parent)
{
  if (!self_) {
	if (parent) {
	  self_ = new DialogConfigure(parent);
	} else {
	  while (!self_) {}
	}
  }
  return self_;
}

void DialogConfigure::setValue(QString group, QString key, QVariant value)
{
  cfg_->beginGroup(group);
  cfg_->setValue(key, value);
  cfg_->endGroup();
}

QVariant DialogConfigure::value(QString group, QString key, QVariant default_value)
{
  QVariant value;
  cfg_->beginGroup(group);
  value = cfg_->value(key, default_value);
  cfg_->endGroup();
  return value;
}

QSettings* DialogConfigure::settings()
{
  return cfg_;
}

#include "dialogconfigure.moc"