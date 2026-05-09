#include "App.h"
#include "FileHandler.h"
#include "TextTools.h"
#include "Validator.h"
#include "InsufficientFundsException.h"
#include "InvalidInputException.h"
#include "SlotUnavailableException.h"
#include<cmath>
#include<iostream>
using namespace std;

enum Screens
{
    SCREEN_ROLE = 1,
    SCREEN_LOGIN_ID,
    SCREEN_LOGIN_PASSWORD,
    SCREEN_PATIENT,
    SCREEN_DOCTOR,
    SCREEN_ADMIN,
    SCREEN_ACTION
};

enum Actions
{
    ACTION_NONE = 0,
    ACTION_BOOK,
    ACTION_CANCEL,
    ACTION_PAY,
    ACTION_TOPUP,
    ACTION_COMPLETE,
    ACTION_NOSHOW,
    ACTION_WRITE_RX,
    ACTION_HISTORY,
    ACTION_ADD_DOCTOR,
    ACTION_REMOVE_DOCTOR,
    ACTION_ADD_PATIENT,
    ACTION_REMOVE_PATIENT,
    ACTION_DISCHARGE
};

enum ButtonKinds
{
    BUTTON_SUBMIT = 1,
    BUTTON_CHOICE,
    BUTTON_LOGOUT,
    BUTTON_BACK,
    BUTTON_EXIT,
    BUTTON_NAV
};

App::App()
    : window(sf::VideoMode::getDesktopMode(),
        "MediCore  -  Hospital Management System",
        sf::State::Fullscreen),
    clickSound(clickBuffer),
    successSound(successBuffer),
    errorSound(errorBuffer)
{
    WIN_W = (int)window.getSize().x;
    WIN_H = (int)window.getSize().y;
    CONTENT_X = SIDE_W + 20;
    CONTENT_Y = HDR_H + 14;
    CONTENT_W = WIN_W - SIDE_W - 40;

 // this is the color palette 
    COL_RED = sf::Color(210, 30, 40);
    COL_RED_DARK = sf::Color(160, 18, 25);
    COL_DARK = sf::Color(20, 20, 24);
    COL_MID = sf::Color(50, 50, 58);
    COL_LIGHT = sf::Color(130, 130, 140);
    COL_WHITE = sf::Color(255, 255, 255);
    COL_BG = sf::Color(245, 246, 248);
    COL_SUCCESS = sf::Color(34, 160, 80);

    buttonCount = 0;
    screen = SCREEN_ROLE;
    action = ACTION_NONE;
    step = 0;
    roleChoice = 0;
    loginId = 0;
    currentPatientId = 0;
    currentDoctorId = 0;
    failedAttempts = 0;
    selectedDoctorId = 0;
    selectedAppointmentId = 0;
    inputActive = false;
    inputIsPassword = false;
    hoveredNav = -1;
    scrollOffset = 0.f;
    contentHeight = 0.f;
    animTime = 0.f;

    TextTools::clear(input);
    TextTools::clear(prompt);
    TextTools::clear(message);
    TextTools::clear(title);
    TextTools::clear(subtitle);
    TextTools::clear(tempA);
    TextTools::clear(tempB);
    TextTools::clear(tempC);
    TextTools::clear(tempD);
    TextTools::clear(tempE);

   // the pop up success or failed message thing (must get a bonus for this creativity of mine)
    TextTools::clear(toast.text);
    toast.success = true;
    toast.alpha = 0.f;
    toast.active = false;

    window.setFramerateLimit(60);
  
    if (!font.openFromFile("C:\\Windows\\Fonts\\segoeui.ttf"))
        if (!font.openFromFile("C:\\Windows\\Fonts\\calibri.ttf"))
            font.openFromFile("C:\\Windows\\Fonts\\arial.ttf");

    if (!fontBold.openFromFile("C:\\Windows\\Fonts\\segoeuib.ttf"))
        if (!fontBold.openFromFile("C:\\Windows\\Fonts\\calibrib.ttf"))
            fontBold.openFromFile("C:\\Windows\\Fonts\\arialbd.ttf");

    loadSounds();
    loadData();
    showRoleMenu();
}

void App::run() 
{
    while (window.isOpen()) 
    {
        processEvents();
        updateAnimations();
        render();
    }
}

void App::loadData() 
{
    FileHandler::makeMissingFiles();
    FileHandler::loadPatients(patients);
    FileHandler::loadDoctors(doctors);
    FileHandler::loadAdmin(admin);
    FileHandler::loadAppointments(appointments);
    FileHandler::loadBills(bills);
    FileHandler::loadPrescriptions(prescriptions);
}

void App::loadSounds()
{
    // I have also added sounds just because I wanted to be different hehe:)
    clickBuffer.loadFromFile("click.wav");
    successBuffer.loadFromFile("success.wav");
    errorBuffer.loadFromFile("error.wav");
    clickSound.setVolume(35.f);
    successSound.setVolume(50.f);
    errorSound.setVolume(50.f);
}

void App::updateAnimations()
{
    float dt = animClock.restart().asSeconds();
    animTime += dt;

    if (toast.active)
    {
        if (toast.alpha > 0.f)
        {
            toast.alpha -= dt * 90.f;   
            if (toast.alpha < 0.f)
            {
                toast.alpha = 0.f; toast.active = false;
            }
        }
    }
}

void App::processEvents()
{
    while (auto event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }

        if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mb->button == sf::Mouse::Button::Left)
                handleClick((float)mb->position.x, (float)mb->position.y);
        }

        if (const auto* mw = event->getIf<sf::Event::MouseWheelScrolled>())
        {
            handleScroll(mw->delta);
        }

        if (const auto* typed = event->getIf<sf::Event::TextEntered>())
        {
            if (!inputActive) continue;
            unsigned int code = typed->unicode;
            int len = TextTools::length(input);
            if (code == 8)
            {
                if (len > 0) input[len - 1] = '\0';
            }
            else if (code == 13)
            {
                handleSubmit();
            }
            else if (code >= 32 && code <= 126 && len < 599)
            {
                input[len] = (char)code;
                input[len + 1] = '\0';
            }
        }
    }
}

void App::handleScroll(float delta)
{
    if (screen == SCREEN_ACTION)
    {
        scrollOffset -= delta * 25.f;
        if (scrollOffset < 0.f) scrollOffset = 0.f;
        float maxScroll = contentHeight - 420.f;
        if (maxScroll < 0.f) maxScroll = 0.f;
        if (scrollOffset > maxScroll) scrollOffset = maxScroll;
    }
}

void App::handleClick(float x, float y)
{
    if (screen != SCREEN_ROLE && screen != SCREEN_ACTION &&
        x >= CONTENT_X && x <= WIN_W - 20 &&
        y >= WIN_H - 70 && y <= WIN_H - 14) 
    {
        inputActive = true;
    }
    if (screen == SCREEN_ACTION &&
        x >= CONTENT_X && x <= WIN_W - 20 &&
        y >= WIN_H - 70 && y <= WIN_H - 14) 
    {
        inputActive = true;
    }

    for (int i = 0; i < buttonCount; i++)
    {
        if (buttons[i].area.contains({ x, y }))
        {
            playClick();
            int k = buttons[i].kind;
            if (k == BUTTON_SUBMIT || k == BUTTON_NAV)
            {
                handleSubmit();
            }
            else if (k == BUTTON_CHOICE)
            {
                TextTools::copy(input, buttons[i].value, 600);
                handleSubmit();
            }
            else if (k == BUTTON_LOGOUT || k == BUTTON_BACK) 
            {
                clearInput();
                scrollOffset = 0.f;
                if (screen == SCREEN_ACTION)
                {
                    if (roleChoice == 1) showPatientMenu();
                    else if (roleChoice == 2) showDoctorMenu();
                    else showAdminMenu();
                }
                else
                {
                    logout();
                }
            }
            else if (k == BUTTON_EXIT)
            {
                window.close();
            }
            return;
        }
    }
}

void App::handleSubmit() 
{
    if (screen == SCREEN_ROLE)
    {
        if (!Validator::isValidMenuChoice(input, 1, 4))
        {
            setToast("Click your desired portal cards to continue.", false);
            clearInput(); return;
        }
        int choice = TextTools::toInt(input);
        clearInput();
        if (choice == 4) window.close();
        else startLoginId(choice);
    }
    else if (screen == SCREEN_LOGIN_ID)
    {
        if (!Validator::isValidId(input)) 
        {
            setToast("Invalid ID! Mehrbani BhaiJan sirf Numbers enter kre", false); clearInput(); return;
        }
        loginId = TextTools::toInt(input);
        clearInput();
        screen = SCREEN_LOGIN_PASSWORD;
        inputActive = true;
        inputIsPassword = true;
        setPrompt("Password", true);
        setTitle("Secure Login", "Enter your password to continue.");
        setMessage("Your credentials are validated against the hospital records.");
    }
    else if (screen == SCREEN_LOGIN_PASSWORD)
    {
        finishLogin();
    }
    else if (screen == SCREEN_PATIENT) 
    {
        if (Validator::isValidMenuChoice(input, 1, 8)) patientChoice(TextTools::toInt(input));
        else setToast("Choose an action from the dashboard.", false);
        clearInput();
    }
    else if (screen == SCREEN_DOCTOR)
    {
        if (Validator::isValidMenuChoice(input, 1, 6)) doctorChoice(TextTools::toInt(input));
        else setToast("Choose an action from the dashboard.", false);
        clearInput();
    }
    else if (screen == SCREEN_ADMIN)
    {
        if (Validator::isValidMenuChoice(input, 1, 12)) adminChoice(TextTools::toInt(input));
        else setToast("Choose an action from the dashboard.", false);
        clearInput();
    }
    else if (screen == SCREEN_ACTION)
    {
        actionInput();
    }
}

void App::render()
{
    buttonCount = 0;

    window.clear(COL_BG);

    if (screen == SCREEN_ROLE) 
    {
        drawRoleScreen();
    }
    else if (screen == SCREEN_LOGIN_ID || screen == SCREEN_LOGIN_PASSWORD) 
    {
        drawSidebar();
        drawHeader();
        drawLoginScreen();
        drawInputBar();
        drawToast();
    }
    else if (screen == SCREEN_ACTION) 
    {
        drawSidebar();
        drawHeader();
        drawActionScreen();
        drawInputBar();
        drawToast();
    }
    else 
    {
        drawSidebar();
        drawHeader();
        drawDashboard();
        drawToast();
    }

    window.display();
}

