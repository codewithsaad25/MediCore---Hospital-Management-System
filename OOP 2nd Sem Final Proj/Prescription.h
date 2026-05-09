#ifndef PRESCRIPTION_H
#define PRESCRIPTION_H

#include"Prescription.h"

class Prescription {
private:
    int id;
    int appointmentId;
    int patientId;
    int doctorId;
    char date[11];
    char medicines[500];
    char notes[300];

public:
    Prescription();
    Prescription(int newId, int newAppointmentId, int newPatientId, int newDoctorId,
        const char newDate[], const char newMedicines[], const char newNotes[]);

    int getId() const;
    int getAppointmentId() const;
    int getPatientId() const;
    int getDoctorId() const;
    const char* getDate() const;
    const char* getMedicines() const;
    const char* getNotes() const;

    void toFileLine(char line[], int maxSize) const;
};

#endif
