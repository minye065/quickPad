#include QMK_KEYBOARD_H
#include "raw_hid.h"

#define EVENT_KEY 0x01
#define EVENT_KNOB 0x02
#define EVENT_STATE 0x03
#define REPORT_SIZE 32

#define CODE_TL 0
#define CODE_TM 1
#define CODE_TR 2
#define CODE_ML 3
#define CODE_MM 4
#define CODE_MR 5
#define CODE_BL 6
#define CODE_BM 7
#define CODE_BR 8

#define CODE_KNOB_CCW 0
#define CODE_KNOB_CW 1

enum custom_keycodes
{
    tl = SAFE_RANGE,
    tm,
    tr,
    ml,
    mm,
    mr,
    bl,
    bm,
    br
};

enum states
{
    stateBase,
    stateFile,
    stateClock,
};

enum codes
{
    temporary,
    emptyRecycleBin,
    openZenWindows,
    startWorkTimer,
    startRestTimer,
    endTimers
};

static uint8_t currentState = stateBase;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] =
{
    /*
     * ┌───┬───┬───┐
     * │ 1 │ 2 │ 3 │
     * ├───┼───┼───┤
     * │ 4 │ 5 │ 6 │
     * ├───┼───┼───┤
     * │ 7 │ 8 │ 9 │
     * └───┴───┴───┘
     */
    [0] = LAYOUT
    (
        tl,   tm,   tr,
        ml,   mm,   mr,
        bl,   bm,   br
    )
};

static void sendEvent(uint8_t eventType, uint8_t value)
{
    uint8_t data[REPORT_SIZE] = {0};
    data[0] = eventType;
    data[1] = value;
    raw_hid_send(data, REPORT_SIZE);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record)
{
    if (!record->event.pressed)
    {
        return true;
    }
    switch (keycode)
    {
        case tl:
            SEND_STRING(SS_LGUI("r"));
            wait_ms(150);
            SEND_STRING("C:\\Users\\beest\\AppData\\Local\\Python\\bin\\pythonw.exe C:\\dev\\quickPad\\App\\main.py" SS_TAP(X_ENTER));
            return false;
        case tm:
            switch(currentState)
            {
                case stateBase: sendEvent(EVENT_KEY, temporary); return false; break;
                case stateFile: sendEvent(EVENT_KEY, emptyRecycleBin); return false; break;
                case stateClock: sendEvent(EVENT_KEY, startWorkTimer); return false; break;
            }
        case tr:
            switch(currentState)
            {
                case stateBase: sendEvent(EVENT_KEY, temporary); return false; break;
                case stateFile: sendEvent(EVENT_KEY, openZenWindows); return false; break;
                case stateClock: sendEvent(EVENT_KEY, startRestTimer); return false; break;
            }
        case ml:
            switch(currentState)
            {
                case stateBase: sendEvent(EVENT_KEY, temporary); return false; break;
                case stateFile: sendEvent(EVENT_KEY, temporary); return false; break;
                case stateClock: sendEvent(EVENT_KEY, endTimers); return false; break;
            }
        case mm:
            switch(currentState)
            {
                case stateBase: sendEvent(EVENT_KEY, temporary); return false; break;
                case stateFile: sendEvent(EVENT_KEY, temporary); return false; break;
                case stateClock: sendEvent(EVENT_KEY, temporary); return false; break;
            }
        case mr:
            switch(currentState)
            {
                case stateBase: sendEvent(EVENT_KEY, temporary); return false; break;
                case stateFile: sendEvent(EVENT_KEY, temporary); return false; break;
                case stateClock: sendEvent(EVENT_KEY, temporary); return false; break;
            }
        case bl:
            switch(currentState)
            {
                case stateBase: sendEvent(EVENT_KEY, temporary); return false; break;
                case stateFile: sendEvent(EVENT_KEY, temporary); return false; break;
                case stateClock: sendEvent(EVENT_KEY, temporary); return false; break;
            }
        case bm:
            switch(currentState)
            {
                case stateBase: sendEvent(EVENT_KEY, temporary); return false; break;
                case stateFile: sendEvent(EVENT_KEY, temporary); return false; break;
                case stateClock: sendEvent(EVENT_KEY, temporary); return false; break;
            }
        case br:
            switch(currentState)
            {
                case stateBase: sendEvent(EVENT_KEY, temporary); return false; break;
                case stateFile: sendEvent(EVENT_KEY, temporary); return false; break;
                case stateClock: sendEvent(EVENT_KEY, temporary); return false; break;
            }
    }
    return true;
}

bool encoder_update_user(uint8_t index, bool clockwise)
{
    if (clockwise)
    {
        currentState = (currentState + 1) % 3;
    }
    else
    {
        currentState = (currentState + 2) % 3;
    }
    sendEvent(EVENT_STATE, currentState);
    return false;
}
