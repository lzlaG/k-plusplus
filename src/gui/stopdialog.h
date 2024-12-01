#ifndef STOPDIALOG_H
#define STOPDIALOG_H

#include <QDialog>

namespace Ui {
class StopDialog;
}

class StopDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StopDialog(QWidget *parent = nullptr);
    ~StopDialog();

private:
    Ui::StopDialog *ui;
};

#endif // STOPDIALOG_H
