#include "namedplacemarker.h"


NamedPlaceMarker::NamedPlaceMarker()
{
}

void NamedPlaceMarker::clear()
{
    variables.clear();
}

bool NamedPlaceMarker::addVariable(QString variableName, QString variableValue)
{
    bool result = true;
    if (variableName == variableValue){
        result = false;
    }
    if(variables.contains(variableName)){
        result = false;
    }
    if(variables.contains(variableValue)){
        result = false;
    }
    if(result){
        variables.insert(variableName,variableValue);
    }
    return result;
}

QString NamedPlaceMarker::replace(QString in)
{
    QString result=in;

    QMap<QString,QString> ::iterator i;
    for (i = variables.begin(); i != variables.end(); ++i){
        result = result.replace(i.key(),i.value());
    }

    return result;
}
