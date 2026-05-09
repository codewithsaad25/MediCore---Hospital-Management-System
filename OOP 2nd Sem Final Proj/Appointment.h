#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <iostream>

class Appointment
{
private:
    int id;
    int patientId;
    int doctorId;
    char date[11];
    char timeSlot[6];
    char status[16];

public:
    Appointment();
    Appointment(int newId, int newPatientId, int newDoctorId,
        const char newDate[], const char newTimeSlot[], const char newStatus[]);

    int getId() const;
    int getPatientId() const;
    int getDoctorId() const;
    const char* getDate() const;
    const char* getTimeSlot() const;
    const char* getStatus() const;

    void setStatus(const char newStatus[]);
    bool isCancelled() const;
    bool operator==(const Appointment& other) const;
    void toFileLine(char line[], int maxSize) const;
    friend std::ostream& operator<<(std::ostream& output, const Appointment& appointment);
};

#endif