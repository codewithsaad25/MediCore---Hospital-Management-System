#ifndef ADMIN_H
#define ADMIN_H

#include "Person.h"

class Admin : public Person
{
public:
    Admin();
    Admin(int newId, const char newName[], const char newPassword[]);
    const char* getRole() const;
    void toFileLine(char line[], int maxSize) const;
};

#endif