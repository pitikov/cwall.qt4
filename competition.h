#ifndef COMPETITION_H
#define COMPETITION_H

#include <QObject>
#include <QStandardItemModel>

class Competition : public QObject
{
    Q_OBJECT

public:
    Competition();
    ~Competition();
    virtual bool event(QEvent* );
    virtual bool eventFilter(QObject* , QEvent* );

public Q_SLOTS:

private:
    QStandardItemModel model_struct;
    QStandardItemModel model_members;
    QString description;
    QString title;
};

#endif // COMPETITION_H
