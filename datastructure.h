#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H
#include <QString>
#include <QTableWidget>
#include <fileoperatinos.h>

class dataStructure
{
public:
    dataStructure();

    bool setTableData(QTableWidget *TableWidgetToFill);
    bool prepareTableData();
    void testShowData();

private:
    enum eWeekDays {MON = 0, TUE, WED, THU, FRI, SAT, SUN, WEEKDAY_LASTITEM};
    enum eStructElements { START_TIME=0, FINISH_TIME, SUBJECT_NAME, SUBJECT_TYPE, GROUPS, DATA_LAST_ITEM };
    struct sLineElements
    {
        QString weekDay;
        QString startTime;
        QString finishTime;
        QString subjectName;
        QString subjectType;
        QString groups;
    };

    fileOperatinos *foInstance;
    QList <sLineElements> list_sLineElements;
    QStringList qstrl_avaliableGroups;
    QString tab_strWeekDays[WEEKDAY_LASTITEM] = {"poniedziałek", "wtorek", "środa", "czwartek", "piątek", "sobota", "niedziela"};
    QList <sLineElements> tab_listOfStructLineElements[DATA_LAST_ITEM];

    bool fillStruct(sLineElements *structToFill, QStringList *listOneLine);
    bool findAvaliableGroups();
};

#endif // DATASTRUCTURE_H
