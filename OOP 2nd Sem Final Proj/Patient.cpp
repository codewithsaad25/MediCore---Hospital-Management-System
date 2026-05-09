#include "Patient.h"
#include "TextTools.h"
#include <cstdio>
#include<iostream>
using  namespace std;

Patient::Patient() : Person() {
    age = 0;
    gender = 'M';
    balance = 0.0f;
}

Patient::Patient(int newId, const char newName[], int newAge, char newGender,
    const char newContact[], const char newPassword[], float newBalance)
    : Person(newId, newName, newContact, newPassword) {
    age = newAge;
    gender = newGender;
    balance = newBalance;
}

int Patient::getAge() const { return age; }
char Patient::getGender() const { return gender; }
float Patient::getBalance() const { return balance; }

void Patient::setAge(int newAge) { age = newAge; }
void Patient::setGender(char newGender) { gender = newGender; }
void Patient::setBalance(float newBalance) { balance = newBalance; }

Patient& Patient::operator+=(float amount) {
    balance = balance + amount;
    return *this;
}

Patient& Patient::operator-=(float amount) {
    balance = balance - amount;
    return *this;
}

bool Patient::operator==(const Patient& other) const {
    return id == other.id;
}

const char* Patient::getRole() const {
    return "Patient";
}

void Patient::toFileLine(char line[], int maxSize) const {
     snprintf(line, maxSize, "%d,%s,%d,%c,%s,%s,%.2f", id, name, age, gender, contact, password, balance);
}

ostream& operator<<(ostream& output, const Patient& patient) 
{
    output << patient.getId() << " | " << patient.getName() << " | "
        << patient.getAge() << " | " << patient.getGender() << " | "
        << patient.getContact() << " | " << patient.getBalance();
    return output;
}