#ifndef FILEUTIL_H
#define FILEUTIL_H

#include<QObject>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QCoreApplication>
#include <QStringList>
#include "ErrorHandler.h"
#include <QMap>

namespace FileUtil{

static QString getDir() {return QCoreApplication::applicationDirPath();}

// json이나 기타 파일들 복사할 수 있도록 구성
static QMap<QString, QString> copyQrcFile(QString type, QString qrcPath, QString folderName){
    QMap<QString, QString> ret;

    QString targetPath = QDir(getDir()).filePath(folderName);

    if(!QDir().mkpath(targetPath)){
        WARNING("기본 폴더 생성 실패 " + targetPath);
        return ret;
    }

    QDir dir(qrcPath);
    QDir targetDir(targetPath);

    QStringList files = dir.entryList(
        QStringList() << "*."+type,
        QDir::Files
    );

    for(const QString& fileName : files){
        QString srcPath = dir.absoluteFilePath(fileName);
        QString dstPath = targetDir.absoluteFilePath(fileName);
        DEBUG(srcPath);
        DEBUG(dstPath);
        if(QFile::exists(dstPath)){
            ret.insert(fileName,dstPath);
            continue;
        }

        if(!QFile::copy(srcPath, dstPath)){
            WARNING(fileName + " 파일 복사 실패");
        }else{
            QFile dstFile(dstPath);
            dstFile.setPermissions(dstFile.permissions() |
                                QFileDevice::WriteOwner |
                                QFileDevice::WriteUser |
                                QFileDevice::WriteGroup |
                                QFileDevice::WriteOther);
            INFO("파일 복사 성공");
            ret.insert(fileName, dstPath);
        }
    }
    return ret;
}

static QMap<QString, QString> listFile(QString type, QString path){
    QDir dir(path);
    QStringList files = dir.entryList(
        QStringList() << "*."+type,
        QDir::Files
    );

    QMap<QString, QString> ret;

    for(const auto& fileName : files){
        ret.insert(fileName,dir.absoluteFilePath(fileName));
    }

    return ret;
}

static QJsonDocument openJson(const QString& path){
    QFile file(path);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        WARNING("파일 열기 실패 %1", path);
        return QJsonDocument();
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parserError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parserError);

    if(parserError.error != QJsonParseError::NoError){
        WARNING("Json 파싱 실패:" + parserError.errorString());
        return QJsonDocument();;
    }

    return doc;
}

static bool saveJson(const QString& path, const QJsonDocument& doc){
    QFile file(path);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        WARNING("파일 열기 실패 %1 | error=%2", path, file.errorString());
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    return true;
}

} // namespace FileUtil
#endif // FILEUTIL_H
