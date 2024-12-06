#ifndef STOPDIALOG_H
#define STOPDIALOG_H

#include <QDialog>

namespace Ui
{
    class StopDialog;
}

/**
 * Класс диалогово окна
 */
class StopDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StopDialog(QWidget *parent = nullptr);
    ~StopDialog();

private slots:
    void on_ContinueButton_clicked();

    void on_StartFromZeroButton_clicked();

private:
    Ui::StopDialog *ui;
signals:
    void WantJustContinue();
    void WantStartFromZero();
};

#endif // STOPDIALOG_H
