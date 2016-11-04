#include "fileoperatinos.h"
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>
#include <QCoreApplication>

#define NEW_LINE_OFFSET 2
#define REGEXP_SPLIT_COLUMN " {2,}" //match at least 2 white spaces

inline eAppendType operator|(eAppendType a, eAppendType b)
{
    return static_cast<eAppendType>(static_cast<int>(a) | static_cast<int>(b));
}

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

bool fileOperatinos::foLoadOrderDataFile(QString *orderOneLine)
{
    bool fRetVal = false;
    QFile inputFile(this->strConvTxtFilePath);
    fRetVal = inputFile.open(QIODevice::ReadOnly);

    if( (true == fRetVal) && (0 != orderOneLine) )
    {
        QTextStream in(&inputFile);
        in.setCodec("UTF-8");
        *orderOneLine = in.readAll();
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
        QStringList arguments; // converter argument list
        arguments << "-table" << "-enc" << "UTF-8" << strPdfFilePath << this->strTxtFilePath;
        QProcess *myProcess = new (std::nothrow) QProcess(parent);

        if( 0 != myProcess )
        {
            qDebug() << "foConvertPdf() start pdftotex:" << strPdfConverterPath << arguments;
            myProcess->execute(this->strPdfConverterPath, arguments);
            myProcess->waitForFinished(q16maxProcTime);
            qDebug() << "end: " << myProcess->exitStatus();
            delete myProcess;
            fRetVal = true;
        }
    }
    else
    {
        qWarning() << "foConvertPdf() PdfFilePath is empty";
    }

    return fRetVal;
}

//TODO consider thread
bool fileOperatinos::foConvertTextFile(QString strTxtFilePath)
{
    bool fRetVal = false;
    QFile inputFile(strTxtFilePath);
    QFile outputFile(this->strConvTxtFilePath);

    fRetVal = foPrepareWriteFile(&outputFile);
    if( true == fRetVal)
    {
        fRetVal = outputFile.open(QIODevice::WriteOnly|QIODevice::Append);
        fRetVal &= inputFile.open(QIODevice::ReadOnly);
    }
    //Both fiile sucessfuly opened
    if( fRetVal )
    {
        QTextStream in(&inputFile);
        QTextStream out(&outputFile);
        QString qstrMainBuffer;//Buffer that will ba saved to output fill
        in.setCodec("UTF-8");

        foProcessFileAnalysis(&in);
        foStoreToMainBuffer(&qstrMainBuffer);
        foCleanAnalyseData();

        foRemoveUnecessarySigns(&qstrMainBuffer);

        QRegularExpression regex(" {2,}");//match at least 2 white spaces
        QStringList MainBufferList = qstrMainBuffer.split(regex, QString::SkipEmptyParts);
        out.setCodec("UTF-8");
        out << MainBufferList.join(";"); //Insert converted data in to file
        outputFile.close();
        inputFile.close();
    }
    return fRetVal;
}

