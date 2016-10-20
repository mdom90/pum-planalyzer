#include "fileoperatinos.h"
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>
#include <QCoreApplication>

fileOperatinos::fileOperatinos()
{
    qstrBasePath = QCoreApplication::applicationDirPath();
    strTxtFilePath = qstrBasePath + "/pdfconvert/output.txt";
    strConvTxtFilePath = qstrBasePath + "/pdfconvert/orderFile.txt";
    strPdfConverterPath = qstrBasePath + "/pdfconvert/pdftotext.exe";
}

bool fileOperatinos::foPrepareFiles(QString strPdfFilePath)
{
    bool fRetVal = false;

    fRetVal = foConvertPdf(strPdfFilePath);
    if(fRetVal)
    {
        fRetVal = foConvertTextFile(this->strTxtFilePath);
    }

    return fRetVal;
}

bool fileOperatinos::foConvertPdf(QString strPdfFilePath)
{
    bool fRetVal = false;
    QObject *parent = 0;   
    qint16 q16maxProcTime = 3000;

    if( false == strPdfFilePath.isEmpty() )
    {
        fRetVal = true;
    }
    if( fRetVal )
    {
        QStringList arguments; // converter argument list
        arguments << "-table" << "-enc" << "UTF-8" << strPdfFilePath << this->strTxtFilePath;

        QProcess *myProcess = new QProcess(parent);
        myProcess->execute(this->strPdfConverterPath, arguments);
        myProcess->waitForFinished(q16maxProcTime);
        qDebug() << "end: " << myProcess->exitStatus();
        delete myProcess;
    }

    return fRetVal;
}

//TODO consider thread
bool fileOperatinos::foConvertTextFile(QString strTxtFilePath)
{
    bool fRetVal = true;
    QFile inputFile(strTxtFilePath);
    QFile outputFile(this->strConvTxtFilePath);
    eSaveData enumTempAnalyseData = LAST_ITEM;

    fRetVal = foPrepareWriteFile(&outputFile);
    if(fRetVal)
    {
        fRetVal = outputFile.open(QIODevice::WriteOnly|QIODevice::Append);
        fRetVal &= inputFile.open(QIODevice::ReadOnly);
    }
    //Both fiile sucessfuly opened
    if( fRetVal )
    {
       QTextStream in(&inputFile);
       QTextStream out(&outputFile);
       QString qstrMainBuffor;
       in.setCodec("UTF-8");
       qstrMainBuffor = in.readAll();
       foShouldAnalyseBuffer(&line, &enumTempAnalyseData);
       if( SAVE == enumTempAnalyseData )
       {
            fRetVal = foGeneralAnalyzeLine(&line, &qstrMainBuffor);
       }
       }

       outputFile.close();
       inputFile.close();
    }
    return fRetVal;
}

bool fileOperatinos::foPrepareWriteFile(QFile *qFileToCheck)
{
    bool fRetVal = false;
    qDebug() << "PrepareWriteFile: " << qFileToCheck->fileName();
    if( (fRetVal = qFileToCheck->exists()) )
    {
        if(!qFileToCheck->remove())
        {
            qDebug() << "Error file remove";
            fRetVal = false;
        }
        else
        {
            fRetVal = true;
            qDebug() << "File removed: " << qFileToCheck->fileName();
        }
    }
    else
    {
        fRetVal = true;
        qDebug() << "File does not exist: " << qFileToCheck->fileName();
    }
    return fRetVal;
}

bool fileOperatinos::foGeneralAnalyzeLine(QString *qstrLineToAnalyze, QString *qstrMainBuffor)
{
    bool fRetVal = false;
    static QStringList qstrlPreviousLine;
    static qint8 q8ExecuteCounter = 0;
    qint8 u8MinimumNrOfColumns = 4;

    if( (0 != qstrLineToAnalyze) && (0 != qstrMainBuffor) )
    {
       q8ExecuteCounter++;
       foRemoveUnecessarySigns(qstrLineToAnalyze);

       QRegularExpression regex("\\s{2,}");//match at least 2 white spaces
       QStringList lstCurrentRowData = qstrLineToAnalyze->split(regex, QString::SkipEmptyParts);//split QString to have line elements in list elements
       if( (false == foIsWeekDay(*qstrLineToAnalyze)) && (lstCurrentRowData.count() < u8MinimumNrOfColumns) )
       {//Append data to previous line
           foAppendLineToList(&qstrlPreviousLine, &lstCurrentRowData);
       }
       else
       {//Normal line
           qstrlPreviousLine = lstCurrentRowData;//Set previous line
       }



       if( q8ExecuteCounter >= 2 )
       {
           qDebug() << qstrlPreviousLine;
           q8ExecuteCounter = 0;
       }
       //qDebug() << *qstrLineToAnalyze;
    /*
       if( (true == fSaveData) && (lstRowData.count() < u8MinimumNrOfColumns) )
       {//Check if part of data is in new line, if yes, append to previous line
           foAppendLineToList(&qstrlMainList, &lstRowData);
           continue;
       }
       if( true == fSaveData )
       {//Normal line
           lstRowData.append("\r\n");
           qstrlMainList << lstRowData;
       }
         //qDebug() << "Num of col: " << lstRowData.count();

       qDebug() << qstrlMainList;
    */
    }

    return fRetVal;
}

