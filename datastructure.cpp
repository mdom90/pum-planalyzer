#include "datastructure.h"
#include <QDebug>

dataStructure::dataStructure()
{
    foInstance = &fileOperatinos::foGetInstance();
}

bool dataStructure::prepareTableData()
{
    bool fRetVal = false;
    CleanPlanDataList();
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
            QString qstrCompare1 = tab_strWeekDays[iDayIter];
            QString qstrCompare2 = listToAnalyse->at(0);
            int iCompareResult = qstrCompare1.compare(qstrCompare2, Qt::CaseInsensitive);
            qDebug() << "WhichDay() compare: " << tab_strWeekDays[iDayIter] << listToAnalyse->at(0) << "res:" << iCompareResult;

            if( 0 == iCompareResult ) //compare data from order file with week days
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
                //TODO cleen up in here
                QStringList qstrlTempGroups;
                QStringList qstrlLineWithoutSpaces;
                QRegularExpression regexpSpecialCase("\\d{1} [a-zA-Z]");//hit "2 grupy" "1 grupa"

                if( true == listWeekDayOneLineData->at(Iter).contains(regexpSpecialCase) )
                {
                    sOneDayPlan.additionalInfo = listWeekDayOneLineData->at(Iter);
                    break;
                }
                qstrlTempGroups = listWeekDayOneLineData->at(Iter).split(",",QString::SkipEmptyParts);
                for(int iGroupIter = 0; iGroupIter < qstrlTempGroups.size(); iGroupIter++)
                {
                    qstrlTempGroups[iGroupIter] = qstrlTempGroups.at(iGroupIter).trimmed();//skip spaces from begining and end [gr, gr, gr]=>[gr,gr,gr]
                    if( qstrlTempGroups[iGroupIter].contains(" ") )// group with street - special case
                    {
                        qstrlLineWithoutSpaces = qstrlTempGroups[iGroupIter].split(" ");//if spaces inside [gr (street)]=>[gr,(street)]
                        qstrlTempGroups.clear();
                        int iAdInfoItter = 0;
                        if(qstrlLineWithoutSpaces[0].size() < 4)//if this is a group (assumption GROUP < 4 signs)
                        {
                            qstrlTempGroups.append(qstrlLineWithoutSpaces[0]);//take 1st element - group
                            iAdInfoItter = 1;//Skip 1st element in next for
                        }
                        for(; iAdInfoItter < qstrlLineWithoutSpaces.size(); iAdInfoItter++)//Fill aditional info field
                        {
                            sOneDayPlan.additionalInfo.append(qstrlLineWithoutSpaces[iAdInfoItter]);
                        }
                    }
                }
                QString tempToLower = qstrlTempGroups.join(",");
                tempToLower = tempToLower.toLower();
                sOneDayPlan.groups = tempToLower.split(",",QString::SkipEmptyParts);
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

bool dataStructure::setTableData(QTableWidget *TableWidgetToFill, int iDayIndex, QStringList qstrlValidateFilter)
{
    bool fRetVal = false;

    if( (0 != TableWidgetToFill) && (iDayIndex < WEEKDAY_LASTITEM) )
    {
        int iDayListSize = this->tab_listOfStructLineElements[iDayIndex].size();
        TableWidgetToFill->setRowCount(iDayListSize);//Set table size
        int iTablePosition = 0;

        for(int i=0; i < iDayListSize; i++)
        {
            if( shouldDisplaySubject(tab_listOfStructLineElements[iDayIndex].at(i).groups, qstrlValidateFilter) ) // filter groups
            {
                TableWidgetToFill->setItem(iTablePosition, START_TIME, new QTableWidgetItem(tab_listOfStructLineElements[iDayIndex].at(i).startTime));
                TableWidgetToFill->setItem(iTablePosition, FINISH_TIME, new QTableWidgetItem(tab_listOfStructLineElements[iDayIndex].at(i).finishTime));
                TableWidgetToFill->setItem(iTablePosition, SUBJECT_NAME, new QTableWidgetItem(tab_listOfStructLineElements[iDayIndex].at(i).subjectName));
                TableWidgetToFill->setItem(iTablePosition, SUBJECT_TYPE, new QTableWidgetItem(tab_listOfStructLineElements[iDayIndex].at(i).subjectType));

                QTableWidgetItem *qtwiGroup = new QTableWidgetItem(tab_listOfStructLineElements[iDayIndex].at(i).groups.join(","));
                if( (false == tab_listOfStructLineElements[iDayIndex].at(i).additionalInfo.isEmpty()) ||
                        ((true == tab_listOfStructLineElements[iDayIndex].at(i).groups.isEmpty()) && (false == tab_listOfStructLineElements[iDayIndex].at(i).additionalInfo.isEmpty()))
                  )
                {
                    qtwiGroup->setBackgroundColor(Qt::red);
                    qtwiGroup->setToolTip(tab_listOfStructLineElements[iDayIndex].at(i).additionalInfo);
                }
                TableWidgetToFill->setItem(iTablePosition, GROUPS, qtwiGroup);
                qtwiGroup = 0;
                iTablePosition++;
            }
        }
        TableWidgetToFill->setRowCount(iTablePosition);
    }
    else
    {
        qWarning() << "Null TableWidget in setTableData()";
    }

    return fRetVal;
}

bool dataStructure::findAvaliableGroups()
{
    bool fRetVal = false;
    QStringList qstrlTempGroups;
    QStringList qstrlLineWithoutSpaces;
    QString qstrIsItGroup;
    QRegularExpression regexp(".{1,4}");
    for(int iDay = MON; iDay < WEEKDAY_LASTITEM; iDay++)
    {
        for(int i=0; i < this->tab_listOfStructLineElements[iDay].size(); i++)
        {
            qstrlTempGroups = tab_listOfStructLineElements[iDay].at(i).groups;
            for(int iGroupIter = 0; iGroupIter < qstrlTempGroups.size(); iGroupIter++)
            {
                if( false == qstrl_avaliableGroups.contains(qstrlTempGroups.at(iGroupIter),Qt::CaseInsensitive) )//check if group is already in list
                {
                    qstrl_avaliableGroups.append(qstrlTempGroups.at(iGroupIter));
                }
            }
        }
    }
    qstrl_avaliableGroups.sort();

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
                     << "Sub: " << this->tab_listOfStructLineElements[iDay].at(i).subjectName << "Group: " << this->tab_listOfStructLineElements[iDay].at(i).groups \
                     << "Ad. info: " << this->tab_listOfStructLineElements[iDay].at(i).additionalInfo;
        }
    }
    qDebug() << "Avaliable groups: ";
    for(int iGroupIter = 0; iGroupIter < qstrl_avaliableGroups.size(); iGroupIter++)
    {
        qDebug() << qstrl_avaliableGroups.at(iGroupIter);
    }
}

