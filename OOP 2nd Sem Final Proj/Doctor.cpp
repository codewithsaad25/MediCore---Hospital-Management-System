#include "Doctor.h"
#include "TextTools.h"
#include <cstdio>
#include<iostream>
using namespace std;

Doctor::Doctor() : Person() 
{
    TextTools::clear(specialization);
    fee = 0.0f;
}

Doctor::Doctor(int newId, const char newName[], const char newSpecialization[], const char newContact[], const char newPassword[], float newFee) : Person(newId, newName, newContact, newPassword)
{
    TextTools::copy(specialization, newSpecialization, 51);
    fee = newFee;
}

const char* Doctor::getSpecialization() const { return specialization; }
float Doctor::getFee() const { return fee; }
void Doctor::setSpecialization(const char newSpecialization[]) { TextTools::copy(specialization, newSpecialization, 51); }
void Doctor::setFee(float newFee) { fee = newFee; }

bool Doctor::operator==(const Doctor& other) const
{
    return id == other.id;
}

const char* Doctor::getRole() const
{
    return "Doctor";
}

void Doctor::toFileLine(char line[], int maxSize) const 
{
     snprintf(line, maxSize, "%d,%s,%s,%s,%s,%.2f", id, name, specialization, contact, password, fee);
}

 std::ostream& operator<<(std::ostream& output, const Doctor& doctor)
 {
    output << doctor.getId() << " | " << doctor.getName() << " | "
        << doctor.getSpecialization() << " | " << doctor.getContact()
        << " | " << doctor.getFee();
    return output;
}