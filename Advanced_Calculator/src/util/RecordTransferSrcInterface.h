#ifndef RECORDTRANSFERSRCINTERFACE_H
#define RECORDTRANSFERSRCINTERFACE_H

#include "CalqlatrRecord.h"

class RecordTransferSrcInterface {
public:
    virtual bool canTransfer() = 0;
    virtual const CalqlatrRecord &getRecordToTransfer() = 0;
};

#endif // RECORDTRANSFERSRCINTERFACE_H
