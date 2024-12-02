#ifndef SLAVE_H
#define SLAVE_H

#include <QObject>
#include <QString>
#include <QStandardItemModel>
#include <QTreeView>
#include <QCoreApplication>
#include "../../models/fileSchema.h"
#include "../nsrlRepository/nsrlRepository.h"
#include "../OutputDB/outputDB.h"
#include <QMutex>
#include <QWaitCondition>

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
    void requestPause();
private:
    QString ScanDir;
    QString NsrlFile;
    void ReadFiles(QString NsrlFile, QString ScanDir);
    void GetDataFromDB(OutputDB& ourDatabase);
    void GuiMultiHashes(vector<FilePtr>& files, int start, int end, NSRLRepository& nsrlRepo, OutputDB& ourDatabase);
    atomic<bool> m_pauseRequested;
    QMutex m_mutex;
    QWaitCondition m_waitCondition;
public slots:
    void doWork();
signals:
    void finished();
    int ProgressUpdated(int value);
    int ChangeRange(int value);
    void WorkStart();
    void ModelsReady(QStandardItemModel* model1, QStandardItemModel *model2);
};

#endif // SLAVE_H
