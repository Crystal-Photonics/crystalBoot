#ifndef NAMEDPLACEMARKER_H
#define NAMEDPLACEMARKER_H

#include <QMap>
#include <QString>

class NamedPlaceMarker
{
public:
    NamedPlaceMarker();
    void clear();
    QString replace(QString in);
    bool addVariable(QString variableName, QString variableValue);
private:
    QMap<QString,QString> variables;
};

#endif // NAMEDPLACEMARKER_H
