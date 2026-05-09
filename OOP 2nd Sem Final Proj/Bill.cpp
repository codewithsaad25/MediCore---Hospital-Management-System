#include "Bill.h"
#include "TextTools.h"
#include <cstdio>
#include<iostream>
using namespace std;

Bill::Bill()
{
    id = 0;
    patientId = 0;
    appointmentId = 0;
    amount = 0.0f;
    TextTools::copy(status, "unpaid", 16);
    TextTools::clear(date);
}

Bill::Bill(int newId, int newPatientId, int newAppointmentId, float newAmount, const char newStatus[], const char newDate[]) 
{
    id = newId;
    patientId = newPatientId;
    appointmentId = newAppointmentId;
    amount = newAmount;
    TextTools::copy(status, newStatus, 16);
    TextTools::copy(date, newDate, 11);
}

int Bill::getId() const { return id; }
int Bill::getPatientId() const { return patientId; }
int Bill::getAppointmentId() const { return appointmentId; }
float Bill::getAmount() const { return amount; }
const char* Bill::getStatus() const { return status; }
const char* Bill::getDate() const { return date; }

void Bill::setStatus(const char newStatus[])
{
    TextTools::copy(status, newStatus, 16);
}

void Bill::toFileLine(char line[], int maxSize) const 
{
     snprintf(line, maxSize, "%d,%d,%d,%.2f,%s,%s", id, patientId, appointmentId, amount, status, date);
}