#include "dialogcustomfilter.h"
#include "ui_dialogcustomfilter.h"
#include <iostream>

DialogCustomFilter::DialogCustomFilter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCustomFilter)
{
    ui->setupUi(this);

    ui->tableWidget_maskInput->setItemDelegate(new SpinBoxDelegate);
    ui->tableWidget_maskInput->setColumnCount(ui->spinBox_maskWidth->value());
    ui->tableWidget_maskInput->setRowCount(ui->spinBox_maskHeight->value());
    ui->tableWidget_maskInput->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tableWidget_maskInput->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tableWidget_maskInput->horizontalHeader()->setDefaultSectionSize(80);
    ui->tableWidget_maskInput->verticalHeader()->setDefaultSectionSize(80);
    fillTableData();
}

DialogCustomFilter::~DialogCustomFilter()
{
    delete ui;
}

void DialogCustomFilter::getVectorData(std::vector<int> &data, int &w, int &h)
{
    data.resize(ui->spinBox_maskWidth->value()*ui->spinBox_maskHeight->value());
    int i = 0;
    #pragma omp parallel for private(i)
    for(int j = 0 ; j < ui->tableWidget_maskInput->rowCount();j++)
        for(i = 0 ; i < ui->tableWidget_maskInput->columnCount();i++)
        {
            data[j*ui->spinBox_maskWidth->value()+i]=ui->tableWidget_maskInput->item(j,i)->text().toInt();
        }
    w = ui->spinBox_maskWidth->value();
    h = ui->spinBox_maskHeight->value();
}

void DialogCustomFilter::fillTableData()
{
    int i = 0;
    #pragma omp parallel for private(i)
    for(int j = 0 ; j < ui->tableWidget_maskInput->rowCount();j++)
        for(i = 0 ; i < ui->tableWidget_maskInput->columnCount();i++)
        {
            if(!ui->tableWidget_maskInput->item(j,i))
            {
                ui->tableWidget_maskInput->setItem(j,i, new QTableWidgetItem);
                ui->tableWidget_maskInput->item(j,i)->setText("1");
                ui->tableWidget_maskInput->item(j,i)->setTextAlignment(Qt::AlignCenter);
            }

        }
}

void DialogCustomFilter::on_spinBox_maskWidth_valueChanged(int arg1)
{
    if(arg1%2==0)
    {
        arg1 = arg1-1;
        ui->spinBox_maskHeight->setValue(arg1);
    }
    ui->tableWidget_maskInput->setColumnCount(arg1);
    fillTableData();

}

void DialogCustomFilter::on_spinBox_maskHeight_valueChanged(int arg1)
{
    if(arg1%2==0)
    {
        arg1 = arg1-1;
        ui->spinBox_maskWidth->setValue(arg1);
    }
    ui->tableWidget_maskInput->setRowCount(arg1);
    fillTableData();
}

SpinBoxDelegate::SpinBoxDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const
{
    QSpinBox *editor = new QSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(-100);
    editor->setMaximum(100);
    editor->setAlignment(Qt::AlignCenter);

    return editor;
}

void SpinBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    int value = index.model()->data(index, Qt::EditRole).toInt();

    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->interpretText();
    int value = spinBox->value();

    model->setData(index, value, Qt::EditRole);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(QRect(option.rect.x(),option.rect.y(),80,80));
}
