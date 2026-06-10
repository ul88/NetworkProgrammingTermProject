#ifndef CSV_H
#define CSV_H

#include <QFile>
#include <QObject>
#include <QTextStream>
#include <QVector>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include "ErrorHandler.h"
#include "FileUtil.h"

namespace FileUtil{

class Csv{
public:
    explicit Csv()
    {
        QString dirPath = getDir() + "/log";
        QDir dir(dirPath);
        dir.mkpath(".");

        csvPath = dir.filePath("log1.csv");
    }

    explicit Csv(QString filename)
    {
        QString dirPath = getDir() + "/log";
        QDir dir(dirPath);
        dir.mkpath(".");

        filename.replace("\\", "/");
        while(filename.startsWith("/")){
            filename.remove(0, 1);
        }

        csvPath = dir.filePath(filename);
    }

    bool isFileEmpty() const{
        QFile file(csvPath);
        QFileInfo info(file.fileName());
        if(!info.exists()) return true;
        return info.size() == 0;
    }

    QVariantList read(){
        QFile file(csvPath);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            WARNING("파일 열기에 실패했습니다. 파일 존재 여부를 확인하세요.");
            return QVariantList();
        }

        QVariantList ret;
        QTextStream in(&file);

        QStringList colsName = in.readLine().split(",");

        while(!in.atEnd()){
            QString line = in.readLine();
            QVariantMap row;
            QStringList cols = line.split(",");
            for(int i=0;i<cols.size() && i<colsName.size();i++){
                row.insert(colsName[i], QVariant(cols[i]));
            }

            ret.append(row);
        }

        file.close();
        return ret;
    }

    QVariantList getHeaderRead(){
        QVariantList ret;

        if(!headerList.empty()){
            for(const auto& iter : headerList){
                ret.append(iter);
            }
            return ret;
        }

        QFile file(csvPath);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            WARNING("파일 열기에 실패했습니다. 파일 존재 여부를 확인하세요.");
            return ret;
        }

        QTextStream in(&file);
        if(in.atEnd()){
            WARNING("헤더가 비어있는 csv 파일입니다.");
            file.close();
            return ret;
        }

        QStringList colsName = in.readLine().split(",");
        file.close();

        for(const auto& iter : colsName){
            headerList.append(iter);
            ret.append(iter);
        }

        return ret;
    }

    void write(const QVector<QString>& v){
        QFile file(csvPath);
        if(!file.open(QIODevice::Append | QIODevice::Text)){
            WARNING("파일 열기에 실패했습니다. 파일 존재 여부를 확인하세요.");
            return;
        }

        if(isFileEmpty()){
            WARNING("csv 파일은 무조건 Header를 먼저 설정해야합니다. SetCsvHeader 함수를 먼저 실행하세요");
            file.close();
            return;
        }

        QTextStream out(&file);
        out << makeCsvLine(v) << "\n";
        out.flush();
        file.close();
    }

    void setCsvHeader(const QVector<QString>& v){
        QFile file(csvPath);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            WARNING("파일 열기에 실패했습니다. 파일 존재 여부를 확인하세요.");
            return;
        }
        QTextStream out(&file);
        out << makeCsvLine(v) << "\n";
        out.flush();
        file.close();
        headerList = v;
    }

private:
    QString makeCsvLine(const QVector<QString>& v){
        QString temp = "";

        for(int i=0;i<v.size();i++){
            temp += v[i];
            if(i == v.size() - 1) break;
            temp += ",";
        }

        return temp;
    }

    QString csvPath;

    QVector<QString> headerList;
};

} // namespace FileUtil
#endif // CSV_H
