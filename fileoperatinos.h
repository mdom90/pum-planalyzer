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

private:
    QString strPdfConverterRPath;
    QStringList slstConverterArguments;

    bool foConvertPdf();
    bool foLoadTextFile();
    bool foSaveData();

};

#endif // FILEOPERATINOS_H
