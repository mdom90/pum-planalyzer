#include "fileoperatinos.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

fileOperatinos::fileOperatinos():strConvTxtFilePath("pdfconvert/orderFile.txt"),
                                 strTxtFilePath("pdfconvert/output.txt")
{

}

bool fileOperatinos::foConvertTextFile(QString strTxtFilePath)
{
    bool fRetVal = true;
    QFile inputFile(strTxtFilePath);
    QFile outputFile(this->strConvTxtFilePath);

    fRetVal = foPrepareWriteFile(&outputFile);

    //Both fiile sucessfuly opened
    if( (true == fRetVal) && (true == inputFile.open(QIODevice::ReadOnly)) )
    {
       QTextStream in(&inputFile);
       QTextStream out(&outputFile);
       QRegularExpression regexp("\\s{2,}");//match at least 2 white spaces
       while (!in.atEnd())
       {
          QString line = in.readLine();
          QStringList LstRowData = line.split(regexp);//spli QString to have line elements in list elements
          if( 0 != line.compare("") )//skip empty string
          {
            line = LstRowData.join(";");
            qDebug() << LstRowData;
            qDebug() << "Num of col: " << LstRowData.count();
          }
       }
       inputFile.close();
    }
}

bool fileOperatinos::foPrepareWriteFile(QFile *qFileToCheck)
{
    bool fRetVal = true;
    if( fRetVal = qFileToCheck->exists())
    {
        if(!qFileToCheck->remove())
        {
            qDebug << "Error file remove";
            fRetVal = false;
        }
    }
    else
    {
        fRetVal = outputFile.open(QIODevice::WriteOnly|QIODevice::Append);
        qDebug << "File removed: " << qFileToCheck->fileName();
    }
    return fRetVal;
}
