#include "Person.h"
#include "TextTools.h"
#include<iostream>
using namespace std;

Person::Person() {
    id = 0;
    TextTools::clear(name);
    TextTools::clear(contact);
    TextTools::clear(password);
}

Person::Person(int newId, const char newName[], const char newContact[], const char newPassword[]) {
    id = newId;
    TextTools::copy(name, newName, 51);
    TextTools::copy(contact, newContact, 12);
    TextTools::copy(password, newPassword, 51);
}

Person::~Person() {
}

int Person::getId() const {
    return id;
}

const char* Person::getName() const {
    return name;
}

const char* Person::getContact() const {
    return contact;
}

const char* Person::getPassword() const {
    return password;
}

bool Person::checkPassword(const char enteredPassword[]) const {
    return TextTools::equals(password, enteredPassword);
}

void Person::setId(int newId) {
    id = newId;
}

void Person::setName(const char newName[]) {
    TextTools::copy(name, newName, 51);
}

void Person::setContact(const char newContact[]) {
    TextTools::copy(contact, newContact, 12);
}

void Person::setPassword(const char newPassword[]) {
    TextTools::copy(password, newPassword, 51);
}