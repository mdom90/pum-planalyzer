#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "fileoperatinos.h"
#include "datastructure.h"
#include <QDesktopWidget>
#include <QGroupBox>
#include <QListWidgetItem>
#include <QPushButton>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btnLoadFile_pressed();
    void on_btnShowFilteredPlan_clicked();

    void on_WeekDayBox_currentIndexChanged(int index);
    void on_GroupCheckBox_itemChanged(QListWidgetItem * item);
    void on_CleanGroupCheckBtn_onClick(bool fClicked);

    void on_LoadedFileTextBrowser_textChanged();

private:
    Ui::MainWindow *ui;
    fileOperatinos *foInstance;
    dataStructure *dataInstance;
    QListWidget *listWidget;
    QGroupBox *groupBox;
    QList<QListWidgetItem *> qlstSelectedGroups;
    QPushButton *CleanGroupCheckBtn;

    const qint8 tableCol = 5;
    const QStringList tableHeaders = {"Od", "Do", "Przemdiot", "Rodzaj", "Grupa // Gdzie"};
    QString filePath;
    QString pdfFilePath;
    QStringList groupFilter;

    void updatePlanList();
    void uiConfigTable();
    void initDayFilter();
    void loadDataForDay(int iDayIndex);
    void cleanTableData();
    void cleanListWidget();//groups

    QGroupBox *createCheckboxGroupsView();
};

#endif // MAINWINDOW_H