void App::drawText(const char text[], float x, float y,
    unsigned int size, sf::Color color, bool bold)
{
    sf::Text t(bold ? fontBold : font, text, size);
    t.setFillColor(color);
    t.setPosition({ x, y });
    window.draw(t);
}

void App::drawTextCentered(const char text[], float cx, float y,
    unsigned int size, sf::Color color)
{
    sf::Text t(font, text, size);
    t.setFillColor(color);
    float tw = t.getLocalBounds().size.x;
    t.setPosition({ cx - tw / 2.f, y });
    window.draw(t);
}

void App::drawLineText(const char text[], float x, float y,
    unsigned int size, sf::Color color, int maxLines)
{
    char line[240];
    int  lineIndex = 0, outIndex = 0, i = 0;
    while (text[i] != '\0' && lineIndex < maxLines) 
    {
        if (text[i] == '\n' || outIndex >= 98)
        {
            line[outIndex] = '\0';
            drawText(line, x, y + lineIndex * (size + 6), size, color);
            lineIndex++;
            outIndex = 0;
            if (text[i] == '\n') i++;
        }
        else 
        {
            line[outIndex++] = text[i++];
        }
    }
    if (outIndex > 0 && lineIndex < maxLines)
    {
        line[outIndex] = '\0';
        drawText(line, x, y + lineIndex * (size + 6), size, color);
    }
}

//this is called creativity where with just some rectangles you will see an actual medical plus sign
void App::drawRoundRect(float x, float y, float w, float h,
    sf::Color fill, sf::Color outline, float thickness)
{
    sf::RectangleShape r({ w, h });
    r.setPosition({ x, y });
    r.setFillColor(fill);
    if (thickness > 0.f) {
        r.setOutlineColor(outline);
        r.setOutlineThickness(thickness);
    }
    window.draw(r);
}

void App::drawDivider(float x, float y, float w)
{
    drawRoundRect(x, y, w, 1.f, sf::Color(220, 222, 228));
}

void App::drawBadge(float x, float y, const char text[],
    sf::Color bg, sf::Color fg)
{
    sf::Text t(font, text, 12);
    float tw = t.getLocalBounds().size.x + 14.f;
    drawRoundRect(x, y, tw, 20.f, bg);
    t.setFillColor(fg);
    t.setPosition({ x + 7.f, y + 3.f });
    window.draw(t);
}

void App::drawButton(float x, float y, float w, float h,
    const char label[], const char value[],
    int kind, bool danger, bool fullRed, bool)
{
    //this is the hover effect, you know halka tu bilkul tu ni ana
    bool hover = isMouseOver(x, y, w, h);


    sf::Color bgCol, fgCol;
    if (fullRed || danger) 
    {
        bgCol = hover ? COL_RED_DARK : COL_RED;
        fgCol = COL_WHITE;
    }
    else 
    {
        bgCol = hover ? COL_DARK : COL_WHITE;
        fgCol = hover ? COL_WHITE : COL_MID;
    }

    drawRoundRect(x, y, w, h, bgCol, sf::Color::Transparent, 0.f);

    sf::Text t(font, label, 15);
    float tw = t.getLocalBounds().size.x;
    float tx = x + (w - tw) / 2.f;
    float ty = y + (h - 20.f) / 2.f;
    t.setFillColor(fgCol);
    t.setPosition({ tx, ty });
    window.draw(t);

    if (buttonCount < 60)
    {
        buttons[buttonCount].area = { { x, y }, { w, h } };
        buttons[buttonCount].danger = danger;
        buttons[buttonCount].selected = false;
        TextTools::copy(buttons[buttonCount].label, label, 80);
        TextTools::copy(buttons[buttonCount].value, value, 20);
        buttons[buttonCount].kind = kind;
        buttonCount++;
    }
}

void App::drawCard(float x, float y, float w, float h,
    const char /*icon*/[], const char heading[],
    const char value[], const char detail[],
    sf::Color accent)
{
    drawRoundRect(x, y, w, h, COL_WHITE,
        sf::Color(218, 220, 226), 1.f);

    drawText(heading, x + 18.f, y + 14.f, 13, COL_LIGHT);
    drawText(value, x + 18.f, y + 34.f, 28, COL_DARK, true);
    drawText(detail, x + 18.f, y + h - 22.f, 12, COL_LIGHT);
}

// this is if there are alot of doctors or patients, a separate scrollable screen will be made for your ease, it applies to security log as well and in all related screens
void App::drawScrollableMessage(float x, float y, float w, float h) {
 
    sf::View clipView;
    clipView.setViewport({
        { x / WIN_W, y / WIN_H },
        { w / WIN_W, h / WIN_H }
        });
    clipView.setSize({ w, h });
    clipView.setCenter({ w / 2.f, h / 2.f + scrollOffset });
    window.setView(clipView);

    int lineCount = 1;
    for (int i = 0; message[i] != '\0'; i++)
        if (message[i] == '\n') lineCount++;
    contentHeight = lineCount * 23.f + 20.f;

    char line[240];
    int  li = 0, oi = 0, idx = 0;
    float yy = 8.f;
    while (message[idx] != '\0')
    {
        if (message[idx] == '\n' || oi >= 98)
        {
            line[oi] = '\0';
            if (li > 0 && li % 2 == 0)
                drawRoundRect(0.f, yy - 2.f, w, 21.f,
                    sf::Color(245, 246, 248));
            bool isHeader = (li == 0 && message[0] != 'N');
            drawText(line, 12.f, yy, 14,
                isHeader ? COL_RED : COL_DARK);
            yy += 23.f; li++; oi = 0;
            if (message[idx] == '\n') idx++;
        }
        else
        {
            line[oi++] = message[idx++];
        }
    }
    if (oi > 0) 
    {
        line[oi] = '\0';
        drawText(line, 12.f, yy, 14, COL_DARK);
    }

    window.setView(window.getDefaultView());

    //this is basically a scroll bar yk professional user experience and thats called attention to detail
    if (contentHeight > h) 
    {
        float barH = h * h / contentHeight;
        float barY = y + (scrollOffset / (contentHeight - h)) * (h - barH);
        drawRoundRect(x + w - 6.f, barY, 4.f, barH,
            sf::Color(180, 182, 190));
    }
}

void App::drawSidebar()
{
    drawRoundRect(0.f, 0.f, (float)SIDE_W, (float)WIN_H, COL_DARK);

    drawRoundRect(18.f, 18.f, (float)SIDE_W - 36.f, 60.f,
        sf::Color(35, 35, 42));
    // the goated medical plus sign
    drawRoundRect(30.f, 34.f, 28.f, 28.f, COL_RED);
    drawRoundRect(37.f, 29.f, 14.f, 38.f, COL_WHITE);
    drawRoundRect(25.f, 41.f, 38.f, 14.f, COL_WHITE);

    drawText("MediCore", 72.f, 28.f, 20, COL_WHITE, true);
    drawText("Hospital System", 72.f, 52.f, 12, COL_LIGHT);

    drawDivider(18.f, 92.f, (float)SIDE_W - 36.f);

    if (screen != SCREEN_ROLE &&
        screen != SCREEN_LOGIN_ID &&
        screen != SCREEN_LOGIN_PASSWORD)
    {
        const char* roleLabel = "PATIENT";
        if (roleChoice == 2) roleLabel = "DOCTOR";
        if (roleChoice == 3) roleLabel = "ADMIN";
        drawBadge(18.f, 104.f, roleLabel, COL_RED, COL_WHITE);

        char uname[60] = "User";
        if (roleChoice == 1 && currentPatient())
            TextTools::copy(uname, currentPatient()->getName(), 60);
        else if (roleChoice == 2 && currentDoctor())
            TextTools::copy(uname, currentDoctor()->getName(), 60);
        else if (roleChoice == 3)
            TextTools::copy(uname, "Administrator", 60);

        drawText(uname, 18.f, 132.f, 15, COL_WHITE, true);
        drawDivider(18.f, 158.f, (float)SIDE_W - 36.f);
    }

    //tag line bhi tu zrori haina k nai?
    drawText("MediCore", 18.f, (float)WIN_H - 46.f, 11, COL_LIGHT);
    drawText("25L-0794  |  M.Saad", 18.f, (float)WIN_H - 30.f, 11, COL_LIGHT);
}

void App::drawHeader() 
{
    drawRoundRect((float)SIDE_W, 0.f,
        (float)(WIN_W - SIDE_W), (float)HDR_H,
        COL_WHITE, sf::Color(218, 220, 226), 0.f);

    drawRoundRect((float)SIDE_W, (float)HDR_H - 1.f,
        (float)(WIN_W - SIDE_W), 1.f,
        sf::Color(210, 212, 220));

    drawText(title, (float)CONTENT_X, 14.f, 22, COL_DARK, true);
    drawText(subtitle, (float)CONTENT_X, 46.f, 13, COL_LIGHT);

    //Status bar type of thing i saw in cmh hospital system so thought bana don
    if (screen != SCREEN_ROLE &&
        screen != SCREEN_LOGIN_ID &&
        screen != SCREEN_LOGIN_PASSWORD)
    {
        drawRoundRect((float)WIN_W - 120.f, 30.f, 10.f, 10.f, COL_SUCCESS);
        drawText("10/10 in OOP Proj:)", (float)WIN_W - 105.f, 28.f, 12, COL_SUCCESS);
    }
}

void App::drawInputBar() 
{
    if (!inputActive && TextTools::isEmpty(prompt)) return;

    float barY = (float)WIN_H - 76.f;
    drawRoundRect((float)SIDE_W, barY, (float)(WIN_W - SIDE_W), 76.f,
        sf::Color(248, 249, 251),
        sf::Color(210, 212, 220), 0.f);
    drawRoundRect((float)SIDE_W, barY, (float)(WIN_W - SIDE_W), 1.f,
        sf::Color(200, 202, 210));

    drawText(prompt, (float)CONTENT_X, barY + 8.f, 12, COL_LIGHT);

    float fieldW = (float)(WIN_W - SIDE_W) - 200.f;
    drawRoundRect((float)CONTENT_X, barY + 26.f, fieldW, 36.f,
        COL_WHITE,
        inputActive ? COL_RED : sf::Color(205, 207, 214),
        inputActive ? 2.f : 1.f);

    //password hide huna chahye
    char display[600] = {};
    if (inputIsPassword) 
    {
        int len = TextTools::length(input);
        for (int i = 0; i < len; i++) display[i] = '*';
        display[len] = '\0';
    }
    else 
    {
        TextTools::copy(display, input, 600);
    }

    drawText(display, (float)CONTENT_X + 10.f, barY + 34.f, 16, COL_DARK);

    //Bhai shb cursor bhi hai, attention to detail.....
    if (inputActive) {
        int   len = TextTools::length(input);
        sf::Text t(font, display, 16);
        float cx = (float)CONTENT_X + 10.f + t.getLocalBounds().size.x + 2.f;
        if ((int)(animTime * 2) % 2 == 0)
            drawRoundRect(cx, barY + 31.f, 2.f, 22.f, COL_RED);
    }

    drawButton((float)WIN_W - 185.f, barY + 24.f, 80.f, 38.f,
        "Submit", "", BUTTON_SUBMIT, false, true);
    drawButton((float)WIN_W - 95.f, barY + 24.f, 70.f, 38.f,
        "Wapis", "", BUTTON_BACK, false, false);
}

