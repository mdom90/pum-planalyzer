#ifndef FILEOPERATINOS_H
#define FILEOPERATINOS_H
#include <QString>
#include <QStringList>


class fileOperatinos
{
public:
    fileOperatinos();
    QString strPdfFilePath;
    QString strTxtFilePath;
    const QString strConvTxtFilePath;

private:
    QString strPdfConverterRPath;
    QStringList slstConverterArguments;
    QStringList qslBaseFileInMemory;

    bool foConvertPdf(QString strPdfFilePath);
    bool foConvertTextFile(QString strTxtFilePath);
    bool foPrepareWriteFile(QFile *qFileToCheck);
    bool foSaveData();

};

#endif // FILEOPERATINOS_H
