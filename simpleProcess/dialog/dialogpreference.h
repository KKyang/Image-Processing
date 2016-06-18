#ifndef DIALOGPREFERENCE_H
#define DIALOGPREFERENCE_H

#include <QDialog>

namespace Ui {
class DialogPreference;
}

class DialogPreference : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPreference(QWidget *parent = 0);
    ~DialogPreference();
    int getRecoverLimit() const;

    void setRecoverLimit(int times);
private slots:
    void showWarningMessage(int num, bool isOrigin);
    void saveSettings();
    void restoreSettings();

    void on_spinBox_undo_limit_valueChanged(int arg1);

private:
    Ui::DialogPreference *ui;

    std::vector<bool> settingFlags;
    struct settings
    {
        int recoverLimit;
    }SimpleSettings;
};

#endif // DIALOGPREFERENCE_H
