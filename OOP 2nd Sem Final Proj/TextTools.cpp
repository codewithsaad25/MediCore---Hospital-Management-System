#include "TextTools.h"
#include <cstdio>
#include <cctype>
#include<iostream>
using namespace std;

int TextTools::length(const char text[]) {
    int count = 0;
    while (text[count] != '\0') {
        count++;
    }
    return count;
}

void TextTools::copy(char destination[], const char source[], int maxSize) {
    int i = 0;
    while (i < maxSize - 1 && source[i] != '\0') {
        destination[i] = source[i];
        i++;
    }
    destination[i] = '\0';
}

void TextTools::append(char destination[], const char source[], int maxSize) {
    int start = length(destination);
    int i = 0;
    while (start + i < maxSize - 1 && source[i] != '\0') {
        destination[start + i] = source[i];
        i++;
    }
    destination[start + i] = '\0';
}

bool TextTools::equals(const char first[], const char second[]) {
    int i = 0;
    while (first[i] != '\0' && second[i] != '\0') {
        if (first[i] != second[i]) {
            return false;
        }
        i++;
    }
    return first[i] == '\0' && second[i] == '\0';
}

bool TextTools::equalsIgnoreCase(const char first[], const char second[]) {
    int i = 0;
    while (first[i] != '\0' && second[i] != '\0') {
        char a = (char) tolower((unsigned char)first[i]);
        char b = (char) tolower((unsigned char)second[i]);
        if (a != b) {
            return false;
        }
        i++;
    }
    return first[i] == '\0' && second[i] == '\0';
}

bool TextTools::isEmpty(const char text[]) {
    return text[0] == '\0';
}

int TextTools::toInt(const char text[]) {
    int value = 0;
    int i = 0;
    while (text[i] != '\0') {
        if (text[i] >= '0' && text[i] <= '9') {
            value = value * 10 + (text[i] - '0');
        }
        i++;
    }
    return value;
}

float TextTools::toFloat(const char text[]) {
    float value = 0;
    float decimal = 0.1f;
    bool afterPoint = false;
    int i = 0;
    while (text[i] != '\0') {
        if (text[i] == '.') {
            afterPoint = true;
        }
        else if (text[i] >= '0' && text[i] <= '9') {
            if (afterPoint) {
                value = value + (text[i] - '0') * decimal;
                decimal = decimal / 10.0f;
            }
            else {
                value = value * 10.0f + (text[i] - '0');
            }
        }
        i++;
    }
    return value;
}

void TextTools::intToText(int value, char output[], int maxSize) {
     snprintf(output, maxSize, "%d", value);
}

void TextTools::floatToText(float value, char output[], int maxSize) {
     snprintf(output, maxSize, "%.2f", value);
}

bool TextTools::containsOnlyDigits(const char text[]) {
    if (isEmpty(text)) {
        return false;
    }
    int i = 0;
    while (text[i] != '\0') {
        if (text[i] < '0' || text[i] > '9') {
            return false;
        }
        i++;
    }
    return true;
}

void TextTools::clear(char text[]) {
    text[0] = '\0';
}

void TextTools::readPart(const char line[], int partNumber, char output[], int maxSize) {
    int currentPart = 0;
    int outIndex = 0;
    int i = 0;
    output[0] = '\0';

    while (line[i] != '\0' && line[i] != '\n') {
        if (line[i] == ',') {
            currentPart++;
        }
        else if (currentPart == partNumber && outIndex < maxSize - 1) {
            output[outIndex] = line[i];
            outIndex++;
        }
        i++;
    }
    output[outIndex] = '\0';
}