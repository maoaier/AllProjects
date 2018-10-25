#ifndef CFLICKDATAVIEW_H
#define CFLICKDATAVIEW_H

#include "statviewbase.h"

class CFlickDataView : public CStatViewBase
{
public:
    CFlickDataView(QWidget *parent = 0);

protected:
    virtual bool _getStatDataFromFile(QString statFilePath, StatDataPoints *points);
};

#endif // CFLICKDATAVIEW_H
