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
                   QStandardItemModel *knownmodel,
                   QStandardItemModel *unknownmodel,
                   QTreeView *knownview,
                   QTreeView *unknownview,
                   QObject *parent = nullptr)
    {
        ScanDir = scandir;
        NsrlFile = nsrlfile;
        KnownModel = knownmodel;
        UnknownModel = unknownmodel;
        KnownView = knownview;
        UnknownView = unknownview;
    };
private:
    QStandardItemModel *KnownModel;
    QStandardItemModel *UnknownModel;
    QTreeView *KnownView;
    QTreeView *UnknownView;
    QString ScanDir;
    QString NsrlFile;
    QString ReadFiles(QString NsrlFile, QString ScanDir);
    void FillTreeView(QTreeView* treeView,
                      QStandardItemModel *neededModel,
                      const char* queryStr,
                      QString DB_path);
public slots:
    void doWork();
signals:
    void finished();
    int ProgressUpdated(int value);
    int ChangeRange(int value);
};

#endif // SLAVE_H
