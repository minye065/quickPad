#include QMK_KEYBOARD_H
#include "raw_hid.h"

#define REPORT_SIZE 32

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
    stateCount
};

enum actions
{
    actTemporary,
    actEmptyRecycleBin,
    actOpenZenWindows,
    actStartWorkTimer,
    actStartRestTimer,
    actEndTimers,
    actStateBase,
    actStateFile,
    actStateClock
};

static uint8_t currentState = stateBase;

static const uint8_t actionMap[stateCount][9] =
{
    {actTemporary, actTemporary, actTemporary, actTemporary, actTemporary, actTemporary, actTemporary, actTemporary, actTemporary},
    {actTemporary, actEmptyRecycleBin, actOpenZenWindows, actTemporary, actTemporary, actTemporary, actTemporary, actTemporary, actTemporary},
    {actTemporary, actStartWorkTimer, actStartRestTimer, actEndTimers, actTemporary, actTemporary, actTemporary, actTemporary, actTemporary}
};

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
        bl,   bm,   br,
    )
};

static void sendCode(uint8_t code)
{
    uint8_t data[REPORT_SIZE] = {0};
    data[0] = code;
    raw_hid_send(data, REPORT_SIZE);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record)
{
    if (keycode >= tl && keycode <= br)
    {
        if (record->event.pressed)
        {
            sendCode(actionMap[currentState][keycode - tl]);
        }
        return false;
    }
    return true;
}

bool encoder_update_user(uint8_t index, bool clockwise)
{
    if (clockwise)
    {
        currentState = (currentState + 1) % stateCount;
    }
    else
    {
        currentState = (currentState + stateCount - 1) % stateCount;
    }
    sendCode(actStateBase + currentState);
    return false;
}
