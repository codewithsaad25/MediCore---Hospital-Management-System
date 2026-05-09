#include "Appointment.h"
#include "TextTools.h"
#include<iostream>
using namespace std;

Appointment::Appointment() {
    id = 0;
    patientId = 0;
    doctorId = 0;
    TextTools::clear(date);
    TextTools::clear(timeSlot);
    TextTools::copy(status, "pending", 16);
}

Appointment::Appointment(int newId, int newPatientId, int newDoctorId,
    const char newDate[], const char newTimeSlot[], const char newStatus[]) {
    id = newId;
    patientId = newPatientId;
    doctorId = newDoctorId;
    TextTools::copy(date, newDate, 11);
    TextTools::copy(timeSlot, newTimeSlot, 6);
    TextTools::copy(status, newStatus, 16);
}

int Appointment::getId() const { return id; }
int Appointment::getPatientId() const { return patientId; }
int Appointment::getDoctorId() const { return doctorId; }
const char* Appointment::getDate() const { return date; }
const char* Appointment::getTimeSlot() const { return timeSlot; }
const char* Appointment::getStatus() const { return status; }

void Appointment::setStatus(const char newStatus[]) {
    TextTools::copy(status, newStatus, 16);
}

bool Appointment::isCancelled() const {
    return TextTools::equals(status, "cancelled");
}

bool Appointment::operator==(const Appointment& other) const {
    return doctorId == other.doctorId
        && TextTools::equals(date, other.date)
        && TextTools::equals(timeSlot, other.timeSlot)
        && !isCancelled()
        && !other.isCancelled();
}

void Appointment::toFileLine(char line[], int maxSize) const {
     snprintf(line, maxSize, "%d,%d,%d,%s,%s,%s", id, patientId, doctorId, date, timeSlot, status);
}

ostream& operator<<(ostream& output, const Appointment& appointment) {
    output << appointment.getId() << " | " << appointment.getPatientId()
        << " | " << appointment.getDoctorId() << " | " << appointment.getDate()
        << " | " << appointment.getTimeSlot() << " | " << appointment.getStatus();
    return output;
}