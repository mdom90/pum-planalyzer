#include "datastructure.h"
#include <QDebug>

dataStructure::dataStructure()
{
    foInstance = &fileOperatinos::foGetInstance();
}

bool dataStructure::prepareTableData()
{
    bool fRetVal = false;
    if(0 != foInstance )
    {
        QString strOrderData;
        if( foInstance->foLoadOrderDataFile(&strOrderData) )
        {//File loaded in memory
            QStringList strListOrderData = strOrderData.split("\r\n", QString::SkipEmptyParts); //file listed by new line elements
            QStringList strListOneLineData;
            for(int OrderIter = 0; OrderIter < strListOrderData.size(); ++OrderIter)
            {
                strListOneLineData = strListOrderData[OrderIter].split(";");//line listed in ";" separated elements
                sLineElements ListElement;
                fillStruct(&ListElement, &strListOneLineData);
                list_sLineElements.append(ListElement);//data for table
            }
            fRetVal = true;
        }
        findAvaliableGroups();
    }

    return fRetVal;
}

bool dataStructure::fillStruct(sLineElements *structToFill, QStringList *dataIn)
{
    bool fRetVal = false;

    if( (0 != structToFill) && (0 != dataIn) )
    {
        fRetVal = true;
        int listSize = dataIn->size();
        if( (listSize < 2) && ( false == dataIn->isEmpty()) )//Week day
        {
            structToFill->weekDay = dataIn->first();
            //qDebug() << "Fill struct week: " << structToFill->weekDay;
        }
        else
        {
            for(int i = 0; i< dataIn->size(); ++i)
            {
                switch (i)
                {
                    case START_TIME:
                    {
                        structToFill->startTime = dataIn->at(i);
                        break;
                    }
                    case FINISH_TIME:
                    {
                        structToFill->finishTime = dataIn->at(i);
                        break;
                    }
                    case SUBJECT_NAME:
                    {
                        structToFill->subjectName = dataIn->at(i);
                        break;
                    }
                    case SUBJECT_TYPE:
                    {
                        structToFill->subjectType = dataIn->at(i);
                        break;
                    }
                    case GROUPS:
                    {
                        structToFill->groups = dataIn->at(i);
                        break;
                    }
                    default:
                    {
                        qDebug() << "Fill struct default!!";
                        break;
                    }
                }
            }
        }
    }

    return false;
}

void dataStructure::setTableData(QTableWidget *TableWidgetToFill)
{
    bool fRetVal = false;

    //if( 0 != TableWidgetToFill )
        //return fRetVal;
    TableWidgetToFill->setRowCount(this->list_sLineElements.size());
    for(int i=0; i < this->list_sLineElements.size(); i++)
    {
//        if(false == list_sLineElements[i].weekDay.isEmpty())
//        {
//            if( list_sLineElements[i].weekDay.contains("wtorek", Qt::CaseInsensitive) )
//            {//For test data only for Monday
//                break;
//            }
//        }
//        else
//        {
            TableWidgetToFill->setItem(i, START_TIME, new QTableWidgetItem(list_sLineElements[i].startTime));
            TableWidgetToFill->setItem(i, FINISH_TIME, new QTableWidgetItem(list_sLineElements[i].finishTime));
            TableWidgetToFill->setItem(i, SUBJECT_NAME, new QTableWidgetItem(list_sLineElements[i].subjectName));
            TableWidgetToFill->setItem(i, SUBJECT_TYPE, new QTableWidgetItem(list_sLineElements[i].subjectType));
            TableWidgetToFill->setItem(i, GROUPS, new QTableWidgetItem(list_sLineElements[i].groups));
//        }
    }

    //return fRetVal;
}

bool dataStructure::findAvaliableGroups()
{
    bool fRetVal = false;
//    for(int lineIter=0; lineIter < list_sLineElements; lineIter++)
//    {
//        if( !list_sLineElements[lineIter].groups.isEmpty() )
//        {
//            QStringList qstrlGroupList = list_sLineElements[lineIter].groups.split(",");
//            for(int i = 0; i< qstrlGroupList.size(); ++i)
//            {
//                if( false == qstrl_avaliableGroups.contains(qstrlGroupList[i]) )//check if group is already in avaliable list
//                {
//                    qstrl_avaliableGroups.append(qstrlGroupList[i]);
//                }
//            }
//        }
//    }
//    qDebug() << "Avaliable groups: " << qstrl_avaliableGroups;
    return fRetVal;
}

void dataStructure::testShowData()
{
    qDebug() << "Data for table: ";
    for(int i=0; i < this->list_sLineElements.size(); i++)
    {
        if(false == list_sLineElements[i].weekDay.isEmpty())
        {
            qDebug() << "Day: " << list_sLineElements[i].weekDay;
        }
        else
        {
            qDebug() << "Od: " << list_sLineElements[i].startTime << "Do: " << list_sLineElements[i].finishTime \
                   << "Sub: " << list_sLineElements[i].subjectName << "Group: " << list_sLineElements[i].groups;
        }

    }
}
