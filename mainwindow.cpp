#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QStringList>
#include <QString>
#include <QCheckBox>
#include <QListWidgetItem>

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
    groupBox = 0;
    listWidget = new QListWidget(this);
    listWidget->setVisible(false);
    connect(listWidget, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(on_GroupCheckBox_itemChanged(QListWidgetItem *)));
    CleanGroupCheckBtn = new QPushButton("&Clear selection", this);
    CleanGroupCheckBtn->setVisible(false);
    connect(CleanGroupCheckBtn, SIGNAL(clicked(bool)), this, SLOT(on_CleanGroupCheckBtn_onClick(bool)));
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
    ui->horizontalLayout->addWidget(createCheckboxGroupsView());
    dataInstance->testShowData();
    ui->WeekDayBox->setEnabled(true);
    if( 0 != ui->tableWidget )
    {
        //dataInstance->setTableData(ui->tableWidget, dataStructure::MON, this->groupFilter);
        updatePlanList();
    }
    else
    {
        qWarning() << "on_btnShowFilteredPlan_clicked() null pointer";
    }
    //ui->btnShowFilteredPlan->setEnabled(false);

}

void MainWindow::updatePlanList()
{
    loadDataForDay(ui->WeekDayBox->currentIndex());
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

QGroupBox *MainWindow::createCheckboxGroupsView()
{
    if(0 == groupBox)
    {
        groupBox = new QGroupBox(tr("Avaliable groups checkboxes"));
        groupBox->setFlat(true);

        QStringList groupLabels = dataInstance->getGroupsNamesList();
        QStringListIterator it(groupLabels);
        QVBoxLayout *vbox = new QVBoxLayout;
        while( it.hasNext() )
        {
            QListWidgetItem *listItem = new QListWidgetItem(it.next(),this->listWidget);
            listItem->setCheckState(Qt::Unchecked);
            listWidget->addItem(listItem);
        }
        listWidget->setFixedHeight(200);
        vbox->addWidget(listWidget);
        vbox->addWidget(CleanGroupCheckBtn);
        //vbox->addStretch(1);
        vbox->setSpacing(0);
        vbox->setMargin(0);
        vbox->setContentsMargins(0,0,0,0);

        groupBox->setLayout(vbox);
        listWidget->setVisible(true);
        CleanGroupCheckBtn->setVisible(true);
    }


    return groupBox;
}

void MainWindow::on_GroupCheckBox_itemChanged(QListWidgetItem *item)
{
    if(0 != item)
    {
        switch(item->checkState())
        {
        case Qt::Checked:
        {
            groupFilter.append(item->text());
            groupFilter.removeDuplicates();
            break;
        }
        case Qt::Unchecked:
        {
            groupFilter.removeOne(item->text());
            break;
        }
        default:
        {
            qDebug() << "on_GroupCheckBox default";
        }
        }
        updatePlanList();
    }
    else
    {
        qDebug() << "on_GroupCheckBox";
    }
}

void MainWindow::on_CleanGroupCheckBtn_onClick(bool fClicked)
{
    for(int iIter = 0; iIter < listWidget->count(); iIter++)
    {
        QListWidgetItem* item = listWidget->item(iIter);
        item->setCheckState(Qt::Unchecked);
    }
    qDebug() << "btn click execute";
}