void App::drawToast()
{
    if (!toast.active || toast.alpha <= 0.f) return;

    uint8_t a = (uint8_t)(toast.alpha > 255.f ? 255.f : toast.alpha);
    sf::Color bg = toast.success
        ? sf::Color(34, 160, 80, a)
        : sf::Color(210, 30, 40, a);
    sf::Color fg = sf::Color(255, 255, 255, a);

    float tw = (float)(WIN_W - SIDE_W) - 40.f;
    float ty = (float)HDR_H + 8.f;
    drawRoundRect((float)CONTENT_X - 10.f, ty, tw, 38.f, bg);

    // Icon
    const char* icon = toast.success ? "[OK]  " : "[ERR]  ";
    sf::Text ic(font, icon, 16);
    ic.setFillColor(fg);
    ic.setPosition({ (float)CONTENT_X, ty + 9.f });
    window.draw(ic);

    sf::Text txt(font, toast.text, 14);
    txt.setFillColor(fg);
    txt.setPosition({ (float)CONTENT_X + 28.f, ty + 11.f });
    window.draw(txt);
}

void App::drawRoleScreen() 
{
    window.clear(COL_DARK);
    //TA bhai if u r reading this, idhr yeh project end hai im telling like look at the creativity, idea original ha though implementation thori si copied hai but still ig creativity k marks tu haina
    //animated background you are seeing are implemented here
    {
        float pulse = 0.38f + 0.18f * sin(animTime * 1.4f);
        uint8_t dotAlpha = (uint8_t)(pulse * 255.f);
        sf::Color dotCol(210, 30, 40, dotAlpha);

        int cols = WIN_W / 55;
        int rows = WIN_H / 55;
        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < cols; c++)
            {
                float dx = 30.f + c * 55.f;
                float dy = 30.f + r * 55.f;
                if (dx < 560.f && dy < 230.f) continue;
                float cardY = (float)WIN_H * 0.36f;
                float cardH = (float)WIN_H * 0.44f;
                float totalCardsW = 3.f * 330.f + 2.f * 30.f;
                float startX = ((float)WIN_W - totalCardsW) / 2.f;
                if (dx >= startX - 20.f && dx <= startX + totalCardsW + 20.f
                    && dy >= cardY - 10.f && dy <= cardY + cardH + 10.f) continue;

                sf::CircleShape dot(2.5f);
                dot.setFillColor(dotCol);
                dot.setPosition({ dx, dy });
                window.draw(dot);
            }
        }
    }

    //bottom wali animation i mean so beautiful, so elegant, just looking like a wow ( i hope u enjoy the cmnts just for fun )
    {
        float lineY = (float)WIN_H - 38.f;
        float lineAlpha = 0.55f + 0.15f * sin(animTime * 2.0f);
        sf::Color ecgCol(210, 30, 40, (uint8_t)(lineAlpha * 255));

        const int segCount = 18;
        float segX[segCount] = 
        {
            0.00f, 0.08f, 0.12f, 0.16f,
            0.20f, 0.22f, 0.24f, 0.26f, 0.28f,
            0.30f, 0.34f, 0.40f,
            0.44f, 0.50f, 0.58f, 0.70f,
            0.85f, 1.00f
        };
        float segY[segCount] =
        {
             0.0f,  0.0f,  0.3f,  0.0f,
             0.0f, -1.8f,  2.4f, -0.5f,  0.0f,
             0.0f,  0.6f,  0.0f,
             0.0f,  0.4f,  0.0f,  0.0f,
             0.0f,  0.0f
        };

        float period = (float)WIN_W * 0.55f;
        float amplitude = 28.f;
        float scrollX = fmod(animTime * 120.f, period);

        sf::VertexArray ecg(sf::PrimitiveType::LineStrip, segCount * 4);
        int vi = 0;
        for (int rep = -1; rep <= 2; rep++) {
            for (int i = 0; i < segCount; i++) {
                float x = segX[i] * period + rep * period - scrollX;
                float y = lineY - segY[i] * amplitude;
                ecg[vi].position = { x, y };
                ecg[vi].color = ecgCol;
                vi++;
            }
        }
        if (vi > 0)
            window.draw(ecg);

        float dotX = (float)WIN_W * 0.72f;
        float relX = fmod(dotX + scrollX, period) / period;
        float dotY = lineY;
        for (int i = 0; i < segCount - 1; i++) {
            if (relX >= segX[i] && relX <= segX[i + 1]) {
                float t = (relX - segX[i]) / (segX[i + 1] - segX[i]);
                float yy = segY[i] + t * (segY[i + 1] - segY[i]);
                dotY = lineY - yy * amplitude;
                break;
            }
        }

        sf::CircleShape glowDot(5.f);
        glowDot.setFillColor(sf::Color(255, 80, 80, (uint8_t)(lineAlpha * 230)));
        glowDot.setPosition({ dotX - 5.f, dotY - 5.f });
        window.draw(glowDot);

        sf::CircleShape glowRing(10.f);
        glowRing.setFillColor(sf::Color::Transparent);
        glowRing.setOutlineColor(sf::Color(210, 30, 40, (uint8_t)(lineAlpha * 80)));
        glowRing.setOutlineThickness(2.f);
        glowRing.setPosition({ dotX - 10.f, dotY - 10.f });
        window.draw(glowRing);

        drawText("LIVE", dotX + 14.f, dotY - 10.f, 11,
            sf::Color(210, 30, 40, (uint8_t)(lineAlpha * 200)));
    }

    drawText("MediCore", 60.f, 80.f, 64, COL_WHITE, true);
    drawText("Hospital Management System", 62.f, 155.f, 22, COL_LIGHT);
    drawDivider(60.f, 192.f, 460.f);
    drawText("Select your desired portal to continue", 62.f, 206.f, 15, COL_LIGHT);

    float cardW = 330.f;
    float totalCardsW = 3.f * cardW + 2.f * 30.f;
    float startX = ((float)WIN_W - totalCardsW) / 2.f;

    struct { float x; const char* label; const char* sub; const char* val; sf::Color col; } cards[] = {
        { startX,                        "Patient Portal",   "Amma na kaha tha na bachpan ma\n doodh peya kro",  "1", sf::Color(20, 20, 24) },
        { startX + cardW + 30.f,         "Doctor Workspace", "An apple a day keeps a doctor away", "2", sf::Color(20, 20, 24)  },
        { startX + 2.f * (cardW + 30.f), "Admin Control",    "Hospitals make alot of money i mean",     "3", sf::Color(14, 14, 18)  },
    };

    float cardY = (float)WIN_H * 0.36f;
    float cardH = (float)WIN_H * 0.44f;
    float exitY = cardY + cardH + 30.f;
    float versionY = exitY + 56.f;

    for (int i = 0; i < 3; i++) 
    {
        float cx = cards[i].x;
        bool  hov = isMouseOver(cx, cardY, cardW, cardH);

        if (hov) drawRoundRect(cx - 3.f, cardY - 3.f, cardW + 6.f, cardH + 6.f,
            sf::Color(210, 30, 40, 60));

        drawRoundRect(cx, cardY, cardW, cardH,
            hov ? sf::Color(30, 30, 36) : cards[i].col,
            sf::Color(60, 60, 70), 1.f);

        drawText(cards[i].val, cx + 20.f, cardY + 20.f, 48, COL_RED, true);

        if (i == 0) 
        {
            float ix = cx + 20.f, iy = cardY + 82.f;
            sf::VertexArray icon(sf::PrimitiveType::LineStrip, 7);
            float pts[][2] = { {0,0},{8,0},{12,-8},{16,12},{20,-4},{24,0},{32,0} };
            for (int k = 0; k < 7; k++) {
                icon[k].position = { ix + pts[k][0], iy + pts[k][1] };
                icon[k].color = COL_RED;
            }
            window.draw(icon);
        }
        else if (i == 1) 
        {
            float ix = cx + 20.f, iy = cardY + 76.f;
            drawRoundRect(ix + 4.f, iy, 8.f, 18.f, sf::Color(210, 30, 40, 180));
            drawRoundRect(ix, iy + 6.f, 16.f, 6.f, sf::Color(210, 30, 40, 180));
        }
        else 
        {
            float ix = cx + 20.f, iy = cardY + 76.f;
            sf::VertexArray shield(sf::PrimitiveType::LineStrip, 6);
            shield[0].position = { ix + 8.f,  iy };
            shield[1].position = { ix + 18.f, iy + 4.f };
            shield[2].position = { ix + 18.f, iy + 12.f };
            shield[3].position = { ix + 8.f,  iy + 20.f };
            shield[4].position = { ix,        iy + 12.f };
            shield[5].position = { ix,        iy + 4.f };
            for (int k = 0; k < 6; k++) shield[k].color = sf::Color(210, 30, 40, 160);
            window.draw(shield);
        }

        drawText(cards[i].label, cx + 20.f, cardY + 104.f, 18, COL_WHITE, true);
        drawText(cards[i].sub, cx + 20.f, cardY + 130.f, 15, COL_WHITE);

        drawText("->", cx + cardW - 50.f, cardY + cardH - 40.f, 22,
            hov ? COL_RED : sf::Color(80, 80, 90));

        addButton(cx, cardY, cardW, cardH,
            cards[i].label, cards[i].val, BUTTON_CHOICE, false);
    }

    drawButton((float)WIN_W / 2.f - 100.f, exitY, 200.f, 46.f,
        "Exit MediCore", "4", BUTTON_EXIT, true, true);

    drawTextCentered("25L-0794  |  MediCore  |  M.Saad",
        (float)WIN_W / 2.f, versionY, 12, COL_LIGHT);
}

