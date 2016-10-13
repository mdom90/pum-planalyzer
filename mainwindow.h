#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
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

    void convertPdf();
    void loadTextFile(QString qstrFilePath);
    void updatePlanList();

    QString filePath;
    QString pdfFilePath;
};

#endif // MAINWINDOW_H
