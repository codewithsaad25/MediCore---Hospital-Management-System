#include "FileHandler.h"
#include "TextTools.h"
#include <fstream>
#include <ctime>
#include<iostream>
using namespace std;

void FileHandler::makeMissingFiles() {
     ifstream adminCheck("admin.txt");
    if (!adminCheck.good()) {
         ofstream admin("admin.txt");
        admin << "1,Admin,admin123\n";
    }
    adminCheck.close();

    const char names[7][24] = {
        "patients.txt", "doctors.txt", "appointments.txt", "bills.txt",
        "prescriptions.txt", "security_log.txt", "discharged.txt"
    };

    for (int i = 0; i < 7; i++) {
         ifstream check(names[i]);
        if (!check.good()) {
             ofstream create(names[i]);
            if (TextTools::equals(names[i], "patients.txt")) {
                create << "1,Ahmed Ali,28,M,03001234567,pass123,5000.00\n";
            }
            else if (TextTools::equals(names[i], "doctors.txt")) {
                create << "1,Dr. Sara Khan,Cardiology,03111234567,doc456,1500.00\n";
                create << "2,Dr. Hamza Noor,Dermatology,03119876543,skin123,1200.00\n";
                create << "3,Dr. Ayesha Rauf,Neurology,03019876543,brain1,2000.00\n";
            }
        }
        check.close();
    }
}

bool FileHandler::loadPatients(Storage<Patient>& patients) {
    patients.clear();
     ifstream file("patients.txt");
    if (!file.is_open()) return false;
    char line[300];
    while (file.getline(line, 300)) {
        if (TextTools::isEmpty(line)) continue;
        char part[7][80];
        for (int i = 0; i < 7; i++) TextTools::readPart(line, i, part[i], 80);
        patients.add(Patient(TextTools::toInt(part[0]), part[1], TextTools::toInt(part[2]), part[3][0], part[4], part[5], TextTools::toFloat(part[6])));
    }
    return true;
}

bool FileHandler::loadDoctors(Storage<Doctor>& doctors) {
    doctors.clear();
     ifstream file("doctors.txt");
    if (!file.is_open()) return false;
    char line[300];
    while (file.getline(line, 300)) {
        if (TextTools::isEmpty(line)) continue;
        char part[6][80];
        for (int i = 0; i < 6; i++) TextTools::readPart(line, i, part[i], 80);
        doctors.add(Doctor(TextTools::toInt(part[0]), part[1], part[2], part[3], part[4], TextTools::toFloat(part[5])));
    }
    return true;
}

bool FileHandler::loadAdmin(Admin& admin) {
     ifstream file("admin.txt");
    if (!file.is_open()) return false;
    char line[200];
    if (file.getline(line, 200)) {
        char part[3][80];
        for (int i = 0; i < 3; i++) TextTools::readPart(line, i, part[i], 80);
        admin = Admin(TextTools::toInt(part[0]), part[1], part[2]);
    }
    return true;
}

bool FileHandler::loadAppointments(Storage<Appointment>& appointments) {
    appointments.clear();
     ifstream file("appointments.txt");
    if (!file.is_open()) return false;
    char line[300];
    while (file.getline(line, 300)) {
        if (TextTools::isEmpty(line)) continue;
        char part[6][80];
        for (int i = 0; i < 6; i++) TextTools::readPart(line, i, part[i], 80);
        appointments.add(Appointment(TextTools::toInt(part[0]), TextTools::toInt(part[1]), TextTools::toInt(part[2]), part[3], part[4], part[5]));
    }
    return true;
}

bool FileHandler::loadBills(Storage<Bill>& bills) {
    bills.clear();
     ifstream file("bills.txt");
    if (!file.is_open()) return false;
    char line[300];
    while (file.getline(line, 300)) {
        if (TextTools::isEmpty(line)) continue;
        char part[6][80];
        for (int i = 0; i < 6; i++) TextTools::readPart(line, i, part[i], 80);
        bills.add(Bill(TextTools::toInt(part[0]), TextTools::toInt(part[1]), TextTools::toInt(part[2]), TextTools::toFloat(part[3]), part[4], part[5]));
    }
    return true;
}

