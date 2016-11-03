#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H
#include <QString>
#include <QTableWidget>
#include <fileoperatinos.h>

class dataStructure
{
public:
    dataStructure();
    enum eWeekDays {MON = 0, TUE, WED, THU, FRI, SAT, SUN, WEEKDAY_LASTITEM};
    static dataStructure& dsGetInstance()
    {
        static dataStructure singleton;
        return singleton;
    }

    bool setTableData(QTableWidget *TableWidgetToFill, int iDayIndex, QString qstrValidateFilter);
    bool prepareTableData();
    bool findAvaliableGroups();
    QStringList getListWeekDays();

    void testShowData();

private:
    //Avoid copy of singleton
    dataStructure(fileOperatinos &);      // Don't Implement
    void operator=(fileOperatinos const&); // Don't implement

    enum eStructElements { START_TIME=0, FINISH_TIME, SUBJECT_NAME, SUBJECT_TYPE, GROUPS, DATA_LAST_ITEM };
    struct sPlanElements
    {
        QString startTime;
        QString finishTime;
        QString subjectName;
        QString subjectType;
        QString groups;
        QString additionalInfo;
    };

    fileOperatinos *foInstance;
    QStringList qstrl_avaliableGroups;
    const QString tab_strWeekDays[WEEKDAY_LASTITEM] = {"poniedzialek", "wtorek", "sroda", "czwartek", "piatek", "sobota", "niedziela"};
    QList <sPlanElements> tab_listOfStructLineElements[WEEKDAY_LASTITEM];

    bool populateWeek(QStringList *listWeekDayOneLineData);
    eWeekDays whichDay(QStringList *listToAnalyse);
    bool populatWeekDayWithData(eWeekDays currentDay, QStringList *listWeekDayOneLineData);
    void CleanPlanDataList();

};

#endif // DATASTRUCTURE_H
