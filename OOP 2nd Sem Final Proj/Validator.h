#ifndef VALIDATOR_H
#define VALIDATOR_H

class Validator {
public:
    static bool isValidId(const char text[]);
    static bool isValidDate(const char text[]);
    static bool isValidTimeSlot(const char text[]);
    static bool isValidContact(const char text[]);
    static bool isValidPassword(const char text[]);
    static bool isPositiveFloat(const char text[]);
    static bool isValidMenuChoice(const char text[], int minimum, int maximum);
    static bool isValidAge(const char text[]);
    static bool isValidGender(const char text[]);
    static int compareDates(const char first[], const char second[]);
    static int compareTimes(const char first[], const char second[]);
    static void today(char output[]);
    static bool isOlderThanSevenDays(const char date[]);
};

#endif
