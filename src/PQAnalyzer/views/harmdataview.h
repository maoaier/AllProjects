#ifndef CHARMDATAVIEW_H
#define CHARMDATAVIEW_H

#include "statviewbase.h"

class CHarmDataView : public CStatViewBase
{
public:
    explicit CHarmDataView(QWidget *parent = 0);
    ~CHarmDataView();

protected:
//    virtual bool _getStatDataFromFile(QString statFilePath, StatDataPoints *points);
    QVector <QString> m_vecBoxName;
};

#endif // CHARMDATAVIEW_H
