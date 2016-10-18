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

    if( 0 != in )
    {
        bool fSaveData = false;
        bool fAppendLine = false;
        QString line;
        QStringList lstRowData;
        QStringList lstConcatExtraLine;
        QRegularExpression regexp("\\s{2,}");//match at least 2 white spaces
        while (!in->atEnd())
        {
           line = in->readLine();
           if( 0 == line.compare("") )//skip empty string
           {
               continue;
           }
           lstRowData = line.split(regexp);//split QString to have line elements in list elements
           foAnalyzeLine(&lstRowData, &fSaveData, &fAppendLine);
           if( (true = fSaveData) && (true == fAppendLine) )
           {

           }
           if( (true == fSaveData) && (false == fAppendLine) )
           {
               line = lstRowData.join(";");
               qDebug() << line;
           }
             //qDebug() << "Num of col: " << lstRowData.count();
        }
    }
    return fRetVal;
}

bool fileOperatinos::foAnalyzeLine(QStringList *qstrlLineToAnalyze, bool* fStartSaveData, bool* fAppendLine)
{
    bool fRetVal = false;

    if( (0 != qstrlLineToAnalyze) && (0 != fStartSaveData) && (0 != fAppendLine) )
    {
        qstrlLineToAnalyze->removeAll("-");
        //Start saving data to file from Monday
        if( qstrlLineToAnalyze->contains("poniedziałek", Qt::CaseInsensitive) )
            *fStartSaveData = true;
        //Save till Uwaga
        if( qstrlLineToAnalyze->contains("UWAGI", Qt::CaseInsensitive))
            *fStartSaveData = false;
        if( qstrlLineToAnalyze->count() < 5 )
        {
            *fAppendLine = true;
        }
        else
        {
            fAppendLine = false;
        }

        fRetVal = true;
    }

    return fRetVal;
}
