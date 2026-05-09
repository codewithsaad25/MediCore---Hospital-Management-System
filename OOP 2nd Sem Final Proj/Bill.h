#ifndef BILL_H
#define BILL_H

class Bill
{
private:
    int id;
    int patientId;
    int appointmentId;
    float amount;
    char status[16];
    char date[11];

public:
    Bill();
    Bill(int newId, int newPatientId, int newAppointmentId, float newAmount,
        const char newStatus[], const char newDate[]);

    int getId() const;
    int getPatientId() const;
    int getAppointmentId() const;
    float getAmount() const;
    const char* getStatus() const;
    const char* getDate() const;

    void setStatus(const char newStatus[]);
    void toFileLine(char line[], int maxSize) const;
};

#endif
