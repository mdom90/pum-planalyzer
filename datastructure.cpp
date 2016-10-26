#include "datastructure.h"
#include <QDebug>

dataStructure::dataStructure()
{
    foInstance = &fileOperatinos::foGetInstance();
//    for(int iDayIter = MON; iDayIter < WEEKDAY_LASTITEM; iDayIter++)
//    {
//        tab_listOfStructLineElements[iDayIter] = new (std::nothrow) QList<sPlanElements>;
//    }
}

bool dataStructure::prepareTableData()
{
    bool fRetVal = false;
    if(0 != foInstance )
    {
        QString strOrderData;
        if( foInstance->foLoadOrderDataFile(&strOrderData) )
        {
            //File loaded in memory
            QStringList strListOrderData = strOrderData.split("\r\n", QString::SkipEmptyParts); //file listed by new line elements
            QStringList strListOneDayLineData;
            qDebug() << "PrepareTableData nr of new lines: " << strListOrderData.size();
            for(int OrderIter = 0; OrderIter < strListOrderData.size(); ++OrderIter)
            {
                strListOneDayLineData = strListOrderData[OrderIter].split(";");//line listed in ";" separated elements
                fRetVal &= populateWeek(&strListOneDayLineData);//parametr - elements from one line in list
            }
        }
        findAvaliableGroups();
    }
    return fRetVal;
}

bool dataStructure::populateWeek(QStringList *listWeekDayOneLineData)
{
    bool fRetVal = false;

    if( (0 != listWeekDayOneLineData) )
    {
        if( false == listWeekDayOneLineData->isEmpty() )
        {
            fRetVal = true;
            static eWeekDays eDayIter; // Need to keep value for else case
            int listSize = listWeekDayOneLineData->size();
            if( (listSize < 2) )//Week day. Enter once per day
            {
                eDayIter = whichDay(listWeekDayOneLineData);//Set current analysed week day
            }
            else
            {

                fRetVal = populatWeekDayWithData(eDayIter, listWeekDayOneLineData);
            }
        }
    }
    return fRetVal;
}

//Return current analysed day. WEEKDAY_LASTITEM - in case of failure
dataStructure::eWeekDays dataStructure::whichDay(QStringList *listToAnalyse)
{
    int iDayIter = 0;
    if( 0 != listToAnalyse )
    {
        QString qstrDayFromList = "";
        for( iDayIter = MON; iDayIter < WEEKDAY_LASTITEM; iDayIter++ )
        {
            if( (0 == this->tab_strWeekDays[iDayIter].compare(listToAnalyse->at(0), Qt::CaseInsensitive)) ) //compare data from order file with week days
            {
                qDebug() << "WhichDay() iter: " << tab_strWeekDays[iDayIter];
                break;
            }
        }
        if( iDayIter > WEEKDAY_LASTITEM)
        {
            iDayIter = WEEKDAY_LASTITEM;//In case no match don't iterate more
        }
    }
    else
    {
        iDayIter = WEEKDAY_LASTITEM;
    }
    eWeekDays retDay = static_cast<eWeekDays>(iDayIter);
    return retDay;
}

bool dataStructure::populatWeekDayWithData(eWeekDays currentDay, QStringList *listWeekDayOneLineData)
{
    bool fRetVal = false;
    if( (0 != listWeekDayOneLineData) && (currentDay < WEEKDAY_LASTITEM) )
    {
        fRetVal = true;
        sPlanElements sOneDayPlan;
        qDebug() << "Populate week: day: " << currentDay << "data size: " << listWeekDayOneLineData->size();
        for(int Iter = 0; Iter < listWeekDayOneLineData->size(); ++Iter)
        {
            switch (Iter)
            {
            case START_TIME:
            {
                sOneDayPlan.startTime = listWeekDayOneLineData->at(Iter);
                break;
            }
            case FINISH_TIME:
            {
                sOneDayPlan.finishTime = listWeekDayOneLineData->at(Iter);
                break;
            }
            case SUBJECT_NAME:
            {
                sOneDayPlan.subjectName = listWeekDayOneLineData->at(Iter);
                break;
            }
            case SUBJECT_TYPE:
            {
                sOneDayPlan.subjectType = listWeekDayOneLineData->at(Iter);
                break;
            }
            case GROUPS:
            {
                sOneDayPlan.groups = listWeekDayOneLineData->at(Iter);
                break;
            }
            default:
            {
                qDebug() << "Fill struct default!!";
                break;
            }
            }
        }
        this->tab_listOfStructLineElements[currentDay].append(sOneDayPlan);//Create plan for current day
    }
    return fRetVal;
}

bool dataStructure::setTableData(QTableWidget *TableWidgetToFill)
{
    bool fRetVal = false;
    /*
        if( 0 != TableWidgetToFill )
        {
            TableWidgetToFill->setRowCount(this->list_sPlanElements.size());
            for(int i=0; i < this->list_sPlanElements.size(); i++)
            {
                TableWidgetToFill->setItem(i, START_TIME, new QTableWidgetItem(list_sPlanElements[i].startTime));
                TableWidgetToFill->setItem(i, FINISH_TIME, new QTableWidgetItem(list_sPlanElements[i].finishTime));
                TableWidgetToFill->setItem(i, SUBJECT_NAME, new QTableWidgetItem(list_sPlanElements[i].subjectName));
                TableWidgetToFill->setItem(i, SUBJECT_TYPE, new QTableWidgetItem(list_sPlanElements[i].subjectType));
                TableWidgetToFill->setItem(i, GROUPS, new QTableWidgetItem(list_sPlanElements[i].groups));
            }
        }
        else
        {
            qWarning() << "Null TableWidget in setTableData()";
        }
    */
    return fRetVal;
}

bool dataStructure::findAvaliableGroups()
{
    bool fRetVal = false;

    return fRetVal;
}

void dataStructure::testShowData()
{
    for(int iDay = MON; iDay < WEEKDAY_LASTITEM; iDay++)
    {
        qDebug() << "Day: " << this->tab_strWeekDays[iDay];
        for(int i=0; i < this->tab_listOfStructLineElements[iDay].size(); i++)
        {
            qDebug() << "Od: " << this->tab_listOfStructLineElements[iDay].at(i).startTime << "Do: " << this->tab_listOfStructLineElements[iDay].at(i).finishTime \
                     << "Sub: " << this->tab_listOfStructLineElements[iDay].at(i).subjectName << "Group: " << this->tab_listOfStructLineElements[iDay].at(i).groups;
        }
    }
}
