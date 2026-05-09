#ifndef PATIENT_H
#define PATIENT_H

#include "Person.h"
#include <ostream>

class Patient : public Person {
private:
    int age;
    char gender;
    float balance;

public:
    Patient();
    Patient(int newId, const char newName[], int newAge, char newGender,
        const char newContact[], const char newPassword[], float newBalance);

    int getAge() const;
    char getGender() const;
    float getBalance() const;

    void setAge(int newAge);
    void setGender(char newGender);
    void setBalance(float newBalance);

    Patient& operator+=(float amount);
    Patient& operator-=(float amount);
    bool operator==(const Patient& other) const;

    const char* getRole() const;
    void toFileLine(char line[], int maxSize) const;

    friend std::ostream& operator<<(std::ostream& output, const Patient& patient);
};
#endif
