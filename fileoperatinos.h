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

    //Data type
    enum eAnalyse {DONT_ANALYSE = 0, ANALYSE = 1, PREVIOUS_STATE = 2, LAST_ITEM = 3};
    enum eAppendType { NOTHING = 0, GROUP = 1, SUBJECT = 2, BOTH = 3, LAST_ITEM = 3};

    //Variables for line analysis
    struct sLineAnalyse
    {
        quint8 qu8StoreCounter; // count from 0 to 2. 1 - line in lastLine, 2 - storeAction
        QString qstrCurrentLine;
        QString qstrLastLine;
        QString qstrSaveBuffer;
    } s_LineToAnalyse;



    //Convert pdf file from path to txt file
    bool foConvertPdf(QString strPdfFilePath);
    //Convert txt file to programmer friendly txt file
    bool foConvertTextFile(QString strTxtFilePath);
    //Analyze qStringList line, and modify it to give back
    //only interesting data
    bool foRemoveUnecessarySigns(QString *pqstrlAnalyzeLine);
    //Analyze text file, help to convert data to csv
    bool foAnalyzeLine(QString *qstrMainBuffer);
    bool foAnalyzeShortLine(QString *qstrMainBuffer);
    bool foAnalyzeNomrlaLine(QString *qstrMainBuffer);
    void foCleanAnalyseData();
    eAppendType foFindGroup(QString *qstrGroup);
    eAppendType foFindSubject(QString *qstrSubject);
    //Append not full line to main line
    bool foAppendLineToList(QStringList *qstrlPreviousLineList, QStringList *qstrlListToAppend);
    //Analayze string, return if Save should start(true) or fiinish(false) base on string
    bool foShouldAnalyseLine(eAnalyse *pShouldAnalyse);
    bool foStoreToMainBuffer(QString *qstrMainBuffer);

    bool foIsWeekDay(QString qstrAnalyseLine);
    //Prapare orderFile.txt for writing. Delete if it was created before
    // and create new empty one
    bool foPrepareWriteFile(QFile *qFileToCheck);

    //Save prepared data to file
    bool foSaveData();

};

#endif // FILEOPERATINOS_H