bool fileOperatinos::foProcessFileAnalysis(QTextStream *in)
{
    bool fRetVal = false;
    if( (0 != in) )
    {
        qDebug() << "Enter proccesFileAnalyssis()";
        eAnalyse eShouldAnalyse = LAST_ITEM;

        while(!in->atEnd())//Till EOF
        {
            this->qstrCurrentLine = in->readLine();
            if(this->qstrCurrentLine.isEmpty())
            {
                //Skip empty lines
                continue;
            }
            foShouldAnalyseLine(&eShouldAnalyse);
            switch(eShouldAnalyse)
            {
            case DONT_ANALYSE:
            {
                break;
            }
            case ANALYSE:
            {
                fRetVal = foAnalyzeLine();
                break;
            }
            case STOP_ANALYSE:
            {
                in->readAll();
                in->flush();
                break;
            }
            default:
            {
                qWarning() << "Warning case default!!";
            }
            }
        }
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

bool fileOperatinos::foAnalyzeLine()
{
    bool fRetVal = false;
    const quint8 u8MinimumNrOfColumns = 4;
    QString *qstrLineToAnalyse = &(this->qstrCurrentLine);

    if( (0 != qstrLineToAnalyse) )
    {
        QRegularExpression regex(REGEXP_SPLIT_COLUMN);//match at least 2 white spaces
        QStringList qstrlCurrentLine = qstrLineToAnalyse->split(regex, QString::SkipEmptyParts);//split QString to have line elements in list elements
        //qDebug() << "Analyze line list: " << qstrlCurrentLine;
        if(qstrlCurrentLine.count() > u8MinimumNrOfColumns)
        {
            fRetVal = foAnalyzeNormalLine();
        }
        else
        {
            fRetVal = foAnalyzeShortLine();
        }
    }
    return fRetVal;
}

bool fileOperatinos::foAnalyzeNormalLine()
{
    this->qstrSaveBuffer += this->qstrCurrentLine + "\r\n";

    return true;
}

bool fileOperatinos::foAnalyzeShortLine()
{
    if(foIsWeekDay(this->qstrCurrentLine))
    {
        //Special case week day
        this->qstrCurrentLine = this->qstrCurrentLine + "\r\n";
        this->qstrSaveBuffer += this->qstrCurrentLine;
    }
    else
    {
        //Special case - add group to last line
        eAppendType appendType = LAST_IT;
        QString qstrGroup = "";
        appendType = foFindGroup(&qstrGroup);
        if( GROUP == appendType)
        {
            foAppendGroup(&qstrGroup);
        }
    }
    return true;
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
        indexOfGroupElement = qstrLineAsList.lastIndexOf(regexpFindGroup); // find last place in list where are groups

        if( (indexOfGroupElement >= 0) && (indexOfGroupElement < qstrLineAsList.size()) )
        {
            *qstrGroup = " "+qstrLineAsList.at(indexOfGroupElement);
            eAppend = GROUP;
            //qDebug() << "Found group: "  <<  *qstrGroup;
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
    {
        //Group is last element in string, so append to last element - \r\n signs
        fRetVal = true;
        int uStrSize = this->qstrSaveBuffer.size();
        this->qstrSaveBuffer.insert(uStrSize - NEW_LINE_OFFSET, *pqstrGroup);//insert before \r\n
    }
    return fRetVal;
}


bool fileOperatinos::foRemoveUnecessarySigns(QString *qstrlLineToCleanUp)
{
    bool fRetVal = false;

    if( (0 != qstrlLineToCleanUp) )
    {
        QRegularExpression regexp_unnecesarysigns("\f");
        qstrlLineToCleanUp->replace(regexp_unnecesarysigns,"");
        fRetVal = true;
    }
    return fRetVal;
}

bool fileOperatinos::foShouldAnalyseLine(eAnalyse *enumSaveData)
{
    bool fRetVal = false;
    QString *qstrAnalyse = &(this->qstrCurrentLine);
    if( (0 != enumSaveData) && ( 0 != qstrAnalyse) )
    {
        static eAnalyse enumSaveDataLastState = DONT_ANALYSE;//keep previous state

        if( foIsWeekDay(*qstrAnalyse) )//Analyse interesiting data
        {
            *enumSaveData = ANALYSE;
            enumSaveDataLastState = *enumSaveData;//set to continue in Continue last mode
            fRetVal = true;
        }
        if( qstrAnalyse->contains("UWAGI", Qt::CaseInsensitive) )//Finish analysing interesting data
        {
            *enumSaveData = STOP_ANALYSE;
            enumSaveDataLastState = *enumSaveData; //set to continue in Continue last mode
            fRetVal = true;
        }
        if( false == fRetVal ) //Continue last mode
        {
            *enumSaveData = enumSaveDataLastState;
            fRetVal = true;
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
        *qstrMainBuffer += (this->qstrSaveBuffer);
        fRetVal = true;
    }
    return fRetVal;
}

void fileOperatinos::foCleanAnalyseData()
{
    this->qstrCurrentLine = "";
    this->qstrSaveBuffer = "";
}

/*----------------------------------------NOT IMPLEMENTED---------------------------------------*/

//eAppendType fileOperatinos::foFindSubject(QString *qstrSubject)
//{
//    eAppendType eAppend = LAST_IT;
//    int indexOfSubjectElement = -1;
//    if( 0 != qstrSubject )
//    {
//        QRegularExpression regexp_splitCol(REGEXP_SPLIT_COLUMN, QRegularExpression::CaseInsensitiveOption);
//        QRegularExpression regexp_findSubject("([a-zA-Z]){3,}");
//        QString *pqstrCurrentLine = &(this->qstrCurrentLine);
//        QStringList qstrCurrentLineList = pqstrCurrentLine->split(regexp_splitCol);
//        indexOfSubjectElement = qstrCurrentLineList.lastIndexOf(regexp_findSubject);
//        if( (indexOfSubjectElement >= 0) && (indexOfSubjectElement < qstrCurrentLineList.size()) )
//        {
//            *qstrSubject = qstrCurrentLineList.at(indexOfSubjectElement);
//            //qDebug() << "List line to find subject: " << qstrCurrentLineList;
//            //qDebug() << "Found subject: "  <<  *qstrSubject;
//            eAppend = SUBJECT;
//        }
//        else
//        {
//            eAppend = NOTHING;
//        }
//    }
//    return eAppend;
//}

//bool fileOperatinos::foAppendSubject(QString *pqstrSubject)
//{
//    bool fRetVal = false;
//        QRegularExpression regexp_splitCol(REGEXP_SPLIT_COLUMN, QRegularExpression::CaseInsensitiveOption);
//        QString *pqstrLastLine = &(this->qstrLastLine);

//        if( 0 != pqstrSubject )
//        {
//            QStringList qstrlLastLine = pqstrLastLine->split(regexp_splitCol);
//            qDebug() << "Subject add in here: " << qstrlLastLine.at(3);
//            QString qstrSubject = qstrlLastLine.at(3);
//            qstrSubject = qstrSubject + *pqstrSubject; // marged subject
//            qstrlLastLine.replace(3, qstrSubject);
//            *pqstrLastLine =
//            this->qstrSaveBuffer = qstrLastLine;
//        }


//    return fRetVal;
//}
