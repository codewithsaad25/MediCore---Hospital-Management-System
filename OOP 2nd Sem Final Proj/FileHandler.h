#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include <fstream>

class FileHandler {
public:
    static void makeMissingFiles();

    static bool loadPatients(Storage<Patient>& patients);
    static bool loadDoctors(Storage<Doctor>& doctors);
    static bool loadAdmin(Admin& admin);
    static bool loadAppointments(Storage<Appointment>& appointments);
    static bool loadBills(Storage<Bill>& bills);
    static bool loadPrescriptions(Storage<Prescription>& prescriptions);

    static void savePatients(const Storage<Patient>& patients);
    static void saveDoctors(const Storage<Doctor>& doctors);
    static void saveAppointments(const Storage<Appointment>& appointments);
    static void saveBills(const Storage<Bill>& bills);
    static void savePrescriptions(const Storage<Prescription>& prescriptions);

    static void appendSecurityLog(const char role[], const char enteredId[], const char result[]);
    static void readSecurityLog(char output[], int maxSize);
    static void cascadeDeletePatient(int patientId, Storage<Patient>& patients,
        Storage<Appointment>& appointments,
        Storage<Bill>& bills,
        Storage<Prescription>& prescriptions);
    static void archiveAndDeletePatient(int patientId, Storage<Patient>& patients,
        Storage<Appointment>& appointments,
        Storage<Bill>& bills,
        Storage<Prescription>& prescriptions);

    static int nextPatientId(const Storage<Patient>& patients);
    static int nextDoctorId(const Storage<Doctor>& doctors);
    static int nextAppointmentId(const Storage<Appointment>& appointments);
    static int nextBillId(const Storage<Bill>& bills);
    static int nextPrescriptionId(const Storage<Prescription>& prescriptions);

private:
    static void writeLine( std::ofstream& file, const char line[]);
};

#endif