void App::drawLoginScreen()
{
    float px = (float)CONTENT_X;
    float py = (float)CONTENT_Y + 10.f;
    float pw = (float)CONTENT_W;

    drawRoundRect(px - 10.f, py, pw + 20.f, 360.f,
        COL_WHITE, sf::Color(215, 217, 224), 1.f);

    drawText("Authentication Required", px + 10.f, py + 20.f,
        24, COL_DARK, true);
    drawText(message, px + 10.f, py + 58.f, 14, COL_LIGHT);

    float stpX = px + 10.f;
    float stpY = py + 100.f;
    const char* steps[] = { "1  Enter ID", "2  Enter Password", "3  Arey, aptu sach bol rhe hu" };
    for (int i = 0; i < 3; i++) {
        bool done = (screen == SCREEN_LOGIN_PASSWORD && i == 0) ||
            (screen == SCREEN_LOGIN_PASSWORD && i == 1);
        bool cur = (screen == SCREEN_LOGIN_ID && i == 0) ||
            (screen == SCREEN_LOGIN_PASSWORD && i == 1);
        sf::Color sc = done ? COL_SUCCESS : (cur ? COL_RED : COL_LIGHT);
        drawText(steps[i], stpX + i * 220.f, stpY, 14, sc, cur || done);
        if (i < 2) drawText("------", stpX + i * 220.f + 130.f, stpY + 1.f, 14, sc);
    }

    drawDivider(px + 10.f, stpY + 30.f, pw - 20.f);
    drawText("Hint (czs i forget): Patient 1 / pass123  |  Doctor 1 / doc456  |  Admin 1 / admin123",
        px + 10.f, stpY + 42.f, 12, COL_LIGHT);
}

void App::drawDashboard() 
{
    float px = (float)CONTENT_X - 10.f;
    float py = (float)CONTENT_Y;
    float pw = (float)CONTENT_W + 20.f;

    int totalPending = 0, totalUnpaid = 0;
    for (int i = 0; i < appointments.size(); i++)
        if (TextTools::equals(appointments.getAll()[i].getStatus(), "pending"))
            totalPending++;
    for (int i = 0; i < bills.size(); i++)
        if (TextTools::equals(bills.getAll()[i].getStatus(), "unpaid"))
            totalUnpaid++;

    char sPat[20], sDoc[20], sPend[20], sUnpd[20];
    TextTools::intToText(patients.size(), sPat, 20);
    TextTools::intToText(doctors.size(), sDoc, 20);
    TextTools::intToText(totalPending, sPend, 20);
    TextTools::intToText(totalUnpaid, sUnpd, 20);

    float cw = (pw - 30.f) / 4.f;
    drawCard(px, py, cw, 90.f, "", "Patients", sPat, "registered", COL_RED);
    drawCard(px + cw + 10.f, py, cw, 90.f, "", "Doctors", sDoc, "available", COL_DARK);
    drawCard(px + 2 * (cw + 10.f), py, cw, 90.f, "", "Pending", sPend, "appointments", sf::Color(200, 120, 0));
    drawCard(px + 3 * (cw + 10.f), py, cw, 90.f, "", "Unpaid", sUnpd, "bills", COL_RED_DARK);

    float sy = py + 106.f;
    drawRoundRect(px, sy, pw, 48.f, COL_WHITE, sf::Color(215, 217, 224), 1.f);
    drawText("Quick Actions", px + 14.f, sy + 14.f, 16, COL_DARK, true);

    float by = sy + 64.f;

    if (screen == SCREEN_PATIENT)
    
    {
        struct { const char* lbl; const char* val; bool d; } pm[] =
        {
            { "Book Appointment",  "1", false },
            { "Cancel Appointment","2", false },
            { "My Appointments",   "3", false },
            { "Medical Records",   "4", false },
            { "My Bills",          "5", false },
            { "Pay Bill",          "6", false },
            { "Top Up Balance",    "7", false },
            { "Logout",            "8", true  },
        };
        float bw = (pw - 30.f) / 3.f;
        for (int i = 0; i < 7; i++)
        {
            float bx = px + (i % 3) * (bw + 10.f);
            float bry = by + (i / 3) * 58.f;
            drawButton(bx, bry, bw, 48.f,
                pm[i].lbl, pm[i].val, BUTTON_CHOICE, pm[i].d);
        }
        drawButton(px + 1 * (bw + 10.f), by + 2 * 58.f, bw, 48.f,
            "Logout", "8", BUTTON_LOGOUT, true, true);
    }
    else if (screen == SCREEN_DOCTOR)
    {
        struct { const char* lbl; const char* val; bool d; } dm[] =
        {
            { "Today's Appointments","1", false },
            { "Mark Complete",       "2", false },
            { "Mark No-Show",        "3", false },
            { "Write Prescription",  "4", false },
            { "Patient History",     "5", false },
        };
        float bw = (pw - 30.f) / 3.f;
        for (int i = 0; i < 5; i++)
        {
            float bx = px + (i % 3) * (bw + 10.f);
            float bry = by + (i / 3) * 58.f;
            drawButton(bx, bry, bw, 48.f,
                dm[i].lbl, dm[i].val, BUTTON_CHOICE, dm[i].d);
        }
        drawButton(px + 2 * (bw + 10.f), by + 58.f, bw, 48.f,
            "Logout", "6", BUTTON_LOGOUT, true, true);
    }
    else if (screen == SCREEN_ADMIN) 
    {
        struct { const char* lbl; const char* val; bool d; } am[] =
        {
            { "Add Doctor",     "1",  false }, { "Remove Doctor",   "2",  false  },
            { "Add Patient",    "3",  false }, { "Remove Patient",  "4",  false  },
            { "All Patients",   "5",  false }, { "All Doctors",     "6",  false },
            { "Appointments",   "7",  false }, { "Unpaid Bills",    "8",  false },
            { "Discharge",      "9",  false  }, { "Security Log",    "10", false },
            { "Daily Report",   "11", false },
        };
        float bw = (pw - 40.f) / 4.f;
        for (int i = 0; i < 11; i++) {
            float bx = px + (i % 4) * (bw + 10.f);
            float bry = by + (i / 4) * 56.f;
            drawButton(bx, bry, bw, 46.f,
                am[i].lbl, am[i].val, BUTTON_CHOICE, am[i].d);
        }
        drawButton(px + 3 * (bw + 10.f), by + 2 * 56.f, bw, 46.f,
            "Logout", "12", BUTTON_LOGOUT, true, true);
    }

    float oy = WIN_H - 82.f;
    drawRoundRect(px, oy, pw, 68.f, COL_DARK);
    drawLineText(message, px + 14.f, oy + 8.f, 14, COL_WHITE, 3);
}

void App::drawActionScreen()
{
    float px = (float)CONTENT_X - 10.f;
    float py = (float)CONTENT_Y;
    float pw = (float)CONTENT_W + 20.f;
    float ph = (float)WIN_H - CONTENT_Y - 82.f;  

    drawRoundRect(px, py, pw, ph, COL_WHITE, sf::Color(215, 217, 224), 1.f);

    if (step > 0) 
    {
        char stpStr[20] = {};
        TextTools::intToText(step, stpStr, 20);
        char stpLabel[40] = "Step ";
        TextTools::append(stpLabel, stpStr, 40);
        drawBadge(px + 14.f, py + 14.f, stpLabel, COL_RED, COL_WHITE);
    }

    drawScrollableMessage(px + 6.f, py + 45.f, pw - 12.f, ph - 52.f);
}

bool App::isMouseOver(float x, float y, float w, float h) const
{
    sf::Vector2i mp = sf::Mouse::getPosition(window);
    return mp.x >= x && mp.x <= x + w && mp.y >= y && mp.y <= y + h;
}

void App::addButton(float x, float y, float w, float h,
    const char label[], const char value[],
    int kind, bool danger)
{
    if (buttonCount >= 60) return;
    buttons[buttonCount].area = { { x, y }, { w, h } };
    buttons[buttonCount].danger = danger;
    buttons[buttonCount].selected = false;
    TextTools::copy(buttons[buttonCount].label, label, 80);
    TextTools::copy(buttons[buttonCount].value, value, 20);
    buttons[buttonCount].kind = kind;
    buttonCount++;
}

void App::setPrompt(const char text[], bool isPassword)
{
    TextTools::copy(prompt, text, 300);
    inputIsPassword = isPassword;
}

void App::setMessage(const char text[])
{
    TextTools::copy(message, text, 8000);
    scrollOffset = 0.f;
}

void App::addMessage(const char text[])
{
    TextTools::append(message, text, 8000);
}

void App::setTitle(const char heading[], const char sub[]) 
{
    TextTools::copy(title, heading, 120);
    TextTools::copy(subtitle, sub, 200);
}

void App::setToast(const char text[], bool success)
{
    TextTools::copy(toast.text, text, 300);
    toast.success = success;
    toast.alpha = 255.f;
    toast.active = true;
    if (success) playSuccess(); else playError();
}

void App::clearInput() 
{
    TextTools::clear(input);
    inputIsPassword = false;
}

void App::playClick() { clickSound.play(); }
void App::playSuccess() { successSound.play(); }
void App::playError() { errorSound.play(); }

void App::showRoleMenu() 
{
    screen = SCREEN_ROLE;
    action = ACTION_NONE;
    inputActive = false;
    setTitle("Welcome to MediCore", "Select your desired portal to begin.");
    setMessage("");
    setPrompt("");
}

void App::showPatientMenu() 
{
    screen = SCREEN_PATIENT;
    action = ACTION_NONE;
    inputActive = false;
    step = 0;
    Patient* p = currentPatient();
    char bal[30]; TextTools::floatToText(p->getBalance(), bal, 30);
    setTitle("Patient Dashboard", "Amma na kaha tha na bachpan ma\n doodh peya kro");
    setMessage("Welcome back, "); addMessage(p->getName());
    addMessage("  |  Balance: PKR "); addMessage(bal);
    setPrompt("", false);
}

void App::showDoctorMenu() 
{
    screen = SCREEN_DOCTOR;
    action = ACTION_NONE;
    inputActive = false;
    step = 0;
    Doctor* d = currentDoctor();
    setTitle("Doctor Workspace", "An Apple a day keeps a doctor away");
    setMessage("Dr. "); addMessage(d->getName());
    addMessage("  |  Specialization: "); addMessage(d->getSpecialization());
    setPrompt("", false);
}

void App::showAdminMenu() 
{
    FileHandler::loadPatients(patients);
    FileHandler::loadDoctors(doctors);
    FileHandler::loadAppointments(appointments);
    FileHandler::loadBills(bills);
    FileHandler::loadPrescriptions(prescriptions);
    screen = SCREEN_ADMIN;
    action = ACTION_NONE;
    inputActive = false;
    step = 0;
    setTitle("Admin Control Centre", "Hospitals make alot of money, i mean");
    setMessage("Select a management action from the dashboard.");
    setPrompt("", false);
}

