#include "stopdialog.h"
#include "ui_stopdialog.h"

StopDialog::StopDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StopDialog)
{
    ui->setupUi(this);
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
