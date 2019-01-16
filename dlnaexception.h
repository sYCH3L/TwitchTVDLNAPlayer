#ifndef DLNAEXCEPTION_H
#define DLNAEXCEPTION_H

#include <exception>
#include <QString>

class DLNAException : public std::exception
{
public:
    DLNAException(QString msg);
    const char * what() {return m_msg.toStdString().c_str(); }
private:
    QString m_msg;
};


#endif // DLNAEXCEPTION_H