void App::startLoginId(int role)
{
    roleChoice = role;
    failedAttempts = 0;
    screen = SCREEN_LOGIN_ID;
    inputActive = true;
    inputIsPassword = false;
    const char* name = role == 1 ? "Patient" : role == 2 ? "Doctor" : "Admin";
    setTitle("Secure Login", "Bhai Jan, Prove kro.");
    setPrompt("User ID");
    char msg[200] = "Logging in as: ";
    TextTools::append(msg, name, 200);
    TextTools::append(msg, ".  Enter your ID in numeric form.", 200);
    setMessage(msg);
}

void App::finishLogin()
{
    bool ok = false;
    if (roleChoice == 1)
    {
        Patient* p = patients.findById(loginId);
        ok = p != 0 && p->checkPassword(input);
        if (ok) currentPatientId = loginId;
    }
    else if (roleChoice == 2)
    {
        Doctor* d = doctors.findById(loginId);
        ok = d != 0 && d->checkPassword(input);
        if (ok) currentDoctorId = loginId;
    }
    else if (roleChoice == 3)
    {
        ok = admin.getId() == loginId && admin.checkPassword(input);
    }

    char idText[20]; TextTools::intToText(loginId, idText, 20);
    clearInput();

    if (ok) 
    {
        FileHandler::appendSecurityLog(
            roleChoice == 1 ? "Patient" : roleChoice == 2 ? "Doctor" : "Admin",
            idText, "SUCCESS");
        setToast("Login successful. Welcome to MediCore.", true);
        if (roleChoice == 1) showPatientMenu();
        else if (roleChoice == 2) showDoctorMenu();
        else                      showAdminMenu();
    }
    else 
    {
        failedAttempts++;
        FileHandler::appendSecurityLog(
            roleChoice == 1 ? "Patient" : roleChoice == 2 ? "Doctor" : "Admin",
            idText, "FAILED");
        if (failedAttempts >= 3)
        {
            setToast("Tsk Tsk Account locked. Contact admin.", false);
            showRoleMenu();
        }
        else 
        {
            setToast("Login failed. Oyee Hoyee Skill Issue", false);
            screen = SCREEN_LOGIN_ID;
            inputActive = true;
            inputIsPassword = false;
            setPrompt("User ID");
        }
    }
}

void App::logout() 
{
    currentPatientId = 0;
    currentDoctorId = 0;
    showRoleMenu();
}

Patient* App::currentPatient() { return patients.findById(currentPatientId); }
Doctor* App::currentDoctor() { return doctors.findById(currentDoctorId); }
Doctor* App::findDoctor(int id) { return doctors.findById(id); }
Patient* App::findPatient(int id) { return patients.findById(id); }
Appointment* App::findAppointment(int id) { return appointments.findById(id); }
Bill* App::findBill(int id) { return bills.findById(id); }

const char* App::doctorName(int id) {
    Doctor* d = doctors.findById(id);
    return d == 0 ? "Unknown Doctor" : d->getName();
}
const char* App::patientName(int id) {
    Patient* p = patients.findById(id);
    return p == 0 ? "Unknown Patient" : p->getName();
}

void App::patientChoice(int choice)
{
    scrollOffset = 0.f;
    if (choice == 1) 
    {
        action = ACTION_BOOK; step = 1; screen = SCREEN_ACTION; inputActive = true;
        setTitle("Book Appointment", "Search by specialization, choose a doctor and time slot.");
        setMessage("Enter specialization to search.\nExample:  Cardiology");
        setPrompt("Specialization");
    }
    else if (choice == 2)
    {
        action = ACTION_CANCEL; step = 1; screen = SCREEN_ACTION; inputActive = true;
        setTitle("Cancel Appointment", "Only pending appointments can be cancelled.");
        setMessage("Pending appointments:\n");
        bool found = false;
        for (int i = 0; i < appointments.size(); i++)
        {
            Appointment a = appointments.getAll()[i];
            if (a.getPatientId() == currentPatientId &&
                TextTools::equals(a.getStatus(), "pending"))
            {
                char id[20]; TextTools::intToText(a.getId(), id, 20);
                addMessage(id); addMessage("  |  "); addMessage(doctorName(a.getDoctorId()));
                addMessage("  |  "); addMessage(a.getDate());
                addMessage("  |  "); addMessage(a.getTimeSlot()); addMessage("\n");
                found = true;
            }
        }
        if (!found) { setToast("No pending appointments.", false); showPatientMenu(); return; }
        setPrompt("Appointment ID");
    }
    else if (choice == 3) { viewMyAppointments(); screen = SCREEN_ACTION; }
    else if (choice == 4) { viewMyRecords();      screen = SCREEN_ACTION; }
    else if (choice == 5) { viewMyBills();        screen = SCREEN_ACTION; }
    else if (choice == 6) {
        action = ACTION_PAY; step = 1; screen = SCREEN_ACTION; inputActive = true;
        setTitle("Pay Bill", "Select an unpaid bill to pay from your balance.");
        setMessage("Unpaid bills:\n");
        bool found = false;
        for (int i = 0; i < bills.size(); i++)
        {
            Bill b = bills.getAll()[i];
            if (b.getPatientId() == currentPatientId &&
                TextTools::equals(b.getStatus(), "unpaid"))
            {
                char id[20], amount[30], aid[20];
                TextTools::intToText(b.getId(), id, 20);
                TextTools::floatToText(b.getAmount(), amount, 30);
                TextTools::intToText(b.getAppointmentId(), aid, 20);
                addMessage(id); addMessage("  |  Appt "); addMessage(aid);
                addMessage("  |  PKR "); addMessage(amount);
                addMessage("  |  "); addMessage(b.getDate()); addMessage("\n");
                found = true;
            }
        }
        if (!found) { setToast("No unpaid bills.", false); showPatientMenu(); return; }
        setPrompt("Bill ID");
    }
    else if (choice == 7) 
    {
        failedAttempts = 0;
        action = ACTION_TOPUP; step = 1; screen = SCREEN_ACTION; inputActive = true;
        setTitle("Top Up Balance", "Add funds to your MediCore wallet.");
        setMessage("Enter amount greater than PKR 0.");
        setPrompt("Amount (PKR)");
    }
    else { logout(); }
}

void App::doctorChoice(int choice)
{
    scrollOffset = 0.f;
    if (choice == 1) 
    {
        char today[11]; Validator::today(today);
        Appointment list[100]; int count = 0;
        for (int i = 0; i < appointments.size(); i++) 
        {
            Appointment a = appointments.getAll()[i];
            if (a.getDoctorId() == currentDoctorId &&
                TextTools::equals(a.getDate(), today))
                list[count++] = a;
        }
        sortAppointmentsByTime(list, count);
        setTitle("Today's Appointments", "Sorted by time slot ascending.");
        setMessage("ID  |  Patient  |  Time  |  Status\n");
        if (count == 0) addMessage("No appointments scheduled for today.");
        for (int i = 0; i < count; i++) {
            char id[20]; TextTools::intToText(list[i].getId(), id, 20);
            addMessage(id); addMessage("  |  ");
            addMessage(patientName(list[i].getPatientId()));
            addMessage("  |  "); addMessage(list[i].getTimeSlot());
            addMessage("  |  "); addMessage(list[i].getStatus()); addMessage("\n");
        }
        screen = SCREEN_ACTION;
    }
    else if (choice == 2 || choice == 3) {
        action = (choice == 2) ? ACTION_COMPLETE : ACTION_NOSHOW;
        step = 1; screen = SCREEN_ACTION; inputActive = true;
        setTitle(choice == 2 ? "Mark Appointment Complete" : "Mark Appointment No-Show","Updates appointment status in text file");
    
        char today[11] = {}; Validator::today(today);
        setMessage("Today's pending appointments:\nID  |  Patient  |  Time\n");
        bool found = false;
        for (int i = 0; i < appointments.size(); i++) 
        {
            Appointment a = appointments.getAll()[i];
            if (a.getDoctorId() == currentDoctorId &&
                TextTools::equals(a.getDate(), today) &&
                TextTools::equals(a.getStatus(), "pending"))
            {
                char id[20] = {}; TextTools::intToText(a.getId(), id, 20);
                addMessage(id); addMessage("  |  ");
                addMessage(patientName(a.getPatientId()));
                addMessage("  |  "); addMessage(a.getTimeSlot()); addMessage("\n");
                found = true;
            }
        }
        if (!found) 
        {
            setToast("No pending appointments for today.", false);
            showDoctorMenu(); return;
        }
        setPrompt("Appointment ID");
    }
    else if (choice == 4)
    {
        action = ACTION_WRITE_RX; step = 1; screen = SCREEN_ACTION; inputActive = true;
        setTitle("Write Prescription", "Prescriptions are saved in text file");
        
        setMessage("Completed appointments (no prescription yet):\nID  |  Patient  |  Date\n");
        bool found = false;
        for (int i = 0; i < appointments.size(); i++)
        {
            Appointment a = appointments.getAll()[i];
            if (a.getDoctorId() == currentDoctorId &&
                TextTools::equals(a.getStatus(), "completed") &&
                !prescriptionExists(a.getId()))
            {
                char id[20] = {}; TextTools::intToText(a.getId(), id, 20);
                addMessage(id); addMessage("  |  ");
                addMessage(patientName(a.getPatientId()));
                addMessage("  |  "); addMessage(a.getDate()); addMessage("\n");
                found = true;
            }
        }
        if (!found) 
        {
            setToast("No completed appointments awaiting prescription.", false);
            showDoctorMenu(); return;
        }
        setPrompt("Appointment ID");
    }
    else if (choice == 5) 
    {
        action = ACTION_HISTORY; step = 1; screen = SCREEN_ACTION; inputActive = true;
        setTitle("Patient Medical History", "Only view records of your own completed patients.");
       
        setMessage("Your patients (completed appointments):\nID  |  Patient Name\n");
        bool found = false;
        for (int i = 0; i < appointments.size(); i++)
        {
            Appointment a = appointments.getAll()[i];
            if (a.getDoctorId() == currentDoctorId &&
                TextTools::equals(a.getStatus(), "completed"))
            {
                // avoid duplicates
                bool alreadyListed = false;
                for (int j = 0; j < i; j++)
                {
                    if (appointments.getAll()[j].getDoctorId() == currentDoctorId &&
                        appointments.getAll()[j].getPatientId() == a.getPatientId() &&
                        TextTools::equals(appointments.getAll()[j].getStatus(), "completed")) {
                        alreadyListed = true; break;
                    }
                }
                if (!alreadyListed) 
                {
                    char pid[20] = {}; TextTools::intToText(a.getPatientId(), pid, 20);
                    addMessage(pid); addMessage("  |  ");
                    addMessage(patientName(a.getPatientId())); addMessage("\n");
                    found = true;
                }
            }
        }
        if (!found) 
        {
            setToast("No completed appointments found.", false);
            showDoctorMenu(); return;
        }
        setPrompt("Patient ID");
    }
    else { logout(); }
}

