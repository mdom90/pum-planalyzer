#ifndef FILEOPERATINOS_H
#define FILEOPERATINOS_H
#include <QString>
#include <QStringList>
#include <QFile>
//exec pdf converter
#include <QProcess>
#include <QObject>
#include <QTextStream>


class fileOperatinos
{
public:
    fileOperatinos();
    static fileOperatinos& foGetInstance()
    {
        static fileOperatinos singleton;
        return singleton;
    }

    QString strPdfFilePath;

    bool foPrepareFiles(QString strPdfFilePath);


private:
    //Avoid copy of singleton
    fileOperatinos(fileOperatinos &);      // Don't Implement
    void operator=(fileOperatinos const&); // Don't implement

    //Necessary file paths//
    QString qstrBasePath; //main program path
    QString strTxtFilePath; // main + pdfconvert/output.txt
    QString strConvTxtFilePath; // main + pdfconvert/orderFile.txt
    QString strPdfConverterPath; // main + pdfconvert/pdftotext.exe

    //QStringList qslBaseFileInMemory;

    //Convert pdf file from path to txt file
    bool foConvertPdf(QString strPdfFilePath);
    //Convert txt file to programmer friendly txt file
    bool foConvertTextFile(QString strTxtFilePath);
    //Analyze qStringList line, and modify it to give back
    //only interesting data
    bool foAnalyzeLine(QStringList *pqstrlAnalyzeLine, bool* fStartSaveData, bool* fAppendLine);
    //Analyze text file, help to convert data to csv
    bool foAnalyzeFile(QTextStream* in);
    //Prapare orderFile.txt for writing. Delete if it was created before
    // and create new empty one
    bool foPrepareWriteFile(QFile *qFileToCheck);
    //Save prepared data to file
    bool foSaveData();

};

#endif // FILEOPERATINOS_H
