#include "infodialog.h"
#include "ui_infodialog.h"
#include <QString>
#include "vc.h"
#include "rpc_transmission/client/generated_app/RPC_TRANSMISSION_qt2mcu.h"

InfoDialog::InfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoDialog)
{
    ui->setupUi(this);
    ui->plainTextEdit->appendPlainText("");
    ui->plainTextEdit->appendPlainText(QString("Build:  ")+__DATE__+"    "+__TIME__);
    ui->plainTextEdit->appendPlainText(QString("Git Hash: ")+QString::number(GITHASH,16).toUpper());
    ui->plainTextEdit->appendPlainText(QString("Git Date: ") + GITDATE);
    ui->plainTextEdit->appendPlainText("");
    QByteArray rpc_hash(RPC_TRANSMISSION_HASH);
    ui->plainTextEdit->appendPlainText("RPC Hash: 0x"+rpc_hash.toHex().toUpper());
}

InfoDialog::~InfoDialog()
{
    delete ui;
}