bool FileHandler::loadPrescriptions(Storage<Prescription>& prescriptions) {
    prescriptions.clear();
     ifstream file("prescriptions.txt");
    if (!file.is_open()) return false;
    char line[1200];
    while (file.getline(line, 1200)) {
        if (TextTools::isEmpty(line)) continue;
        char p0[20], p1[20], p2[20], p3[20], p4[20], p5[500], p6[300];
        TextTools::readPart(line, 0, p0, 20);
        TextTools::readPart(line, 1, p1, 20);
        TextTools::readPart(line, 2, p2, 20);
        TextTools::readPart(line, 3, p3, 20);
        TextTools::readPart(line, 4, p4, 20);
        TextTools::readPart(line, 5, p5, 500);
        TextTools::readPart(line, 6, p6, 300);
        prescriptions.add(Prescription(TextTools::toInt(p0), TextTools::toInt(p1), TextTools::toInt(p2), TextTools::toInt(p3), p4, p5, p6));
    }
    return true;
}

void FileHandler::savePatients(const Storage<Patient>& patients) {
     ofstream file("patients.txt");
    char line[300];
    const Patient* data = patients.getAll();
    for (int i = 0; i < patients.size(); i++) {
        data[i].toFileLine(line, 300);
        writeLine(file, line);
    }
}

void FileHandler::saveDoctors(const Storage<Doctor>& doctors) {
     ofstream file("doctors.txt");
    char line[300];
    const Doctor* data = doctors.getAll();
    for (int i = 0; i < doctors.size(); i++) {
        data[i].toFileLine(line, 300);
        writeLine(file, line);
    }
}

void FileHandler::saveAppointments(const Storage<Appointment>& appointments) {
     ofstream file("appointments.txt");
    char line[300];
    const Appointment* data = appointments.getAll();
    for (int i = 0; i < appointments.size(); i++) {
        data[i].toFileLine(line, 300);
        writeLine(file, line);
    }
}

void FileHandler::saveBills(const Storage<Bill>& bills) {
     ofstream file("bills.txt");
    char line[300];
    const Bill* data = bills.getAll();
    for (int i = 0; i < bills.size(); i++) {
        data[i].toFileLine(line, 300);
        writeLine(file, line);
    }
}

void FileHandler::savePrescriptions(const Storage<Prescription>& prescriptions) {
     ofstream file("prescriptions.txt");
    char line[1200];
    const Prescription* data = prescriptions.getAll();
    for (int i = 0; i < prescriptions.size(); i++) {
        data[i].toFileLine(line, 1200);
        writeLine(file, line);
    }
}

void FileHandler::appendSecurityLog(const char role[], const char enteredId[], const char result[]) {
     ofstream file("security_log.txt",  ios::app);
     time_t now =  time(0);
     tm localTime;
    localtime_s(&localTime, &now);
    char stamp[30];
     strftime(stamp, 30, "%d-%m-%Y %H:%M:%S", &localTime);
    file << stamp << "," << role << "," << enteredId << "," << result << "\n";
}

void FileHandler::readSecurityLog(char output[], int maxSize) {
    output[0] = '\0';
     ifstream file("security_log.txt");
    char line[300];
    while (file.getline(line, 300)) {
        TextTools::append(output, line, maxSize);
        TextTools::append(output, "\n", maxSize);
    }
    if (TextTools::isEmpty(output)) {
        TextTools::copy(output, "No security events logged.", maxSize);
    }
}

