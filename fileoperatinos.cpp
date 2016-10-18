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
       in.setCodec("UTF-8");

       foAnalyzeFile(&in);

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

bool fileOperatinos::foAnalyzeFile(QTextStream *in)
{
    bool fRetVal = false;
    QStringList qstrlMainList;
    qint8 u8MinimumNrOfColumns = 4;

    if( 0 != in )
    {
        bool fSaveData = false;
        QString line;
        QStringList lstRowData;
        QRegularExpression regexp("\\s{2,}");//match at least 2 white spaces
        while (!in->atEnd())
        {
           line = in->readLine();
           if( line.isEmpty() )//skip empty string
           {
               continue;
           }
           lstRowData = line.split(regexp);//split QString to have line elements in list elements

           fRetVal = foDoesContainWeekDay(&lstRowData);
           //Append week day to main list buffor & start save data from monday
           if(fRetVal)
           {
                lstRowData.append("\r\n");
                qstrlMainList << lstRowData;
                fSaveData = true;
                continue;
           }
           //Remove "-" and finish save data at "Uwaga"
           foAnalyzeLine(&lstRowData, &fSaveData);
           if( (true == fSaveData) && (lstRowData.count() < u8MinimumNrOfColumns) )
           {
               foAppendLineToList(&qstrlMainList, &lstRowData);
               continue;
           }
           if( true == fSaveData )
           {
               qstrlMainList << lstRowData;
               //qDebug() << lstRowData;
           }
             //qDebug() << "Num of col: " << lstRowData.count();
        }
        qDebug() << qstrlMainList;
    }
    return fRetVal;
}

bool fileOperatinos::foAnalyzeLine(QStringList *qstrlLineToAnalyze, bool* fStartSaveData)
{
    bool fRetVal = false;

    if( (0 != qstrlLineToAnalyze) && (0 != fStartSaveData) )
    {
        qstrlLineToAnalyze->removeAll("-");
        //Save till Uwaga
        if( qstrlLineToAnalyze->contains("UWAGI", Qt::CaseInsensitive))
            *fStartSaveData = false;

        fRetVal = true;
    }

    return fRetVal;
}

bool fileOperatinos::foAppendLineToList(QStringList *qstrlMainList, QStringList *qstrlListToAppend)
{
    bool fRetVal = true;
    int indexOfGroupElement = -1;
    int indexOfSubjectElement = -1;
    if( (0 != qstrlMainList) && (qstrlListToAppend != 0))
    {
        QRegularExpression regexpFindGroup(".*\\d+.", QRegularExpression::CaseInsensitiveOption);  //("(/w+/d)|(/d)|(/w{1})");
        QRegularExpression regexpFindSubject("\\w+[^0-9]", QRegularExpression::CaseInsensitiveOption); // find words not containng numbers

        indexOfGroupElement = qstrlListToAppend->lastIndexOf(regexpFindGroup);
        indexOfSubjectElement = qstrlListToAppend->lastIndexOf(regexpFindSubject);
        //qDebug() << "String: " << *qstrlListToAppend;
        if( indexOfGroupElement > -1 )
        {
            //qDebug() << "Grupa: " << qstrlListToAppend->at(indexOfGroupElement);
            qstrlMainList->last() += " " + qstrlListToAppend->at(indexOfGroupElement) + "\r\n";
        }
        if( indexOfSubjectElement > -1 )
        {
            //qDebug() << "Subject: " << qstrlListToAppend->at(indexOfSubjectElement);
        }
    }
    return fRetVal;
}

 bool fileOperatinos::foDoesContainWeekDay(QStringList *qstrAnalyseList)
{
    bool fRetVal = false;
    if( 0 != qstrAnalyseList)
    {
        QRegularExpression regexWeekDays("poniedziałek|wtorek|środa|czwartek|piątek|sobota|niedziela", QRegularExpression::CaseInsensitiveOption);
        QString line = qstrAnalyseList->join(" ");
        if(line.contains(regexWeekDays))
        {
            fRetVal = true;
            qDebug() << "Week day" << line;
        }
    }
    return fRetVal;
}
