#include "FileNotFoundException.h"

FileNotFoundException::FileNotFoundException(const char fileName[])
    : HospitalException(fileName)
{
}