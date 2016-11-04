#ifndef FILEOPERATINOS_H
#define FILEOPERATINOS_H
#include <QString>
#include <QStringList>
#include <QFile>
//exec pdf converter
#include <QProcess>
#include <QObject>
#include <QTextStream>
enum eAppendType { NOTHING = 0, GROUP = 1, SUBJECT = 2, BOTH = 3, LAST_IT = 3};

class fileOperatinos
{
public:
    fileOperatinos();
    //Singleton
    static fileOperatinos& foGetInstance()
    {
        static fileOperatinos singleton;
        return singleton;
    }
    QString strPdfFilePath;

    bool foPrepareFiles(QString strPdfFilePath);
    bool foLoadOrderDataFile(QString *strOrderData);

private:
    //Avoid copy of singleton
    fileOperatinos(fileOperatinos &);      // Don't Implement
    void operator=(fileOperatinos const&); // Don't implement

    //Necessary file paths//
    QString qstrBasePath; //main program path
    QString strTxtFilePath; // main + pdfconvert/output.txt
    QString strConvTxtFilePath; // main + pdfconvert/orderFile.txt
    QString strPdfConverterPath; // main + pdfconvert/pdftotext.exe

    //Data type
    enum eAnalyse {DONT_ANALYSE = 0, ANALYSE = 1, PREVIOUS_STATE = 2, STOP_ANALYSE = 3, LAST_ITEM = 4};

    //Variables for line analysis
    QString qstrCurrentLine;
    QString qstrSaveBuffer;

    //Convert pdf file from path to txt file
    bool foConvertPdf(QString strPdfFilePath);
    //Convert txt file to csv txt file
    bool foConvertTextFile(QString strTxtFilePath);
    bool foProcessFileAnalysis(QTextStream *qtsInput);

    //Analyze qStringList line, and modify it to give back
    //only interesting data
    bool foRemoveUnecessarySigns(QString *pqstrlAnalyzeLine);
    //Analyze text file, help to convert data to csv
    bool foAnalyzeLine();
    bool foAnalyzeShortLine();
    bool foAnalyzeNormalLine();
    void foCleanAnalyseData();
    eAppendType foFindGroup(QString *qstrGroup);
    eAppendType foFindSubject(QString *qstrSubject);
    bool foAppendGroup(QString *pqstrGroup);
    bool foAppendSubject(QString *pqstrSubject);
    //Check if line should be analysed
    bool foShouldAnalyseLine(eAnalyse *pShouldAnalyse);
    bool foStoreToMainBuffer(QString *qstrMainBuffer);
    bool foIsWeekDay(QString qstrAnalyseLine);
    //Prapare file for writing. Delete if file was created before
    // and create new empty one
    bool foPrepareWriteFile(QFile *qFileToCheck);

    //Save prepared data to file
    bool foSaveData();

};

#endif // FILEOPERATINOS_H
