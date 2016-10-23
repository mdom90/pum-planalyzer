#include "fileoperatinos.h"
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>
#include <QCoreApplication>

#define MAX_LINE_IN_ANALYSIS 2
#define REGEXP_SPLIT_COLUMN "\\s{2,}" //match at least 2 white spaces

inline eAppendType operator|(eAppendType a, eAppendType b)
{return static_cast<eAppendType>(static_cast<int>(a) | static_cast<int>(b));}

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
           this->qstrCurrentLine = in.readLine();
           if(this->qstrCurrentLine.isEmpty())
               continue;

           foShouldAnalyseLine(&eShouldAnalyse);
           switch(eShouldAnalyse)
           {
               case DONT_ANALYSE:
               {
                  //qDebug() << "Case Dont Analyse!!";
                  break;
               }
               case ANALYSE:
               {
                   qDebug() << "Line: " << this->qstrCurrentLine;
                   fRetVal = foAnalyzeLine(&qstrMainBuffer);
                   break;
               }
               default:
               { qDebug() << "Warning case default!!";
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
    QString *qstrLineToAnalyse = &(this->qstrCurrentLine);
    quint8 *qu8StoreCounter = &(this->qu8StoreCounter);

    if( (0 != qstrMainBuffer) && (0 != qstrLineToAnalyse) && (0 != qu8StoreCounter))
    {
       QRegularExpression regex(REGEXP_SPLIT_COLUMN);//match at least 2 white spaces
       QStringList qstrlCurrentLine = qstrLineToAnalyse->split(regex, QString::SkipEmptyParts);//split QString to have line elements in list elements
       qDebug() << "Analyze line list: " << qstrlCurrentLine;
       if(qstrlCurrentLine.count() > u8MinimumNrOfColumns)
       {//Nomral line
            qDebug() << "Normal line";
            fRetVal = foAnalyzeNormalLine(qstrMainBuffer);
       }
       else
       {//Short line
            qDebug() << "Short line";
            fRetVal = foAnalyzeShortLine(qstrMainBuffer);
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
        this->qu8StoreCounter++;
        qDebug() << "CounterN: " << qu8StoreCounter;
        if( this->qu8StoreCounter >= MAX_LINE_IN_ANALYSIS )
        {
            this->qstrSaveBuffer = this->qstrLastLine + this->qstrCurrentLine;
            fRetVal = foStoreToMainBuffer(qstrMainBuffer);
            foCleanAnalyseData();
        }
        else
        {
            this->qstrLastLine = this->qstrCurrentLine;
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
        if(foIsWeekDay(this->qstrCurrentLine))
        {
            qDebug() << "Week day";
            this->qstrCurrentLine = "\r\n" + this->qstrCurrentLine + "\r\n";
            this->qu8StoreCounter++;
            if( this->qu8StoreCounter >= MAX_LINE_IN_ANALYSIS )
            {
                this->qstrSaveBuffer = this->qstrLastLine + this->qstrCurrentLine;
                fRetVal = foStoreToMainBuffer(qstrMainBuffer);
                foCleanAnalyseData();
            }
        }
        else if( this->qu8StoreCounter > 0 )
        {//Special line to analyse
            qDebug() << "Short line case";
            eAppendType appendType = LAST_IT;
            eAppendType appendTypeSub = LAST_IT;
            QString qstrGroup = "";
            QString qstrSubject = "";

            appendType = foFindGroup(&qstrGroup);
            appendTypeSub = foFindSubject(&qstrSubject);
            appendType = appendType|appendTypeSub;

            switch(appendType)
            {
                case GROUP:
                {
                    foAppendGroup(&qstrGroup);
                    foStoreToMainBuffer(qstrMainBuffer);
                    break;
                }
                case SUBJECT:
                {
                    foAppendSubject(&qstrSubject);
                    foStoreToMainBuffer(qstrMainBuffer);
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
            qDebug() << "Oops...";
            qDebug() << "CounterS: " << qu8StoreCounter;
        }
    }
    return fRetVal;
}

eAppendType fileOperatinos::foFindGroup(QString *qstrGroup)
{
    eAppendType eAppend= LAST_IT;
    if( 0 != qstrGroup )
    {
        int indexOfGroupElement = -1;
        QRegularExpression regex(REGEXP_SPLIT_COLUMN);//match at least 2 white spaces
        QStringList qstrLineAsList = this->qstrCurrentLine.split(regex, QString::SkipEmptyParts);

        QRegularExpression regexpFindGroup("([a-zA-Z]{1,2}[0-9]{1,2})", QRegularExpression::CaseInsensitiveOption); //".*\\d+."  //("(/w+/d)|(/d)|(/w{1})");
        indexOfGroupElement = qstrLineAsList.lastIndexOf(regexpFindGroup);

        if( indexOfGroupElement >= 0  )
        {
            if(indexOfGroupElement < qstrLineAsList.size())
            {
                *qstrGroup = qstrLineAsList.at(indexOfGroupElement);
                eAppend = GROUP;
                qDebug() << "List line to find group: " << qstrLineAsList;
                qDebug() << "Found group: "  <<  *qstrGroup;
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

eAppendType fileOperatinos::foFindSubject(QString *qstrSubject)
{
    eAppendType eAppend = LAST_IT;
    int indexOfSubjectElement = -1;
    if( 0 != qstrSubject )
    {
        QRegularExpression regexp_splitCol(REGEXP_SPLIT_COLUMN, QRegularExpression::CaseInsensitiveOption);
        QRegularExpression regexp_findSubject("([a-zA-Z]){3,}");
        QString *pqstrCurrentLine = &(this->qstrCurrentLine);
        QStringList qstrCurrentLineList = pqstrCurrentLine->split(regexp_splitCol);
        indexOfSubjectElement = qstrCurrentLineList.lastIndexOf(regexp_findSubject);
        if( (indexOfSubjectElement >= 0) && (indexOfSubjectElement < qstrCurrentLineList.size()) )
        {
            *qstrSubject = qstrCurrentLineList.at(indexOfSubjectElement);
            qDebug() << "Found subject: "  <<  *qstrSubject;
            eAppend = SUBJECT;
        }
        else
        {
            eAppend = NOTHING;
        }
    }
    return eAppend;
}

bool fileOperatinos::foAppendGroup(QString *pqstrGroup)
{
    bool fRetVal = false;
    if( 0 != pqstrGroup )
    {//Group is last element in string, so append to last element
        fRetVal = true;
        this->qstrLastLine.append(pqstrGroup);
        this->qstrSaveBuffer = this->qstrLastLine;
    }
    return fRetVal;
}

bool fileOperatinos::foAppendSubject(QString *pqstrSubject)
{
    bool fRetVal = false;
    QRegularExpression regexp_splitCol(REGEXP_SPLIT_COLUMN, QRegularExpression::CaseInsensitiveOption);
    QString *pqstrLastLine = &(this->qstrLastLine);

    if( 0 != pqstrSubject )
    {
        QStringList qstrlLastLine = pqstrLastLine->split(regexp_splitCol);
        qDebug() << "Subject add in here: " << qstrlLastLine.at(3);
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

bool fileOperatinos::foShouldAnalyseLine(eAnalyse *enumSaveData)//InterestingData?
{
    bool fRetVal = false;
    QString *qstrAnalyse = &(this->qstrCurrentLine);
    if( (0 != enumSaveData) && ( 0 != qstrAnalyse) )
    {
        static eAnalyse enumSaveDataLastState = DONT_ANALYSE;
        fRetVal = true;

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
        *qstrMainBuffer += (this->qstrSaveBuffer + "\r\n");
        fRetVal = true;
    }
    return fRetVal;
}

void fileOperatinos::foCleanAnalyseData()
{
    this->qu8StoreCounter = 0; // count from 0 to 2. 1 - line in lastLine, 2 - storeAction
    this->qstrCurrentLine = "";
    this->qstrLastLine = "";
    this->qstrSaveBuffer = "";
}
