#include "dialogpreference.h"
#include "ui_dialogpreference.h"
#include "iostream"

DialogPreference::DialogPreference(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPreference)
{
    ui->setupUi(this);
    ui->label_warning->setVisible(false);
    connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(this, SIGNAL(rejected()), this, SLOT(restoreSettings()));

    settingFlags.resize(1,1);
}

DialogPreference::~DialogPreference()
{
    disconnect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));
    disconnect(this, SIGNAL(rejected()), this, SLOT(restoreSettings()));
    delete ui;
}

void DialogPreference::saveSettings()
{
    SimpleSettings.recoverLimit = ui->spinBox_undo_limit->value();
}

void DialogPreference::restoreSettings()
{
    ui->spinBox_undo_limit->setValue(SimpleSettings.recoverLimit);
}

int DialogPreference::getRecoverLimit() const
{
    return ui->spinBox_undo_limit->value();
}

void DialogPreference::setRecoverLimit(int times)
{
    SimpleSettings.recoverLimit = times;
    ui->spinBox_undo_limit->setValue(times);
}

//Warning settings
void DialogPreference::showWarningMessage(int num, bool isOrigin)
{
    bool warning = 0;
    settingFlags[num] = isOrigin;

    for(int i = 0; i < settingFlags.size(); i++)
    {
        if(settingFlags[i] == 0)
        {
            warning = 1;
            break;
        }
    }

    if(warning){ui->label_warning->setVisible(true);}
    else{ui->label_warning->setVisible(false);}
}

void DialogPreference::on_spinBox_undo_limit_valueChanged(int arg1)
{
    if(arg1 != SimpleSettings.recoverLimit)
        showWarningMessage(0, 0);
    else
        showWarningMessage(0, 1);
}
