#ifndef APP_H
#define APP_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"

class App 
{
private:
    struct Button 
    {
        sf::FloatRect area;
        char label[80];
        char value[20];
        int  kind;
        bool danger;
        bool selected;
    };

    struct Toast
    {
        char   text[300];
        bool   success;
        float  alpha;        
        bool   active;
    };

    sf::RenderWindow window;
    sf::Font         font;
    sf::Font         fontBold;  

    sf::SoundBuffer clickBuffer;
    sf::SoundBuffer successBuffer;
    sf::SoundBuffer errorBuffer;
    sf::Sound       clickSound;
    sf::Sound       successSound;
    sf::Sound       errorSound;

    Storage<Patient>     patients;
    Storage<Doctor>      doctors;
    Storage<Appointment> appointments;
    Storage<Bill>        bills;
    Storage<Prescription>prescriptions;
    Admin                admin;

    Button buttons[60];
    int    buttonCount;

    int  screen;
    int  action;
    int  step;
    int  roleChoice;
    int  loginId;
    int  currentPatientId;
    int  currentDoctorId;
    int  failedAttempts;
    int  selectedDoctorId;
    int  selectedAppointmentId;

    bool inputActive;
    bool inputIsPassword;  

    int  hoveredNav;        

    Toast toast;

    sf::Clock animClock;
    float     animTime;     

    float scrollOffset;
    float contentHeight;

    char input[600];
    char prompt[300];
    char message[8000];
    char title[120];
    char subtitle[200];
    char notice[300];
    char tempA[600];
    char tempB[600];
    char tempC[600];
    char tempD[600];
    char tempE[600];

    int WIN_W;
    int WIN_H;
    static const int SIDE_W = 260;  
    static const int HDR_H = 80;    
    int CONTENT_X;
    int CONTENT_Y;
    int CONTENT_W;

    sf::Color COL_RED;
    sf::Color COL_RED_DARK;
    sf::Color COL_RED_LIGHT;
    sf::Color COL_BLACK;
    sf::Color COL_DARK;
    sf::Color COL_MID;
    sf::Color COL_LIGHT;
    sf::Color COL_WHITE;
    sf::Color COL_BG;
    sf::Color COL_SUCCESS;
    sf::Color COL_SUCCESS_DARK;

public:
    App();
    void run();

private:
    void loadData();
    void loadSounds();
    void processEvents();
    void handleSubmit();
    void handleClick(float x, float y);
    void handleScroll(float delta);
    void render();
    void updateAnimations();

    void drawText(const char text[], float x, float y,
        unsigned int size, sf::Color color,
        bool bold = false);
    void drawTextCentered(const char text[], float cx, float y,
        unsigned int size, sf::Color color);
    void drawLineText(const char text[], float x, float y,
        unsigned int size, sf::Color color, int maxLines);
    void drawRoundRect(float x, float y, float w, float h,
        sf::Color fill, sf::Color outline = sf::Color::Transparent,
        float thickness = 0.f);
    void drawButton(float x, float y, float w, float h,
        const char label[], const char value[],
        int kind, bool danger,
        bool fullRed = false, bool iconLeft = false);
    void drawCard(float x, float y, float w, float h,
        const char icon[], const char heading[],
        const char value[], const char detail[],
        sf::Color accent);
    void drawBadge(float x, float y, const char text[], sf::Color bg, sf::Color fg);
    void drawDivider(float x, float y, float w);
    void drawScrollableMessage(float x, float y, float w, float h);

    void drawSidebar();
    void drawHeader();
    void drawToast();
    void drawInputBar();

    void drawRoleScreen();
    void drawLoginScreen();
    void drawDashboard();
    void drawActionScreen();

    bool isMouseOver(float x, float y, float w, float h) const;
    void addButton(float x, float y, float w, float h,
        const char label[], const char value[],
        int kind, bool danger);

    void setPrompt(const char text[], bool isPassword = false);
    void setMessage(const char text[]);
    void addMessage(const char text[]);
    void setTitle(const char heading[], const char sub[]);
    void setToast(const char text[], bool success);
    void clearInput();
    void playClick();
    void playSuccess();
    void playError();

    void showRoleMenu();
    void showPatientMenu();
    void showDoctorMenu();
    void showAdminMenu();
    void startLoginId(int role);
    void finishLogin();
    void logout();

    Patient* currentPatient();
    Doctor* currentDoctor();
    Doctor* findDoctor(int id);
    Patient* findPatient(int id);
    Appointment* findAppointment(int id);
    Bill* findBill(int id);
    const char* doctorName(int id);
    const char* patientName(int id);

    void patientChoice(int choice);
    void doctorChoice(int choice);
    void adminChoice(int choice);
    void actionInput();

    void viewMyAppointments();
    void viewMyRecords();
    void viewMyBills();
    void viewAllPatients();
    void viewAllDoctors();
    void viewAllAppointments();
    void viewUnpaidBills();
    void dailyReport();

    bool patientHasPending(int patientId);
    bool patientHasUnpaid(int patientId);
    bool doctorHasPending(int doctorId);
    bool prescriptionExists(int appointmentId);
    bool doctorCanSeePatient(int doctorId, int patientId);
    void sortAppointmentsByDate(Appointment list[], int count, bool desc);
    void sortPrescriptionsByDate(Prescription list[], int count, bool desc);
    void sortAppointmentsByTime(Appointment list[], int count);
    void showAvailableSlots(int doctorId, const char date[]);

};

#endif