#include "fileoperatinos.h"
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>
#include <QCoreApplication>

#define MAX_LINE_IN_ANALYSIS 2

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
    eAnalyse eShouldAnalyse = LAST_ITEM;

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
       QString qstrMainBuffer;
       in.setCodec("UTF-8");
       while(!in.atEnd())
       {
           this->s_LineToAnalyse.qstrCurrentLine = in.readLine();
           foShouldAnalyseLine(&eShouldAnalyse);
           switch(eShouldAnalyse)
           {
               case DONT_ANALYSE:
               {
                  continue;
               }
               case ANALYSE:
               {
                   fRetVal = foAnalyzeLine(&qstrMainBuffer);
                   break;
               }
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

bool fileOperatinos::foAnalyzeLine(QString *qstrMainBuffer)
{
    bool fRetVal = false;
    const quint8 u8MinimumNrOfColumns = 4;

    if( (0 != qstrMainBuffor) )
    {
       QRegularExpression regex("\\s{2,}");//match at least 2 white spaces
       QStringList qstrlCurrentLine = this->s_LineToAnalyse.qstrCurrentLine.split(regex, QString::SkipEmptyParts);//split QString to have line elements in list elements
       if(qstrlCurrentLine.count() > u8MinimumNrOfColumns)
       {//Nomral line

       }
       else
       {//Short line
            if(foIsWeekDay(*qstrLineToAnalyze))
            {
                *qstrLineToAnalyze = ("\r\n" + (*qstrLineToAnalyze) + "\r\n");
                q8StoreCounter++;

            }
       }
    }
    return fRetVal;
}

bool fileOperatinos::foAnalyzeNormalLine(QString *qstrMainBuffer)
{
    bool fRetVal = false;

    if( 0 != qstrMainBuffer )
    {
        fRetVal = true;
        this->s_LineToAnalyse.qu8StoreCounter++;
        if( this->s_LineToAnalyse.qu8StoreCounter >= MAX_LINE_IN_ANALYSIS )
        {
            this->s_LineToAnalyse.qstrSaveBuffer = this->s_LineToAnalyse.qstrLastLine + this->s_LineToAnalyse.qstrCurrentLine;
            fRetVal = foStoreToMainBuffer(qstrMainBuffer);
            foCleanAnalyseData();
        }
        else
        {
            this->s_LineToAnalyse.qstrLastLine = this->s_LineToAnalyse.qstrCurrentLine;
        }
    }
    return fRetVal;
}


bool fileOperatinos::foAnalyzeShortLine(QString *qstrMainBuffer)
{
    bool fRetVal = false;
    if( 0 != qstrMainBuffer )
    {
        fRetVal = true;
        if(foIsWeekDay(this->s_LineToAnalyse.qstrCurrentLine))
        {
            this->s_LineToAnalyse.qstrCurrentLine = "\r\n" + this->s_LineToAnalyse.qstrCurrentLine + "\r\n";
            this->s_LineToAnalyse.qu8StoreCounter++;
            if( this->s_LineToAnalyse.qu8StoreCounter >= MAX_LINE_IN_ANALYSIS )
            {
                this->s_LineToAnalyse.qstrSaveBuffer = this->s_LineToAnalyse.qstrLastLine + this->s_LineToAnalyse.qstrCurrentLine;
                fRetVal = foStoreToMainBuffer(qstrMainBuffer);
                foCleanAnalyseData();
            }
        }
        else if( this->s_LineToAnalyse.qu8StoreCounter > 0 )
        {//Special line to analyse
            eAppendType appendType = LAST_ITEM;
            QString qstrGroup = "";
            QString qstrSubject = "";

            appendType = foFindGroup(&qstrGroup);
            appendType |= foFindGroup(&qstrSubject);

            switch(appendType)
            {
                case GROUP:
                {
                    break;
                }
                case SUBJECT:
                {
                    break;
                }
                case BOTH:
                {
                    break;
                }
                case  NOTHING:
                default:
                {//Some shit in line
                    foCleanAnalyseData();
                }
            }
        }
        else
        {//This should never happened. To do this scenario should be: shortLine_not_weekDay, normalLine
            //Nothing to do in here
        }
    }
    return fRetVal;
}

eAppendType fileOperatinos::foFindGroup(QString *qstrGroup)
{
    eAppendType eAppend= LAST_ITEM;
    if( 0 != qstrGroup )
    {
        int indexOfGroupElement = -1;
        QRegularExpression regex("\\s{2,}");//match at least 2 white spaces
        QStringList qstrLineAsList = this->s_LineToAnalyse.qstrCurrentLine.split(regex, QString::SkipEmptyParts);

        QRegularExpression regexpFindGroup("([a-zA-Z]{1,2}[0-9]{1,2})", QRegularExpression::CaseInsensitiveOption); //".*\\d+."  //("(/w+/d)|(/d)|(/w{1})");
        indexOfGroupElement = qstrLineAsList.lastIndexOf(regexpFindGroup);

        if( indexOfGroupElement >= 0  )
        {
            if(indexOfGroupElement < qstrLineAsList.size())
            {
                *qstrGroup = qstrLineAsList.at(indexOfGroupElement);
                eAppend = GROUP;
            }
            else
            {
                qDebug() << "foFindGroup ERROR out of scope";
                eAppend = NOTHING;
            }
        }
        eAppend = NOTHING;
    }
    return eAppend;
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

 bool fileOperatinos::foShouldAnalyseLine(QString *qstrAnalyse, eSaveData *enumSaveData)//InterestingData?
{
    bool fRetVal = false;
    if( (0 != qstrAnalyse) && (0 != enumSaveData))
    {
        static eSaveData enumSaveDataLastState = LAST_ITEM;
        fRetVal = true;

        if(qstrAnalyse->isEmpty())//Dont analyse if empty line
        {
            *enumSaveData = DONT_ANALYSE;
            return fRetVal;
        }
        if( foIsWeekDay(*qstrAnalyse) )//Analyse interesiting data
        {
            *enumSaveData = ANALYSE;
            enumSaveDataLastState = *enumSaveData;//set to continue in Continue last mode
            return fRetVal;
        }
        else if(qstrAnalyse->contains("UWAGI", Qt::CaseInsensitive))//Finish analysing interesting data
        {
            *enumSaveData = DONT_ANALYSE;
            enumSaveDataLastState = *enumSaveData; //set to continue in Continue last mode
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

bool fileOperatinos::foStoreToMainBuffer(QString *qstrMainBuffer)
{
    bool fRetVal = false;
    if( (0 != qstrMainBuffer) )
    {
        *qstrMainBuffer += (this->s_LineToAnalyse.qstrSaveBuffer + "\r\n");
        fRetVal = true;
    }
    return fRetVal;
}

void fileOperatinos::foCleanAnalyseData()
{
    memset(&this->s_LineToAnalyse, 0, sizeof(sLineAnalyse));
}