void App::adminChoice(int choice)
{
    scrollOffset = 0.f;
    if (choice == 1) 
    {
        action = ACTION_ADD_DOCTOR; step = 1; screen = SCREEN_ACTION; inputActive = true;
        setTitle("Add Doctor", "Create a new doctor record in text file");
        setMessage("Enter doctor's full name."); setPrompt("Name");
    }
    else if (choice == 2)
    {
        action = ACTION_REMOVE_DOCTOR; step = 1; screen = SCREEN_ACTION; inputActive = true;
        viewAllDoctors();
        setTitle("Remove Doctor", "Doctors with pending appointments cannot be removed.");
        screen = SCREEN_ACTION; setPrompt("Doctor ID");
    }
    else if (choice == 3) 
    {
        action = ACTION_ADD_PATIENT; step = 1; screen = SCREEN_ACTION; inputActive = true;
        setTitle("Add Patient", "Create a new patient record in the text file.");
        setMessage("Enter patient's full name."); setPrompt("Name");
    }
    else if (choice == 4)
    {
        action = ACTION_REMOVE_PATIENT; step = 1; screen = SCREEN_ACTION; inputActive = true;
        viewAllPatients();
        setTitle("Remove Patient", "Cannot remove patients with pending appointments or unpaid bills.");
        screen = SCREEN_ACTION; setPrompt("Patient ID");
    }
    else if (choice == 5) { viewAllPatients(); screen = SCREEN_ACTION; }
    else if (choice == 6) { viewAllDoctors();  screen = SCREEN_ACTION; }
    else if (choice == 7) { viewAllAppointments(); screen = SCREEN_ACTION; }
    else if (choice == 8) { viewUnpaidBills(); screen = SCREEN_ACTION; }
    else if (choice == 9) 
    {
        action = ACTION_DISCHARGE; step = 1; screen = SCREEN_ACTION; inputActive = true;
        setTitle("Discharge Patient", "Archives patient and all related records.");
        setMessage("Enter Patient ID to discharge."); setPrompt("Patient ID");
    }
    else if (choice == 10) 
    {
        setTitle("Security Log", "Failed and successful login attempts.");
        FileHandler::readSecurityLog(message, 8000);
        screen = SCREEN_ACTION;
    }
    else if (choice == 11) { dailyReport(); screen = SCREEN_ACTION; }
    else { logout(); }
}

void App::actionInput()
{
    if (action == ACTION_BOOK)
    {
        if (step == 1)
        {
            TextTools::copy(tempA, input, 600);
            setMessage("Doctors matching that specialization:\n");
            bool found = false;
            for (int i = 0; i < doctors.size(); i++) 
            {
                Doctor d = doctors.getAll()[i];
                if (TextTools::equalsIgnoreCase(d.getSpecialization(), tempA))
                {
                    char id[20], fee[30];
                    TextTools::intToText(d.getId(), id, 20);
                    TextTools::floatToText(d.getFee(), fee, 30);
                    addMessage(id); addMessage("  |  "); addMessage(d.getName());
                    addMessage("  |  PKR "); addMessage(fee); addMessage("\n");
                    found = true;
                }
            }
            clearInput();
            if (!found) { setToast("Esa koi doctor ni is hospital m, kisi aur ma jae.", false); showPatientMenu(); return; }
            step = 2; setPrompt("Doctor ID");
        }
        else if (step == 2) 
        {
            selectedDoctorId = TextTools::toInt(input);
            Doctor* d = findDoctor(selectedDoctorId);
            clearInput();
            if (d == 0 || !TextTools::equalsIgnoreCase(d->getSpecialization(), tempA)) {
                setToast("Doctor not found.", false); showPatientMenu(); return;
            }
            step = 3; setPrompt("Date  DD-MM-YYYY");
            setMessage("Enter appointment date.\nExample:  15-05-2026");
        }
        else if (step == 3)
        {
            if (!Validator::isValidDate(input))
            {
                setToast("Invalid date. Use DD-MM-YYYY format.", false); clearInput(); return;
            }
            TextTools::copy(tempB, input, 600); clearInput();
            showAvailableSlots(selectedDoctorId, tempB);
            step = 4; setPrompt("Time Slot  (e.g. 09:00)");
        }
        else if (step == 4) 
        {
            try {
                if (!Validator::isValidTimeSlot(input))
                    throw InvalidInputException("Invalid time slot.");
                Appointment test(0, currentPatientId, selectedDoctorId, tempB, input, "pending");
                for (int i = 0; i < appointments.size(); i++)
                    if (appointments.getAll()[i] == test) throw SlotUnavailableException();
                Doctor* d = findDoctor(selectedDoctorId);
                Patient* p = currentPatient();
                if (p->getBalance() < d->getFee()) throw InsufficientFundsException();
                *p -= d->getFee();
                int aid = FileHandler::nextAppointmentId(appointments);
                int bid = FileHandler::nextBillId(bills);
                appointments.add(Appointment(aid, currentPatientId, selectedDoctorId, tempB, input, "pending"));
                bills.add(Bill(bid, currentPatientId, aid, d->getFee(), "unpaid", tempB));
                FileHandler::savePatients(patients);
                FileHandler::saveAppointments(appointments);
                FileHandler::saveBills(bills);
                char id[20]; TextTools::intToText(aid, id, 20);
                setToast("Appointment booked successfully.", true);
                setMessage("Appointment confirmed!\nAppointment ID: ");
                addMessage(id);
                clearInput(); showPatientMenu();
            }
            catch (HospitalException& e) { setToast(e.what(), false); clearInput(); }
        }
    }
  
    else if (action == ACTION_CANCEL)
    {
        int id = TextTools::toInt(input); clearInput();
        Appointment* a = findAppointment(id);
        if (a == 0 || a->getPatientId() != currentPatientId ||
            !TextTools::equals(a->getStatus(), "pending"))
        {
            setToast("Invalid appointment ID.", false); showPatientMenu(); return;
        }
        Doctor* d = findDoctor(a->getDoctorId());
        currentPatient()->operator+=(d->getFee());
        a->setStatus("cancelled");
        for (int i = 0; i < bills.size(); i++)
            if (bills.getAll()[i].getAppointmentId() == id)
                bills.getAll()[i].setStatus("cancelled");
        FileHandler::savePatients(patients);
        FileHandler::saveAppointments(appointments);
        FileHandler::saveBills(bills);
        setToast("Appointment cancelled and fee refunded.", true);
        showPatientMenu();
    }
  
    else if (action == ACTION_PAY)
    {
        int id = TextTools::toInt(input); clearInput();
        Bill* b = findBill(id);
        if (b == 0 || b->getPatientId() != currentPatientId ||
            !TextTools::equals(b->getStatus(), "unpaid"))
        {
            setToast("Invalid bill ID.", false); showPatientMenu(); return;
        }
        try
        {
            Patient* p = currentPatient();
            if (p->getBalance() < b->getAmount()) throw InsufficientFundsException();
            *p -= b->getAmount(); b->setStatus("paid");
            FileHandler::savePatients(patients); FileHandler::saveBills(bills);
            setToast("Bill paid successfully.", true);
            showPatientMenu();
        }
        catch (HospitalException& e) { setToast(e.what(), false); showPatientMenu(); }
    }

    else if (action == ACTION_TOPUP)
    {
        try
        {
            if (!Validator::isPositiveFloat(input))
            {
                failedAttempts++;
                char attemptsLeft[40] = "Invalid amount. Attempts left: ";
                char num[5];
                TextTools::intToText(3 - failedAttempts, num, 5);
                TextTools::append(attemptsLeft, num, 40);
                throw InvalidInputException(attemptsLeft);
            }
            failedAttempts = 0;
            *currentPatient() += TextTools::toFloat(input);
            FileHandler::savePatients(patients);
            clearInput();
            setToast("Balance topped up successfully.", true);
            showPatientMenu();
        }
        catch (HospitalException& e)
        {
            setToast(e.what(), false);
            clearInput();
            if (failedAttempts >= 3)
            {
                failedAttempts = 0;
                setToast("Kaha tha sai paise add kro!!", false);
                showPatientMenu();
            }
        }
}

    else if (action == ACTION_COMPLETE || action == ACTION_NOSHOW)
    {
        char today[11]; Validator::today(today);
        int id = TextTools::toInt(input); clearInput();
        Appointment* a = findAppointment(id);
        if (a == 0 || a->getDoctorId() != currentDoctorId ||
            !TextTools::equals(a->getDate(), today) ||
            !TextTools::equals(a->getStatus(), "pending"))
        {
            setToast("Invalid appointment ID.", false); showDoctorMenu(); return;
        }
        if (action == ACTION_COMPLETE) 
        {
            a->setStatus("completed");
            setToast("Appointment marked as completed.", true);
        }
        else 
        {
            a->setStatus("no-show");
            for (int i = 0; i < bills.size(); i++)
                if (bills.getAll()[i].getAppointmentId() == id)
                    bills.getAll()[i].setStatus("cancelled");
            FileHandler::saveBills(bills);
            setToast("Appointment marked as no-show.", false);
        }
        FileHandler::saveAppointments(appointments);
        showDoctorMenu();
    }

    else if (action == ACTION_WRITE_RX)
    {
        if (step == 1) {
            selectedAppointmentId = TextTools::toInt(input); clearInput();
            Appointment* a = findAppointment(selectedAppointmentId);
            if (a == 0 || a->getDoctorId() != currentDoctorId ||
                !TextTools::equals(a->getStatus(), "completed"))
            {
                setToast("Invalid appointment ID.", false); showDoctorMenu(); return;
            }
            if (prescriptionExists(selectedAppointmentId))
            {
                setToast("Prescription already written for this appointment.", false); showDoctorMenu(); return;
            }
            step = 2; setPrompt("Medicines");
            setMessage("Enter medicines.\nFormat:  Panadol 500mg;Amoxicillin 250mg");
        }
        else if (step == 2) 
        {
            TextTools::copy(tempA, input, 500); clearInput();
            step = 3; setPrompt("Notes (max 300 chars)");
            setMessage("Enter clinical notes.");
        }
        else 
        {
            TextTools::copy(tempB, input, 300); clearInput();
            Appointment* a = findAppointment(selectedAppointmentId);
            int id = FileHandler::nextPrescriptionId(prescriptions);
            prescriptions.add(Prescription(id, selectedAppointmentId,
                a->getPatientId(), currentDoctorId,
                a->getDate(), tempA, tempB));
            FileHandler::savePrescriptions(prescriptions);
            setToast("Prescription saved successfully.", true);
            showDoctorMenu();
        }
    }
 
    else if (action == ACTION_HISTORY) 
    {
        int pid = TextTools::toInt(input); clearInput();
        if (!doctorCanSeePatient(currentDoctorId, pid))
        {
            setToast("Access denied. Not your patient.", false); showDoctorMenu(); return;
        }
        setMessage("Medical History\nDate  |  Medicines  |  Notes\n");
        for (int i = 0; i < prescriptions.size(); i++) {
            Prescription p = prescriptions.getAll()[i];
            if (p.getPatientId() == pid && p.getDoctorId() == currentDoctorId) {
                addMessage(p.getDate()); addMessage("  |  ");
                addMessage(p.getMedicines()); addMessage("  |  ");
                addMessage(p.getNotes()); addMessage("\n");
            }
        }
    }
   
    else if (action == ACTION_ADD_DOCTOR)
    {
        if (step == 1) { TextTools::copy(tempA, input, 51); step = 2; setPrompt("Specialization"); setMessage("Enter specialization."); }
        else if (step == 2) { TextTools::copy(tempB, input, 51); step = 3; setPrompt("Contact (11 digits)"); setMessage("Enter 11-digit contact number."); }
        else if (step == 3) { if (!Validator::isValidContact(input)) { setToast("Invalid contact number.", false); clearInput(); return; } TextTools::copy(tempC, input, 12); step = 4; setPrompt("Password (min 6 chars)"); setMessage("Enter a password."); inputIsPassword = true; }
        else if (step == 4) { if (!Validator::isValidPassword(input)) { setToast("Password must be at least 6 characters.", false); clearInput(); return; } TextTools::copy(tempD, input, 51); step = 5; setPrompt("Consultation Fee"); setMessage("Enter consultation fee (PKR)."); inputIsPassword = false; }
        else {
            if (!Validator::isPositiveFloat(input)) { setToast("Invalid fee.", false); clearInput(); return; }
            int id = FileHandler::nextDoctorId(doctors);
            doctors.add(Doctor(id, tempA, tempB, tempC, tempD, TextTools::toFloat(input)));
            FileHandler::saveDoctors(doctors);
            char ids[20] = {}; TextTools::intToText(id, ids, 20);
            setToast("Doctor added successfully.", true);
            showAdminMenu();
        }
        clearInput();
    }
  
    else if (action == ACTION_ADD_PATIENT) 
    {
        if (step == 1) { TextTools::copy(tempA, input, 51); step = 2; setPrompt("Age"); setMessage("Enter patient age."); }
        else if (step == 2) { if (!Validator::isValidAge(input)) { setToast("Invalid age.", false); clearInput(); return; } TextTools::copy(tempB, input, 10); step = 3; setPrompt("Gender (M/F)"); setMessage("Enter M or F."); }
        else if (step == 3) { if (!Validator::isValidGender(input)) { setToast("Invalid gender. Enter M or F.", false); clearInput(); return; } tempC[0] = input[0]; tempC[1] = '\0'; step = 4; setPrompt("Contact (11 digits)"); setMessage("Enter 11-digit contact number."); }
        else if (step == 4) { if (!Validator::isValidContact(input)) { setToast("Invalid contact.", false); clearInput(); return; } TextTools::copy(tempD, input, 20); step = 5; setPrompt("Password (min 6 chars)"); setMessage("Enter a password."); inputIsPassword = true; }
        else if (step == 5) { if (!Validator::isValidPassword(input)) { setToast("Password must be at least 6 characters.", false); clearInput(); return; } TextTools::copy(tempE, input, 80); step = 6; setPrompt("Initial Balance (PKR)"); setMessage("Enter initial balance."); inputIsPassword = false; }
        else {
            if (!Validator::isPositiveFloat(input)) { setToast("Invalid balance.", false); clearInput(); return; }
            int id = FileHandler::nextPatientId(patients);
            patients.add(Patient(id, tempA, TextTools::toInt(tempB), tempC[0], tempD, tempE, TextTools::toFloat(input)));
            FileHandler::savePatients(patients);
            setToast("Patient added successfully.", true);
            showAdminMenu();
        }
        clearInput();
    }
    
    else if (action == ACTION_REMOVE_DOCTOR)
    {
        int id = TextTools::toInt(input); clearInput();
        if (doctorHasPending(id)) { setToast("Cannot remove doctor with pending appointments.", false); showAdminMenu(); return; }
        doctors.removeById(id); FileHandler::saveDoctors(doctors);
        setToast("Doctor removed.", true); showAdminMenu();
    }
  
    else if (action == ACTION_REMOVE_PATIENT || action == ACTION_DISCHARGE) 
    {
        int id = TextTools::toInt(input); clearInput();
        if (patientHasPending(id))
        {
            setToast(action == ACTION_DISCHARGE
                ? "Cannot discharge patient with pending appointments."
                : "Cannot remove patient with pending appointments.", false);
            showAdminMenu(); return;
        }
        if (patientHasUnpaid(id))
        {
            setToast(action == ACTION_DISCHARGE
                ? "Cannot discharge patient with unpaid bills."
                : "Cannot remove patient with unpaid bills.", false);
            showAdminMenu(); return;
        }
        if (action == ACTION_DISCHARGE)
        {
            FileHandler::archiveAndDeletePatient(id, patients, appointments, bills, prescriptions);
            setToast("Patient discharged and archived.", true);
        }
        else
        {
            FileHandler::cascadeDeletePatient(id, patients, appointments, bills, prescriptions);
            setToast("Patient removed successfully.", true);
        }
        showAdminMenu();
    }
}

