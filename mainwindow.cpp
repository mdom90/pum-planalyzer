#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QStringList>
#include <QString>
//
#include <QDebug>

#include <windef.h>
#include <winnt.h>
#include <winbase.h>

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
}

MainWindow::~MainWindow()
{
    delete ui;
    foInstance = 0;
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
    bool fResult = false;
    fResult = foInstance->foPrepareFiles(this->pdfFilePath);
    dataInstance.prepareTableData();
    dataInstance.testShowData();
    dataInstance.setTableData(ui->tableWidget);
}

void MainWindow::updatePlanList()
{

}

void MainWindow::uiConfigTable()
{
    QTableWidget *qtablew = ui->tableWidget;

    qtablew->setColumnCount(tableCol);
    //qtablew->setHorizontalHeaderLabels(tableHeaders);
    qtablew->horizontalHeader()->setStretchLastSection(true);//fit header to table size
    qtablew->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    qtablew->setEditTriggers(QAbstractItemView::NoEditTriggers);
}
