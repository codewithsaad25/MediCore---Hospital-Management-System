#include "Prescription.h"
#include "TextTools.h"
#include <cstdio>
#include<iostream>
using namespace std;

Prescription::Prescription() {
    id = 0;
    appointmentId = 0;
    patientId = 0;
    doctorId = 0;
    TextTools::clear(date);
    TextTools::clear(medicines);
    TextTools::clear(notes);
}

Prescription::Prescription(int newId, int newAppointmentId, int newPatientId, int newDoctorId,
    const char newDate[], const char newMedicines[], const char newNotes[]) {
    id = newId;
    appointmentId = newAppointmentId;
    patientId = newPatientId;
    doctorId = newDoctorId;
    TextTools::copy(date, newDate, 11);
    TextTools::copy(medicines, newMedicines, 500);
    TextTools::copy(notes, newNotes, 300);
}

int Prescription::getId() const { return id; }
int Prescription::getAppointmentId() const { return appointmentId; }
int Prescription::getPatientId() const { return patientId; }
int Prescription::getDoctorId() const { return doctorId; }
const char* Prescription::getDate() const { return date; }
const char* Prescription::getMedicines() const { return medicines; }
const char* Prescription::getNotes() const { return notes; }

void Prescription::toFileLine(char line[], int maxSize) const {
     snprintf(line, maxSize, "%d,%d,%d,%d,%s,%s,%s", id, appointmentId, patientId, doctorId, date, medicines, notes);
}