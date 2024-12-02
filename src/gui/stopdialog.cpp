#include "stopdialog.h"
#include "ui_stopdialog.h"

StopDialog::StopDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StopDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Выберите способ продолжения анализа...");
    QPixmap pixmap("../../../src/gui/img/hamster25.jpg");
    // Масштабируем изображение под размер QFrame
    QPixmap scaledPixmap = pixmap.scaled(ui->frame->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    // Устанавливаем фон для QFrame через палитру
    QPalette palette = ui->frame->palette();
    palette.setBrush(QPalette::Window, QBrush(pixmap));
    ui->frame->setPalette(palette);
    ui->frame->setAutoFillBackground(true);

}

StopDialog::~StopDialog()
{
    delete ui;
}

void StopDialog::on_ContinueButton_clicked()
{
    emit WantJustContinue();
    accept();
}


void StopDialog::on_StartFromZeroButton_clicked()
{
    emit WantStartFromZero();
    accept();
}

