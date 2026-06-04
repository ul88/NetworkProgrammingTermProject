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
        QDir().mkpath(dirPath);

        m_file.setFileName(dirPath + "/log1.csv");
    }

    explicit Csv(QString filename)
    {
        QString dirPath = QCoreApplication::applicationDirPath() + "/log";
        QDir().mkpath(dirPath);

        m_file.setFileName(dirPath + filename);
    }

    bool isFileEmpty() const{
        QFileInfo info(m_file.fileName());
        if(!info.exists()) return true;
        return info.size() == 0;
    }

    void write(const QVector<QString>& v){
        if(!m_file.open(QIODevice::Append | QIODevice::Text)){
            MyProgram::ErrorHandler::getInstance()
                .send("파일 열기에 실패했습니다. 파일 존재 여부를 확인하세요.");
            return;
        }

        if(isFileEmpty()){
            MyProgram::ErrorHandler::getInstance()
            .send("csv 파일은 무조건 Header를 먼저 설정해야합니다. SetCsvHeader 함수를 먼저 실행하세요");
            m_file.close();
            return;
        }

        QTextStream out(&m_file);
        out << makeCsvLine(v) << "\n";
        out.flush();
        m_file.flush();
    }

    void setCsvHeader(const QVector<QString>& v){
        if(!m_file.open(QIODevice::WriteOnly | QIODevice::Text)){
            MyProgram::ErrorHandler::getInstance()
                .send("파일 열기에 실패했습니다. 파일 존재 여부를 확인하세요.");
            return;
        }
        QTextStream out(&m_file);
        out << makeCsvLine(v) << "\n";
        out.flush();
        m_file.close();
    }

    ~Csv(){
        if(m_file.isOpen()) m_file.close();
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
    QFile m_file;
};

} // namespace FileUtil
#endif // CSV_H
