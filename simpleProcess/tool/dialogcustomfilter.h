#ifndef DIALOGCUSTOMFILTER_H
#define DIALOGCUSTOMFILTER_H

#include <QDialog>
#include <QStyledItemDelegate>
#include <QSpinBox>
#include <QVector>

namespace Ui {
class DialogCustomFilter;
}

class DialogCustomFilter : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCustomFilter(QWidget *parent = 0);
    ~DialogCustomFilter();
    void getVectorData(std::vector<int> &data, int &w, int &h);

private slots:
    void on_spinBox_maskWidth_valueChanged(int arg1);

    void on_spinBox_maskHeight_valueChanged(int arg1);

private:
    void fillTableData();
    Ui::DialogCustomFilter *ui;
};

class SpinBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    SpinBoxDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // DIALOGCUSTOMFILTER_H
