#include "HospitalException.h"
#include "TextTools.h"

HospitalException::HospitalException() {
    TextTools::copy(message, "Hospital error.", 200);
}

HospitalException::HospitalException(const char text[]) {
    TextTools::copy(message, text, 200);
}

const char* HospitalException::what() const {
    return message;
}   