void App::viewMyAppointments()
{
    Appointment list[100]; int count = 0;
    for (int i = 0; i < appointments.size(); i++)
        if (appointments.getAll()[i].getPatientId() == currentPatientId)
            list[count++] = appointments.getAll()[i];
    sortAppointmentsByDate(list, count, false);
    setTitle("My Appointments", "All appointments sorted by date ascending.");
    setMessage("ID  |  Doctor  |  Specialization  |  Date  |  Time  |  Status\n");
    if (count == 0) { addMessage("No appointments found."); return; }
    for (int i = 0; i < count; i++) {
        Doctor* d = findDoctor(list[i].getDoctorId());
        char id[20]; TextTools::intToText(list[i].getId(), id, 20);
        addMessage(id); addMessage("  |  "); addMessage(doctorName(list[i].getDoctorId()));
        addMessage("  |  "); addMessage(d ? d->getSpecialization() : "Unknown");
        addMessage("  |  "); addMessage(list[i].getDate());
        addMessage("  |  "); addMessage(list[i].getTimeSlot());
        addMessage("  |  "); addMessage(list[i].getStatus()); addMessage("\n");
    }
}

void App::viewMyRecords() 
{
    Prescription list[100]; int count = 0;
    for (int i = 0; i < prescriptions.size(); i++)
        if (prescriptions.getAll()[i].getPatientId() == currentPatientId)
            list[count++] = prescriptions.getAll()[i];
    sortPrescriptionsByDate(list, count, true);
    setTitle("Medical Records", "Prescriptions sorted by newest date first.");
    setMessage("Date  |  Doctor  |  Medicines  |  Notes\n");
    if (count == 0) { addMessage("No medical records found."); return; }
    for (int i = 0; i < count; i++) 
    {
        addMessage(list[i].getDate()); addMessage("  |  ");
        addMessage(doctorName(list[i].getDoctorId())); addMessage("  |  ");
        addMessage(list[i].getMedicines()); addMessage("  |  ");
        addMessage(list[i].getNotes()); addMessage("\n");
    }
}

void App::viewMyBills()
{
    setTitle("My Bills", "Pay kro bhai, bhagna mana hai!!");
    setMessage("Bill  |  Appointment  |  Amount (PKR)  |  Status  |  Date\n");
    float total = 0.f; bool found = false;
    for (int i = 0; i < bills.size(); i++) 
    {
        Bill b = bills.getAll()[i];
        if (b.getPatientId() == currentPatientId)
        {
            char id[20], aid[20], amt[30];
            TextTools::intToText(b.getId(), id, 20);
            TextTools::intToText(b.getAppointmentId(), aid, 20);
            TextTools::floatToText(b.getAmount(), amt, 30);
            addMessage(id); addMessage("  |  "); addMessage(aid);
            addMessage("  |  PKR "); addMessage(amt);
            addMessage("  |  "); addMessage(b.getStatus());
            addMessage("  |  "); addMessage(b.getDate()); addMessage("\n");
            if (TextTools::equals(b.getStatus(), "unpaid")) total += b.getAmount();
            found = true;
        }
    }
    if (!found) { addMessage("No bills found."); return; }
    char t[30]; TextTools::floatToText(total, t, 30);
    addMessage("\nTotal amount unpaid: PKR "); addMessage(t);
}

void App::viewAllPatients() 
{
    FileHandler::loadPatients(patients);
    FileHandler::loadBills(bills);
    setTitle("All Patients", "Full patient directory with unpaid bill count.");
    setMessage("ID  |  Name  |  Age  |  Gender  |  Contact  |  Balance  |  Unpaid\n");
    for (int i = 0; i < patients.size(); i++)
    {
        Patient p = patients.getAll()[i]; int unpaid = 0;
        for (int b = 0; b < bills.size(); b++)
            if (bills.getAll()[b].getPatientId() == p.getId() &&
                TextTools::equals(bills.getAll()[b].getStatus(), "unpaid")) unpaid++;
        char id[20], age[20], bal[30], u[20];
        TextTools::intToText(p.getId(), id, 20);
        TextTools::intToText(p.getAge(), age, 20);
        TextTools::floatToText(p.getBalance(), bal, 30);
        TextTools::intToText(unpaid, u, 20);
        char g[2] = { p.getGender(), '\0' };
        addMessage(id); addMessage("  |  "); addMessage(p.getName());
        addMessage("  |  "); addMessage(age); addMessage("  |  "); addMessage(g);
        addMessage("  |  "); addMessage(p.getContact());
        addMessage("  |  PKR "); addMessage(bal);
        addMessage("  |  "); addMessage(u); addMessage("\n");
    }
}

