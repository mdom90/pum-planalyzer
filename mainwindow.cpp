#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QStringList>
#include <QString>
//
#include <QDebug>
//exec pdf converter
#include <QProcess>
#include <QObject>

#include <windef.h>
#include <winnt.h>
#include <winbase.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::convertPdf()
{
    QObject *parent = 0;
    //todo check if not empty
    QString document = this->pdfFilePath;
    QString program = "pdfconvert/pdftotext.exe";
    QStringList arguments;
    QString outputDir;

    QProcess *myProcess = new QProcess(parent);
    outputDir = QCoreApplication::applicationDirPath();
    outputDir += "/pdfconvert/output.txt";
    this->filePath = outputDir;
    arguments << "-table" << "-enc" << "UTF-8" << document << outputDir;

    myProcess->execute(program, arguments);
    qDebug() << "source file: " << document;
    qDebug() << "args: " << arguments;
    qDebug() << "end: " << myProcess->exitStatus();
}

void MainWindow::on_btnShowFilteredPlan_clicked()
{
    convertPdf();
    loadTextFile(filePath);
}

void MainWindow::updatePlanList()
{

}

void MainWindow::loadTextFile(QString textFilePath)
{
    QFile inputFile(textFilePath);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       qint16 i16counter = 0;
       QTextStream in(&inputFile);
       //QTableWidget *tableView = ui->tableWidget;
       QRegularExpression regexp("\\s+");
       while (!in.atEnd())
       {
          i16counter++;
          QString line = in.readLine();
          QStringList LstRowData = line.split(regexp);
          qDebug() << "line: " << i16counter << LstRowData;
       }
       inputFile.close();
    }
}
