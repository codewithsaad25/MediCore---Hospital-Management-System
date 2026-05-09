#include "Admin.h"
#include <cstdio>
using namespace std;

Admin::Admin() : Person()
{
}

Admin::Admin(int newId, const char newName[], const char newPassword[])
    : Person(newId, newName, "00000000000", newPassword) 
{
}

const char* Admin::getRole() const
{
    return "Admin";
}

void Admin::toFileLine(char line[], int maxSize) const
{
     snprintf(line, maxSize, "%d,%s,%s", id, name, password);
}   