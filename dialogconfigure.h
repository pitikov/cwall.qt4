#ifndef DIALOGCONFIGURE_H
#define DIALOGCONFIGURE_H

#include <QDialog>
#include <QSettings>

#include "ui_dialog_configure.h"

namespace Ui {
  class DialogConfigure;
}

#define DbSettings "Database_config"
#define DbItems "Database_item"

class DialogConfigure : public QDialog
{
Q_OBJECT
public:
  virtual ~DialogConfigure();

  static DialogConfigure* cfg(QWidget *parent = 0);
  QVariant value(QString group, QString key, QVariant default_value = QVariant());
  void setValue(QString group, QString key, QVariant value);
  QSettings *settings();

private:
  explicit DialogConfigure(QWidget* parent = 0, Qt::WindowFlags f = 0);
  virtual void accept();
  virtual void reject();

  static DialogConfigure *self_;
  Ui::DialogConfigure *ui;
  QSettings *cfg_;
};

#endif // DIALOGCONFIGURE_H
