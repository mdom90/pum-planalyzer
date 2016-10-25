#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "fileoperatinos.h"
#include "datastructure.h"
#include <QDesktopWidget>

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

private:
    Ui::MainWindow *ui;
    fileOperatinos *foInstance;
    dataStructure dataInstance;

    const qint8 tableCol = 5;
    const QStringList tableHeaders = {"Od", "Do", "Przemdiot", "Rodzaj", "Grupa // Gdzie"};
    QString filePath;
    QString pdfFilePath;

    void updatePlanList();
    void uiConfigTable();
};

#endif // MAINWINDOW_H
