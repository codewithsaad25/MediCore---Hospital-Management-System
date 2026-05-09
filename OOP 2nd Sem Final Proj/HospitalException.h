#ifndef HOSPITALEXCEPTION_H
#define HOSPITALEXCEPTION_H

class HospitalException {
protected:
    char message[200];

public:
    HospitalException();
    HospitalException(const char text[]);
    virtual const char* what() const;
};

#endif
