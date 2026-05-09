#include "Validator.h"
#include "TextTools.h"
#include <ctime>
#include <cstdio>
#include<iostream>
using namespace std;

bool Validator::isValidId(const char text[]) {
    return TextTools::containsOnlyDigits(text) && TextTools::toInt(text) > 0;
}

bool Validator::isValidDate(const char text[]) {
    if (TextTools::length(text) != 10) return false;
    if (text[2] != '-' || text[5] != '-') return false;
    for (int i = 0; i < 10; i++) {
        if (i != 2 && i != 5 && (text[i] < '0' || text[i] > '9')) return false;
    }
    int day = (text[0] - '0') * 10 + (text[1] - '0');
    int month = (text[3] - '0') * 10 + (text[4] - '0');
    int year = (text[6] - '0') * 1000 + (text[7] - '0') * 100 + (text[8] - '0') * 10 + (text[9] - '0');

    time_t now =  time(0);
     tm localTime;
    localtime_s(&localTime, &now);
    int currentYear = localTime.tm_year + 1900;

    return day >= 1 && day <= 31 && month >= 1 && month <= 12 && year >= currentYear;
}

bool Validator::isValidTimeSlot(const char text[]) {
    const char slots[8][6] = { "09:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00" };
    for (int i = 0; i < 8; i++) {
        if (TextTools::equals(text, slots[i])) {
            return true;
        }
    }
    return false;
}

bool Validator::isValidContact(const char text[]) {
    return TextTools::length(text) == 11 && TextTools::containsOnlyDigits(text);
}

bool Validator::isValidPassword(const char text[]) {
    return TextTools::length(text) >= 6;
}

bool Validator::isPositiveFloat(const char text[]) {
    if (TextTools::isEmpty(text)) return false;
    int dots = 0;
    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] == '.') dots++;
        else if (text[i] < '0' || text[i] > '9') return false;
    }
    return dots <= 1 && TextTools::toFloat(text) > 0.0f;
}

bool Validator::isValidMenuChoice(const char text[], int minimum, int maximum) {
    if (!TextTools::containsOnlyDigits(text)) return false;
    int value = TextTools::toInt(text);
    return value >= minimum && value <= maximum;
}

bool Validator::isValidAge(const char text[]) {
    if (!TextTools::containsOnlyDigits(text)) return false;
    int age = TextTools::toInt(text);
    return age > 0 && age <= 120;
}

bool Validator::isValidGender(const char text[]) {
    return TextTools::length(text) == 1 && (text[0] == 'M' || text[0] == 'F' || text[0] == 'm' || text[0] == 'f');
}

int Validator::compareDates(const char first[], const char second[]) {
    int d1 = (first[0] - '0') * 10 + (first[1] - '0');
    int m1 = (first[3] - '0') * 10 + (first[4] - '0');
    int y1 = (first[6] - '0') * 1000 + (first[7] - '0') * 100 + (first[8] - '0') * 10 + (first[9] - '0');
    int d2 = (second[0] - '0') * 10 + (second[1] - '0');
    int m2 = (second[3] - '0') * 10 + (second[4] - '0');
    int y2 = (second[6] - '0') * 1000 + (second[7] - '0') * 100 + (second[8] - '0') * 10 + (second[9] - '0');
    if (y1 != y2) return y1 - y2;
    if (m1 != m2) return m1 - m2;
    return d1 - d2;
}

int Validator::compareTimes(const char first[], const char second[]) {
    int h1 = (first[0] - '0') * 10 + (first[1] - '0');
    int h2 = (second[0] - '0') * 10 + (second[1] - '0');
    return h1 - h2;
}

void Validator::today(char output[]) {
     time_t now =  time(0);
     tm localTime;
    localtime_s(&localTime, &now);
     strftime(output, 11, "%d-%m-%Y", &localTime);
}

bool Validator::isOlderThanSevenDays(const char date[]) {
     tm billTime = {};
    billTime.tm_mday = (date[0] - '0') * 10 + (date[1] - '0');
    billTime.tm_mon = ((date[3] - '0') * 10 + (date[4] - '0')) - 1;
    billTime.tm_year = ((date[6] - '0') * 1000 + (date[7] - '0') * 100 + (date[8] - '0') * 10 + (date[9] - '0')) - 1900;

     time_t bill =  mktime(&billTime);
     time_t now =  time(0);
    double seconds =  difftime(now, bill);
    return seconds > 7.0 * 24.0 * 60.0 * 60.0;
}