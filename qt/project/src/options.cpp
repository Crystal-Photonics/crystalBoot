#include "options.h"
#include "ui_optionsdiag.h"

OptionsDiag::OptionsDiag(CrystalBootSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDiag)
{
    ui->setupUi(this);
    this->settings = settings;
    loadUIFromSettings();
}



OptionsDiag::~OptionsDiag()
{
    delete ui;
}

void OptionsDiag::loadUIFromSettings()
{
    ui->edtArguments->setText(settings->callProcessArguments);
    ui->edtProcess->setText(settings->callProcessPath);
    ui->chbRunProcess->setChecked(settings->callProcessAfterProgrammuing);
    ui->edtResultPath->setText(settings->flashResultDocumentationPath);
}


void OptionsDiag::on_buttonBox_accepted()
{
    settings->callProcessArguments = ui->edtArguments->text();
    settings->callProcessPath = ui->edtProcess->text();
    settings->callProcessAfterProgrammuing = ui->chbRunProcess->isChecked();
    settings->flashResultDocumentationPath = ui->edtResultPath->text();
}
