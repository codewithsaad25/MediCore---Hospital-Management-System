#ifndef DOCTOR_H
#define DOCTOR_H

#include "Person.h"
#include <ostream>

class Doctor : public Person
{
private:
    char specialization[51];
    float fee;

public:
    Doctor();
    Doctor(int newId, const char newName[], const char newSpecialization[],
        const char newContact[], const char newPassword[], float newFee);

    const char* getSpecialization() const;
    float getFee() const;
    void setSpecialization(const char newSpecialization[]);
    void setFee(float newFee);

    bool operator==(const Doctor& other) const;

    const char* getRole() const;
    void toFileLine(char line[], int maxSize) const;
    friend std::ostream& operator<<(std::ostream& output, const Doctor& doctor);
};
#endif
