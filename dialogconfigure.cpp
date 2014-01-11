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

bool DialogConfigure::defaultDatabase(QString* host, QString* base, QString* user, QString* passwd, int* port)
{
	QString thost;
	bool ret = false;
	int hostcount = settings()->beginReadArray(DbSettings);
	for (int chost = 0; chost < hostcount; chost++) {
		settings()->setArrayIndex(chost);
		thost = settings()->value("hostname", "localhost").toString();

		int basecount = settings()->beginReadArray(DbItems);

		for (int cbase = 0; cbase < basecount; cbase++) {
			settings()->setArrayIndex(cbase);

			if (settings()->value("is_default", false).toBool()) {
				*host = thost;
				*base = settings()->value("database").toString();
				*user = settings()->value("user").toString();
				*passwd = settings()->value("password").toString();
				*port = 3306;

				ret = true;
				break;
			}
		}

		settings()->endArray();
		if (ret) break;
	}

	settings()->endArray();

	return ret;
}

int DialogConfigure::exec()
{

	return QDialog::exec();
}

#include "dialogconfigure.moc"