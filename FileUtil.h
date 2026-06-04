#ifndef FILEUTIL_H
#define FILEUTIL_H

#include<QObject>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QStringList>
#include "ErrorHandler.h"

namespace FileUtil{

static QString getDir() {return QCoreApplication::applicationDirPath();}

// json이나 기타 파일들 복사할 수 있도록 구성
static void copyFile(QString qrcPath, QString targetPath, QString type){
    QDir().mkpath(targetPath);

    QDir dir(qrcPath);
    QDir targetDir(targetPath);

    QStringList files = dir.entryList(
        QStringList() << "*."+type,
        QDir::Files
    );

    for(const QString& fileName : files){
        QString srcPath = dir.absoluteFilePath(fileName);
        QString dstPath = targetDir.absoluteFilePath(fileName);
        if(QFile::exists(dstPath)){
            continue;
        }

        if(!QFile::copy(srcPath, dstPath)){
            MyProgram::ErrorHandler::getInstance().send(fileName + " 파일 복사 실패", MyProgram::ErrorHandler::Warning);
        }else{
            MyProgram::ErrorHandler::getInstance().send("파일 복사 성공", MyProgram::ErrorHandler::Info);
        }
    }
}

} // namespace FileUtil
#endif // FILEUTIL_H
