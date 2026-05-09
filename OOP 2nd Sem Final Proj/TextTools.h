#ifndef TEXTTOOLS_H
#define TEXTTOOLS_H

class TextTools {
public:
    static int length(const char text[]);
    static void copy(char destination[], const char source[], int maxSize);
    static void append(char destination[], const char source[], int maxSize);
    static bool equals(const char first[], const char second[]);
    static bool equalsIgnoreCase(const char first[], const char second[]);
    static bool isEmpty(const char text[]);
    static int toInt(const char text[]);
    static float toFloat(const char text[]);
    static void intToText(int value, char output[], int maxSize);
    static void floatToText(float value, char output[], int maxSize);
    static bool containsOnlyDigits(const char text[]);
    static void clear(char text[]);
    static void readPart(const char line[], int partNumber, char output[], int maxSize);
};

#endif