bool fileOperatinos::foRemoveUnecessarySigns(QString *qstrlLineToAnalyze)
{
    bool fRetVal = false;

    if( (0 != qstrlLineToAnalyze) )
    {
        QRegularExpression regex("\\s{2,}");//match at least 2 white spaces
        QStringList tmpList = qstrlLineToAnalyze->split(regex, QString::SkipEmptyParts);
        tmpList.removeAll("-");
        *qstrlLineToAnalyze = tmpList.join("  ");
        fRetVal = true;
    }
    return fRetVal;
}

bool fileOperatinos::foAppendLineToList(QStringList *qstrlPreviousLineList, QStringList *qstrlListToAppend)
{
    bool fRetVal = true;
    int indexOfGroupElement = -1;
    int indexOfSubjectElement = -1;
    if( (0 != qstrlPreviousLineList) && (0 != qstrlListToAppend))
    {
        QStringList tmpData = *qstrlListToAppend;
        QRegularExpression regexpFindGroup(".*\\d+", QRegularExpression::CaseInsensitiveOption); //".*\\d+."  //("(/w+/d)|(/d)|(/w{1})");
        QRegularExpression regexpFindSubject("\\w+[^0-9]", QRegularExpression::CaseInsensitiveOption); // find words not containng numbers

        indexOfGroupElement = tmpData.lastIndexOf(regexpFindGroup);
        indexOfSubjectElement = tmpData.lastIndexOf(regexpFindSubject);
        //qDebug() << "String: " << *qstrlListToAppend;
        if( indexOfGroupElement > -1 )
        {
            //qDebug() << "Grupa: " << qstrlListToAppend->at(indexOfGroupElement);
            qstrlListToAppend->last() += " " + tmpData->at(indexOfGroupElement);
        }
       /* if( indexOfSubjectElement > -1 )
        {
            qint16 lastIndexOfHour = qstrlPreviousLineList->lastIndexOf(QRegularExpression("\\d{1,2}\.\\d{1,2}")); //match last hour
            lastIndexOfHour++;//switch to next column - which should be subject
            qstrlPreviousLineList->replace(lastIndexOfHour, (" " + qstrlListToAppend->at(indexOfSubjectElement)) );
            qDebug() << "Subject: " << qstrlListToAppend->at(lastIndexOfHour);
        }*/
    }
    return fRetVal;
}

 bool fileOperatinos::foShouldAnalyseBuffer(QString *qstrAnalyse, eSaveData *enumSaveData)
{
    bool fRetVal = false;
    if( (0 != qstrAnalyse) && (0 != enumSaveData))
    {
        static eSaveData enumSaveDataLastState = LAST_ITEM;
        fRetVal = true;

        if(qstrAnalyse->isEmpty())//Dont analyse if empty line
        {
            *enumSaveData = DONT_SAVE;
            return fRetVal;
        }
        if( foIsWeekDay(*qstrAnalyse) )//Analyse interesiting data
        {
            *enumSaveData = SAVE;
            enumSaveDataLastState = *enumSaveData;
            return fRetVal;
        }
        else if(qstrAnalyse->contains("UWAGI", Qt::CaseInsensitive))//Finish analysing interesting data
        {
            *enumSaveData = DONT_SAVE;
            enumSaveDataLastState = *enumSaveData;
            return fRetVal;
        }
        else//Continue last mode
        {
            *enumSaveData = enumSaveDataLastState;
            return fRetVal;
        }
    }
    return fRetVal;
}

 bool fileOperatinos::foIsWeekDay(QString qstrAnalyseLine)
 {
    bool fRetVal = false;
    QRegularExpression regexWeekDays("poniedziałek|wtorek|środa|czwartek|piątek|sobota|niedziela", QRegularExpression::CaseInsensitiveOption);
    if(qstrAnalyseLine.contains(regexWeekDays))
    {
        fRetVal = true;
    }
    return fRetVal;
 }
