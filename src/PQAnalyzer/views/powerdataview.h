#ifndef CPOWERDATAVIEW_H
#define CPOWERDATAVIEW_H

#include "statviewbase.h"

class CPowerDataView : public CStatViewBase
{
public:
    explicit CPowerDataView(QWidget *parent = 0);
    ~CPowerDataView();

protected:
    //virtual bool _getStatDataFromFile(QString statFilePath, StatDataPoints *points);
};

#endif // CPOWERDATAVIEW_H
