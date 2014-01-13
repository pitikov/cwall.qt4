#include "dialogconfigure.h"
#include <QCryptographicHash>

#include <QDebug>
#include <QDir>
#include <QFileDialog>

DialogConfigure *DialogConfigure::self_ = NULL;

DialogConfigure::DialogConfigure(QWidget* parent, Qt::WindowFlags f)
: QDialog(parent, f)
, ui(new Ui::DialogConfigure)
, cfg_(new QSettings(QSettings::IniFormat, QSettings::UserScope, "ClimbingWall", qAppName(), this))
{
	ui->setupUi(this);

	connect(ui->lineEditOrganization, SIGNAL(textChanged(QString)), SLOT(serialValidate()));
	connect(ui->lineEditName, SIGNAL(textChanged(QString)), SLOT(serialValidate()));
	connect(ui->lineEditEmail, SIGNAL(textChanged(QString)), SLOT(serialValidate()));
	connect(ui->lineEditSerial, SIGNAL(textChanged(QString)), SLOT(serialValidate()));
}

void DialogConfigure::accept()
{
	setValue("SQL_Files", "dbcore", ui->lineEditSqlCore->text());
	setValue("PDF_Files", "rules", ui->lineEditRules->text());
	setValue("Output_Files", "target", ui->lineEditOutputDir->text());

	setValue("registration", "organization", ui->lineEditOrganization->text());
	setValue("registration", "email", ui->lineEditEmail->text());
	setValue("registration", "name", ui->lineEditName->text());
	if (registration()) setValue("registration", "serial", ui->lineEditSerial->text());

	settings()->sync();
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
	settings()->sync();
	ui->tabWidget->setCurrentWidget(ui->tabFiles);
	ui->lineEditSqlCore->setText(value("SQL_Files", "dbcore", "/usr/share/cwall/sql/schema.mysql.sql").toString());
	ui->lineEditRules->setText(value("PDF_Files", "rules", "/usr/share/cwall/pdf/rules2012.pdf").toString());
	ui->lineEditOutputDir->setText(value("Output_Files", "target", QDir::homePath() + "/climbing_wall").toString());

	ui->lineEditEmail->setText(value("Registration", "email").toString());
	ui->lineEditOrganization->setText(value("Registration", "organization").toString());
	ui->lineEditName->setText(value("Registration", "name").toString());
	ui->lineEditSerial->setText(value("Registration", "serial").toString());

	serialValidate();
	if (!registration()) ui->lineEditSerial->clear();

	return QDialog::exec();
}

bool DialogConfigure::registration()
{
	bool ret = true;
	QString ctrlsumm;

	for (int i = 0; i<25; i++) {
		ctrlsumm += QCryptographicHash::hash(QString(ui->lineEditOrganization->text()+ui->lineEditEmail->text()+ui->lineEditName->text()).toUtf8(), QCryptographicHash::Md5).toHex().toUpper().left(25)[i];
		if ((i==4)|(i==9)|(i==14)|(i==19)) ctrlsumm+="-";
	}
	//qDebug() << ctrlsumm;
	return (ctrlsumm == ui->lineEditSerial->text());
}

void DialogConfigure::serialValidate()
{
	if (registration()) {
		ui->labelRegStatus->setPixmap(QPixmap("/usr/share/icons/default.kde4/32x32/actions/view-certificate.png"));
	} else {
		ui->labelRegStatus->setPixmap(QPixmap("/usr/share/icons/default.kde4/32x32/actions/dialog-cancel.png"));
	}
}

void DialogConfigure::setOutputPath()
{
	QString path = QFileDialog::getExistingDirectory(this, tr("Documentation output"), ui->lineEditOutputDir->text());
	if (!path.isEmpty()) {
		ui->lineEditOutputDir->setText(path);
	}
}

void DialogConfigure::setRulesPath()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Competition rules pdf"), ui->lineEditRules->text(), "PDF file (*.PDF *.pdf)");
	if (!path.isEmpty()) {
		ui->lineEditRules->setText(path);
	}
}

void DialogConfigure::setSqlPath()
{
	QString path = QFileDialog::getOpenFileName(this, tr("SQL core database script"), ui->lineEditSqlCore->text(), "SQL scripts (*.SQL *.sql)");
	if (!path.isEmpty()) {
		ui->lineEditSqlCore->setText(path);
	}
}

bool DialogConfigure::rulseAccess()
{
	bool ret = false;
	QFile rules(value("PDF_Files", "rules", "/usr/share/cwall/pdf/rules2012.pdf").toString());
	if (rules.open(QIODevice::ReadOnly)) {
		ret = true;
		rules.close();
	}
	return ret;
}

#include "dialogconfigure.moc"