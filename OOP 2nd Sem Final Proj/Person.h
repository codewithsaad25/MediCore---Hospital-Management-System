#ifndef PERSON_H
#define PERSON_H

class Person {
protected:
    int id;
    char name[51];
    char contact[12];
    char password[51];

public:
    Person();
    Person(int newId, const char newName[], const char newContact[], const char newPassword[]);
    virtual ~Person();

    int getId() const;
    const char* getName() const;
    const char* getContact() const;
    const char* getPassword() const;
    bool checkPassword(const char enteredPassword[]) const;

    void setId(int newId);
    void setName(const char newName[]);
    void setContact(const char newContact[]);
    void setPassword(const char newPassword[]);

    virtual const char* getRole() const = 0;
    virtual void toFileLine(char line[], int maxSize) const = 0;
};

#endif
