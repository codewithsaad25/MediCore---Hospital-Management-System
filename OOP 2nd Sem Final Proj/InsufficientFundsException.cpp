#include "InsufficientFundsException.h"

InsufficientFundsException::InsufficientFundsException()
    : HospitalException("Insufficient balance for this operation.") {
}