void FileHandler::cascadeDeletePatient(int patientId, Storage<Patient>& patients,
    Storage<Appointment>& appointments,
    Storage<Bill>& bills,
    Storage<Prescription>& prescriptions) {
    patients.removeById(patientId);
    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = 0; i < appointments.size(); i++) {
            if (appointments.getAll()[i].getPatientId() == patientId) {
                appointments.removeById(appointments.getAll()[i].getId());
                changed = true;
                break;
            }
        }
    }
    changed = true;
    while (changed) {
        changed = false;
        for (int i = 0; i < bills.size(); i++)
        {
            if (bills.getAll()[i].getPatientId() == patientId)
            {
                bills.removeById(bills.getAll()[i].getId());
                changed = true;
                break;
            }
        }
    }
    changed = true;
    while (changed) 
    {
        changed = false;
        for (int i = 0; i < prescriptions.size(); i++)
        {
            if (prescriptions.getAll()[i].getPatientId() == patientId) 
            {
                prescriptions.removeById(prescriptions.getAll()[i].getId());
                changed = true;
                break;
            }
        }
    }
    savePatients(patients);
    saveAppointments(appointments);
    saveBills(bills);
    savePrescriptions(prescriptions);
}

void FileHandler::archiveAndDeletePatient(int patientId, Storage<Patient>& patients,
    Storage<Appointment>& appointments,
    Storage<Bill>& bills,
    Storage<Prescription>& prescriptions)
{
     ofstream archive("discharged.txt",  ios::app);
    char line[1200];
    Patient* patient = patients.findById(patientId);
    if (patient != 0) {
        archive << "PATIENT," << *patient << "\n"; 
    }
    for (int i = 0; i < appointments.size(); i++) 
    {
        if (appointments.getAll()[i].getPatientId() == patientId)
        {
            archive << "APPOINTMENT," << appointments.getAll()[i] << "\n";      
        }
    }
    for (int i = 0; i < bills.size(); i++) 
    {
        if (bills.getAll()[i].getPatientId() == patientId)
        {
            bills.getAll()[i].toFileLine(line, 1200);
            archive << "BILL," << line << "\n";
        }
    }
    for (int i = 0; i < prescriptions.size(); i++)
    {
        if (prescriptions.getAll()[i].getPatientId() == patientId) 
        {
            prescriptions.getAll()[i].toFileLine(line, 1200);
            archive << "PRESCRIPTION," << line << "\n";
        }
    }
    cascadeDeletePatient(patientId, patients, appointments, bills, prescriptions);
}

int FileHandler::nextPatientId(const Storage<Patient>& patients)
{
    int highest = 0;
    for (int i = 0; i < patients.size(); i++) if (patients.getAll()[i].getId() > highest) highest = patients.getAll()[i].getId();
    return highest + 1;
}

int FileHandler::nextDoctorId(const Storage<Doctor>& doctors) 
{
    int highest = 0;
    for (int i = 0; i < doctors.size(); i++) if (doctors.getAll()[i].getId() > highest) highest = doctors.getAll()[i].getId();
    return highest + 1;
}

int FileHandler::nextAppointmentId(const Storage<Appointment>& appointments)
{
    int highest = 0;
    for (int i = 0; i < appointments.size(); i++) if (appointments.getAll()[i].getId() > highest) highest = appointments.getAll()[i].getId();
    return highest + 1;
}

int FileHandler::nextBillId(const Storage<Bill>& bills)
{
    int highest = 0;
    for (int i = 0; i < bills.size(); i++) if (bills.getAll()[i].getId() > highest) highest = bills.getAll()[i].getId();
    return highest + 1;
}

int FileHandler::nextPrescriptionId(const Storage<Prescription>& prescriptions)
{
    int highest = 0;
    for (int i = 0; i < prescriptions.size(); i++) if (prescriptions.getAll()[i].getId() > highest) highest = prescriptions.getAll()[i].getId();
    return highest + 1;
}

void FileHandler::writeLine( ofstream& file, const char line[])
{
    file << line << "\n";
}