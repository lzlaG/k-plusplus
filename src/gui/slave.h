#ifndef SLAVE_H
#define SLAVE_H

#include <QObject>
#include <QString>
#include <QStandardItemModel>
#include <QTreeView>
#include <QCoreApplication>

class Slave : public QObject
{
    Q_OBJECT
public:
    explicit Slave(QString scandir,
                   QString nsrlfile,
                   QObject *parent = nullptr)
    {
        ScanDir = scandir;
        NsrlFile = nsrlfile;
    };
private:
    QString ScanDir;
    QString NsrlFile;
    QString ReadFiles(QString NsrlFile, QString ScanDir);
    void GetDataFromDB(QString DB_path);
public slots:
    void doWork();
signals:
    void finished();
    int ProgressUpdated(int value);
    int ChangeRange(int value);
    void AnekdotTime();
    void WorkStart();
    void ModelsReady(QStandardItemModel* model1, QStandardItemModel *model2);
};

#endif // SLAVE_H
