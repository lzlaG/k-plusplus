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

    void on_SetupNsrlButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
protected:
    QStandardItemModel *KnownModel = new QStandardItemModel(this);
    QStandardItemModel *UnknownModel = new QStandardItemModel(this);
    QString ScanDir;
    QString NsrlFile;
    QThread *Thread;
};

#endif // MAINWINDOW_H
