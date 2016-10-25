#include "infodialog.h"
#include "ui_infodialog.h"
#include <QString>
#include "vc.h"

InfoDialog::InfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoDialog)
{
    ui->setupUi(this);
    ui->plainTextEdit->appendPlainText("");
    ui->plainTextEdit->appendPlainText(QString("Git Hash: ")+QString::number(GITHASH,16).toUpper());
    ui->plainTextEdit->appendPlainText(QString("Git Date: ") + GITDATE);
}

InfoDialog::~InfoDialog()
{
    delete ui;
}