QStringList dataStructure::getListWeekDays()
{
    QStringList qstrlWeekDays;
    for(int iDay = MON; iDay < WEEKDAY_LASTITEM; iDay++)
    {
        qstrlWeekDays << tab_strWeekDays[iDay];
    }

    return qstrlWeekDays;
}

void dataStructure::CleanPlanDataList()
{
    qstrl_avaliableGroups.clear();
    for(int iListIter = MON; iListIter < WEEKDAY_LASTITEM; iListIter++)
    {
        tab_listOfStructLineElements[iListIter].clear();
    }
}

int dataStructure::getGroupsSize()
{
    return qstrl_avaliableGroups.size();
}

QStringList dataStructure::getGroupsNamesList()
{
    return qstrl_avaliableGroups;
}

bool dataStructure::shouldDisplaySubject(QStringList pqstrlGroupList, QStringList pqstrlFilters)
{
    bool fRetVal = false;
    if( pqstrlGroupList.isEmpty() )
    {
        fRetVal = true;
    }

    for(int i=0; i < pqstrlGroupList.size(); i++)
    {
        if(pqstrlFilters.isEmpty())
        {
            fRetVal = true;
            break;
        }
        fRetVal = pqstrlFilters.contains(pqstrlGroupList[i]);
        if(fRetVal)
        {
            break;
        }
    }
    return fRetVal;
}
