#include "SlotUnavailableException.h"

SlotUnavailableException::SlotUnavailableException()
    : HospitalException("Selected slot is already unavailable.") {
}