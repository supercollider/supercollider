#ifndef SCIDE_SETTINGS_INTERFACE_HPP_INCLUDED
#define SCIDE_SETTINGS_INTERFACE_HPP_INCLUDED

#include <QVariant>

namespace ScIDE {

class SettingsInterface
{
public:
    virtual void beginGroup ( const QString & prefix ) = 0;
    virtual void endGroup () = 0;

    virtual QVariant value ( const QString & key ) const = 0;
    virtual void setValue ( const QString & key, const QVariant & value ) = 0;

    QString getConfigFile()
    {
        return value("IDE/interpreter/configFile").toString();
    }

    void setConfigFile(QString path)
    {
        setValue("IDE/interpreter/configFile", path);
    }
};

}
#endif // SCIDE_SETTINGS_INTERFACE_HPP_INCLUDED
