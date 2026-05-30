#include "vendingmachine.h"

VendingMachine::VendingMachine(QObject *parent)
    : QObject{parent}
{
    openJson(":/json/beverage_data.json");
}

void VendingMachine::openJson(const QString& path){
    QFile file(path);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qWarning() << "파일 열기 실패" << path;
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parserError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parserError);

    if(parserError.error != QJsonParseError::NoError){
        qWarning() << "Json 파싱 실패:" << parserError.errorString();
        return;
    }

    if(!doc.isArray()){
        qWarning() << "JSON 최상위 구조가 배열 X";
        return;
    }

    const QJsonArray array = doc.array();
    for(const QJsonValue& value : array){
        if(!value.isObject()) continue;

        QJsonObject obj = value.toObject();

        int index = obj["index"].toInt();
        QString name = obj["name"].toString();
        int cost = obj["cost"].toInt();
        int count = obj["count"].toInt();
        QString imagePath = obj["imagePath"].toString();

        insertBeverage(QSharedPointer<Beverage>(
            new Beverage(index, name, cost, count, imagePath)));
    }
}
