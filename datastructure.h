#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H
#include <QString>
#include <QTableWidget>
#include <fileoperatinos.h>

class dataStructure
{
public:
    dataStructure();

    void setTableData(QTableWidget *TableWidgetToFill);
    bool prepareTableData();
    void testShowData();

private:
    fileOperatinos *foInstance;
    struct sLineElements
    {
        QString weekDay;
        QString startTime;
        QString finishTime;
        QString subjectName;
        QString subjectType;
        QString groups;
    };
    QList <sLineElements> list_sLineElements;
    QStringList qstrl_avaliableGroups;

    enum eStructElements { START_TIME=0, FINISH_TIME, SUBJECT_NAME, SUBJECT_TYPE, GROUPS, DATA_LAST_ITEM };

    bool fillStruct(sLineElements *structToFill, QStringList *listOneLine);
    bool findAvaliableGroups();
};

#endif // DATASTRUCTURE_H
