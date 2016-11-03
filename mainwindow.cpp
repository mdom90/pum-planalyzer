#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QStringList>
#include <QString>
#include <QCheckBox>

#include <QDebug>

#include <QTableWidget>
#include <QHeaderView>//to streach headers in table

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
    uiConfigTable();
    foInstance = &fileOperatinos::foGetInstance();
    dataInstance = &dataStructure::dsGetInstance();
    initDayFilter();
    //configGroupFilter();
}

MainWindow::~MainWindow()
{
    delete ui;
    //foInstance = 0;
}

void MainWindow::on_btnLoadFile_pressed()
{
    QStringList filters;
    QStringList selectedFile;

    filters << "PDF files (*.pdf)";
    QFileDialog dialog(this);
    dialog.setNameFilters(filters);
    dialog.setFileMode(QFileDialog::ExistingFile);
    if(dialog.exec())
    {
        QString filePath;
        selectedFile = dialog.selectedFiles();
        filePath = selectedFile.join("");
        if( !filePath.isEmpty() )
        {
            ui->LoadedFileTextBrowser->setPlainText(filePath);
            pdfFilePath = filePath;
        }
    }
}

void MainWindow::on_btnShowFilteredPlan_clicked()
{
    //Instance of fileOperaion
    //bool fResult = false;
    cleanTableData();
    foInstance->foPrepareFiles(this->pdfFilePath);
    dataInstance->prepareTableData();
    dataInstance->findAvaliableGroups();
    dataInstance->testShowData();
    ui->WeekDayBox->setEnabled(true);
    if( 0 != ui->tableWidget )
    {
        dataInstance->setTableData(ui->tableWidget, dataStructure::MON, this->groupFilter);
    }
    else
    {
        qWarning() << "on_btnShowFilteredPlan_clicked() null pointer";
    }
    //ui->btnShowFilteredPlan->setEnabled(false);

}

void MainWindow::updatePlanList()
{
}

void MainWindow::uiConfigTable()
{
    QTableWidget *qtablew = ui->tableWidget;
    if( 0 != qtablew )
    {
        qtablew->setColumnCount(tableCol);
        //qtablew->setHorizontalHeaderLabels(tableHeaders);
        qtablew->horizontalHeader()->setStretchLastSection(true);//fit header to table size
        qtablew->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        qtablew->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
    else
    {
        qWarning() << "uiConfigTable() null pointer";
    }
}

void MainWindow::initDayFilter()
{
    ui->WeekDayBox->addItems(dataInstance->getListWeekDays());
}

void MainWindow::on_WeekDayBox_currentIndexChanged(int index)
{
    loadDataForDay(index);
}

void MainWindow::loadDataForDay(int iDayIndex)
{
    cleanTableData();
    dataInstance->setTableData(ui->tableWidget, iDayIndex, this->groupFilter);
}

void MainWindow::cleanTableData()
{
    ui->tableWidget->setRowCount(0);
}

void MainWindow::on_GroupFilter_lineEdit_selectionChanged()
{
    ui->GroupFilter_lineEdit->clear();
}

void MainWindow::configGroupFilter()
{
    QRegExp rx("([a-z0-9]{1,4}\|?)");
    QValidator *validator = new QRegExpValidator(rx, this);
    ui->GroupFilter_lineEdit->setValidator(validator);

}

void MainWindow::on_GroupFilter_lineEdit_editingFinished()
{
    this->groupFilter = ui->GroupFilter_lineEdit->displayText();
    this->loadDataForDay(ui->WeekDayBox->currentIndex());
}
