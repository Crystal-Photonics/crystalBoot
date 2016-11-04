#ifndef OPTIONS_H
#define OPTIONS_H

#include <QDialog>
#include "crystalsettings.h"

namespace Ui {
class OptionsDiag;
}

class OptionsDiag : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDiag(CrystalBootSettings *settings, QWidget *parent = 0);
    ~OptionsDiag();
private slots:
    void on_buttonBox_accepted();

private:
    Ui::OptionsDiag *ui;
    CrystalBootSettings *settings;
    void loadUIFromSettings();
};

#endif // OPTIONS_H
