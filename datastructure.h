#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H
#include <QString>
#include <QTableWidget>

class dataStructure
{
public:
    dataStructure();

    void setTableData(QTableWidget *TableWidgetToFill);

private:
    QString startTime;
    QString finishTime;
    QString subjectName;
    QString subjectType;
    QString group;
    QStringList qslTableRowToSet;
};

#endif // DATASTRUCTURE_H
