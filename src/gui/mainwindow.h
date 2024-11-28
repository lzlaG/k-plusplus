#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QThread>
#include <QString>
#include <QTreeView>

namespace Ui {
class MainWindow;
}

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
    //void AnekdotUpdate();
    void BlockButtons();
    void UnblockButtons();
    void on_searchButton_clicked();
    void handleModels(QStandardItemModel *model1, QStandardItemModel *model2);// Устанавливаем модель в QTreeView
    void on_progressBar_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    QStandardItemModel *originalKnownModel = new QStandardItemModel();
    QStandardItemModel *originalUnknownModel = new QStandardItemModel();
protected:
    QString ScanDir;
    QString NsrlFile;
    QThread *Thread;
};

#endif // MAINWINDOW_H
