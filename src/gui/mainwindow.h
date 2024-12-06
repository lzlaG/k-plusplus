#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QThread>
#include <QString>
#include <QTreeView>
#include "slave.h"

namespace Ui
{
    class MainWindow;
}

/**
 * Класс Основного окна
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_SetupDirButton_clicked();
    void updateProgress(int value);
    void RangeUpdate(int value);
    void on_SetupNsrlButton_clicked();
    void on_pushButton_clicked();
    void BlockButtons();
    void UnblockButtons();
    void on_searchButton_clicked();
    void handleModels(QStandardItemModel *model1, QStandardItemModel *model2); // Устанавливаем модель в QTreeView
    void on_progressBar_valueChanged(int value);
    void on_stopButton_clicked();
    void ContinueAfterResume();
    void StartFromZeroAfterPause();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *originalKnownModel = new QStandardItemModel();
    QStandardItemModel *originalUnknownModel = new QStandardItemModel();
    bool IsPaused = false;

protected:
    QString ScanDir;
    QString NsrlFile;
    QThread *Thread;
    Slave *slave1;
};

#endif // MAINWINDOW_H
