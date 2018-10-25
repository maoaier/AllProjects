#ifndef CBASEDATAVIEW_H
#define CBASEDATAVIEW_H

#include "statviewbase.h"

class CBaseDataView : public CStatViewBase
{    
public:
    explicit CBaseDataView(QWidget *parent = 0);
    virtual ~CBaseDataView();

protected:
    QVector <QString> m_vecBoxName;
    //virtual bool _getStatDataFromFile(QString statFilePath, StatDataPoints *points);
};

#endif // CBASEDATAVIEW_H