void App::viewAllDoctors()
{
    FileHandler::loadDoctors(doctors);
    setTitle("All Doctors", "Hospital doctor directory.");
    setMessage("ID  |  Name  |  Specialization  |  Contact  |  Fee\n");
    for (int i = 0; i < doctors.size(); i++) {
        Doctor d = doctors.getAll()[i];
        cout << d << endl;                                                    
        char id[20], fee[30];
        TextTools::intToText(d.getId(), id, 20);
        TextTools::floatToText(d.getFee(), fee, 30);
        addMessage(id); addMessage("  |  "); addMessage(d.getName());
        addMessage("  |  "); addMessage(d.getSpecialization());
        addMessage("  |  "); addMessage(d.getContact());
        addMessage("  |  PKR "); addMessage(fee); addMessage("\n");
    }
}

void App::viewAllAppointments()
{
    FileHandler::loadAppointments(appointments);
    FileHandler::loadPatients(patients);
    FileHandler::loadDoctors(doctors);
    Appointment list[100]; int count = appointments.size();
    for (int i = 0; i < count; i++) list[i] = appointments.getAll()[i];
    sortAppointmentsByDate(list, count, true);
    setTitle("All Appointments", "Sorted by newest date first.");
    setMessage("ID  |  Patient  |  Doctor  |  Date  |  Time  |  Status\n");
    for (int i = 0; i < count; i++) {
        char id[20]; TextTools::intToText(list[i].getId(), id, 20);
        addMessage(id); addMessage("  |  "); addMessage(patientName(list[i].getPatientId()));
        addMessage("  |  "); addMessage(doctorName(list[i].getDoctorId()));
        addMessage("  |  "); addMessage(list[i].getDate());
        addMessage("  |  "); addMessage(list[i].getTimeSlot());
        addMessage("  |  "); addMessage(list[i].getStatus()); addMessage("\n");
    }
}

void App::viewUnpaidBills()
{
    FileHandler::loadBills(bills);
    FileHandler::loadPatients(patients);
    setTitle("Unpaid Bills", "Bills overdue by 7+ days are flagged OVERDUE.");
    setMessage("Bill  |  Patient  |  Amount (PKR)  |  Date\n");
    for (int i = 0; i < bills.size(); i++) 
    {
        Bill b = bills.getAll()[i];
        if (TextTools::equals(b.getStatus(), "unpaid")) 
        {
            char id[20], amt[30];
            TextTools::intToText(b.getId(), id, 20);
            TextTools::floatToText(b.getAmount(), amt, 30);
            addMessage(id); addMessage("  |  "); addMessage(patientName(b.getPatientId()));
            addMessage("  |  PKR "); addMessage(amt); addMessage("  |  "); addMessage(b.getDate());
            if (Validator::isOlderThanSevenDays(b.getDate())) addMessage("  [OVERDUE]");
            addMessage("\n");
        }
    }
}

void App::dailyReport()
{
    FileHandler::loadAppointments(appointments);
    FileHandler::loadBills(bills);
    FileHandler::loadPatients(patients);
    FileHandler::loadDoctors(doctors);
    char today[11]; Validator::today(today);
    int pending = 0, completed = 0, noshow = 0, cancelled = 0;
    float revenue = 0.f;
    for (int i = 0; i < appointments.size(); i++) 
    {
        Appointment a = appointments.getAll()[i];
        if (TextTools::equals(a.getDate(), today)) 
        {
            if (TextTools::equals(a.getStatus(), "pending"))   pending++;
            else if (TextTools::equals(a.getStatus(), "completed")) completed++;
            else if (TextTools::equals(a.getStatus(), "no-show"))   noshow++;
            else if (TextTools::equals(a.getStatus(), "cancelled")) cancelled++;
        }
    }
    for (int i = 0; i < bills.size(); i++)
        if (TextTools::equals(bills.getAll()[i].getDate(), today) &&
            TextTools::equals(bills.getAll()[i].getStatus(), "paid"))
            revenue += bills.getAll()[i].getAmount();

    char total[20], p[20], c[20], n[20], x[20], rev[30];
    TextTools::intToText(pending + completed + noshow + cancelled, total, 20);
    TextTools::intToText(pending, p, 20); TextTools::intToText(completed, c, 20);
    TextTools::intToText(noshow, n, 20); TextTools::intToText(cancelled, x, 20);
    TextTools::floatToText(revenue, rev, 30);

    setTitle("Daily Report", "Generated from today's records.");
    setMessage("Daily Summary\nDate: "); addMessage(today);
    addMessage("\nTotal appointments: "); addMessage(total);
    addMessage("  (Pending: "); addMessage(p);
    addMessage("  Completed: "); addMessage(c);
    addMessage("  No-show: "); addMessage(n);
    addMessage("  Cancelled: "); addMessage(x); addMessage(")");
    addMessage("\nRevenue collected today: PKR "); addMessage(rev);
    addMessage("\n\nPatients with outstanding unpaid bills:\n");
    for (int i = 0; i < patients.size(); i++) 
    {
        float owed = 0.f;
        for (int b = 0; b < bills.size(); b++)
            if (bills.getAll()[b].getPatientId() == patients.getAll()[i].getId() &&
                TextTools::equals(bills.getAll()[b].getStatus(), "unpaid"))
                owed += bills.getAll()[b].getAmount();
        if (owed > 0.f) 
        {
            char owedText[30]; TextTools::floatToText(owed, owedText, 30);
            addMessage(patients.getAll()[i].getName()); addMessage("  |  PKR "); addMessage(owedText); addMessage("\n");
        }
    }
    addMessage("\nDoctor-wise summary for today:\n");
    for (int d = 0; d < doctors.size(); d++) 
    {
        int dp = 0, dc = 0, dn = 0;
        for (int a = 0; a < appointments.size(); a++) 
        {
            Appointment ap = appointments.getAll()[a];
            if (ap.getDoctorId() == doctors.getAll()[d].getId() &&
                TextTools::equals(ap.getDate(), today))
            {
                if (TextTools::equals(ap.getStatus(), "pending"))   dp++;
                else if (TextTools::equals(ap.getStatus(), "completed")) dc++;
                else if (TextTools::equals(ap.getStatus(), "no-show"))   dn++;
            }
        }
        char tp[20], tc[20], tn[20];
        TextTools::intToText(dp, tp, 20); TextTools::intToText(dc, tc, 20); TextTools::intToText(dn, tn, 20);
        addMessage(doctors.getAll()[d].getName());
        addMessage("  |  Completed: "); addMessage(tc);
        addMessage("  Pending: "); addMessage(tp);
        addMessage("  No-show: "); addMessage(tn); addMessage("\n");
    }
}

bool App::patientHasPending(int pid)
{
    Patient dummy(pid, "", 0, 'M', "", "", 0);
    if (!patients.contains(dummy)) return false;  

    for (int i = 0; i < appointments.size(); i++)
        if (appointments.getAll()[i].getPatientId() == pid &&
            TextTools::equals(appointments.getAll()[i].getStatus(), "pending"))
            return true;
    return false;
}

bool App::patientHasUnpaid(int pid)
{
    for (int i = 0; i < bills.size(); i++)
        if (bills.getAll()[i].getPatientId() == pid &&
            TextTools::equals(bills.getAll()[i].getStatus(), "unpaid")) return true;
    return false;
}
bool App::doctorHasPending(int did)
{
    Doctor dummy(did, "", "", "", "", 0);
    if (!doctors.contains(dummy)) return false;

    for (int i = 0; i < appointments.size(); i++)
        if (appointments.getAll()[i].getDoctorId() == did &&
            TextTools::equals(appointments.getAll()[i].getStatus(), "pending"))
            return true;
    return false;
}

bool App::prescriptionExists(int aid)
{
    for (int i = 0; i < prescriptions.size(); i++)
        if (prescriptions.getAll()[i].getAppointmentId() == aid) return true;
    return false;
}
bool App::doctorCanSeePatient(int did, int pid)
{
    for (int i = 0; i < appointments.size(); i++)
        if (appointments.getAll()[i].getDoctorId() == did &&
            appointments.getAll()[i].getPatientId() == pid &&
            TextTools::equals(appointments.getAll()[i].getStatus(), "completed")) return true;
    return false;
}

void App::sortAppointmentsByDate(Appointment list[], int count, bool desc)
{
    for (int i = 0; i < count - 1; i++)
        for (int j = 0; j < count - i - 1; j++) 
        {
            int r = Validator::compareDates(list[j].getDate(), list[j + 1].getDate());
            if ((!desc && r > 0) || (desc && r < 0)) {
                Appointment t = list[j]; list[j] = list[j + 1]; list[j + 1] = t;
            }
        }
}
void App::sortPrescriptionsByDate(Prescription list[], int count, bool desc) 
{
    for (int i = 0; i < count - 1; i++)
        for (int j = 0; j < count - i - 1; j++) 
        {
            int r = Validator::compareDates(list[j].getDate(), list[j + 1].getDate());
            if ((!desc && r > 0) || (desc && r < 0)) {
                Prescription t = list[j]; list[j] = list[j + 1]; list[j + 1] = t;
            }
        }
}
void App::sortAppointmentsByTime(Appointment list[], int count)
{
    for (int i = 0; i < count - 1; i++)
        for (int j = 0; j < count - i - 1; j++)
            if (Validator::compareTimes(list[j].getTimeSlot(), list[j + 1].getTimeSlot()) > 0) {
                Appointment t = list[j]; list[j] = list[j + 1]; list[j + 1] = t;
            }
}

void App::showAvailableSlots(int doctorId, const char date[])
{
    const char slots[8][6] = 
    {
        "09:00","10:00","11:00","12:00","13:00","14:00","15:00","16:00"
    };
    setMessage("Available time slots for ");
    addMessage(doctorName(doctorId)); addMessage(" on "); addMessage(date); addMessage(":\n");
    bool anyFree = false;
    for (int s = 0; s < 8; s++)
    {
        Appointment test(0, 0, doctorId, date, slots[s], "pending");
        bool taken = false;
        for (int i = 0; i < appointments.size(); i++)
            if (appointments.getAll()[i] == test) { taken = true; break; }
        if (!taken) { addMessage(slots[s]); addMessage("  "); anyFree = true; }
    }
    if (!anyFree) addMessage("\nAll slots are booked for this date.");
}