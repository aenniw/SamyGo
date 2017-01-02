#ifndef __BUTTONS_H
#define __BUTTONS_H
// standard button codes
enum {
    KEY_SOURCE = 0x01,
    KEY_POWER,
    KEY_NUM1 = 0x04,
    KEY_NUM2,
    KEY_NUM3,
    KEY_VOL_PLUS,
    KEY_NUM4,
    KEY_NUM5,
    KEY_NUM6,
    KEY_VOL_MINUS,
    KEY_NUM7,
    KEY_NUM8,
    KEY_NUM9,
    KEY_MUTE,
    KEY_P_DOWN,
    KEY_NUM0,
    KEY_P_UP,
    KEY_PRE_CH,
    KEY_COL_GREEN,
    KEY_COL_YELLOW,
    KEY_COL_BLUE,
    KEY_MENU = 0x1A,
    KEY_TV,
    KEY_INFO = 0x1F,
    KEY_SUBT = 0x25,
    KEY_TTX_MIX = 0x2C,
    KEY_EXIT,
    KEY_FAV_CH = 0x44,
    KEY_REWIND,
    KEY_STOP,
    KEY_PLAY,
    KEY_FORWARD,
    KEY_RECORD,
    KEY_PAUSE,
    KEY_TOOLS,
    KEY_GUIDE = 0x4F,
    KEY_RETURN = 0x58,
    KEY_CUR_UP = 0x60,
    KEY_CUR_DOWN,
    KEY_CUR_RIGHT,
    KEY_CUR_LEFT = 0x65,
    KEY_ENTER = 0x68,
    KEY_CH_LIST = 0x6B,
    KEY_COL_RED,
    KEY_CONTENT = 0x79,
    KEY_MEDIA_P = 0x8C,
    KEY_INTERNET = 0x93
};
// bezel touch-button codes
enum {
    KEY_BEZEL_SOURCE = 0x101, KEY_BEZEL_POWER,
    KEY_BEZEL_VOL_UP = 0x107,
    KEY_BEZEL_VOL_DOWN = 0x10B,
    KEY_BEZEL_P_DOWN = 0x110,
    KEY_BEZEL_P_UP = 0x112
};
// virtual button codes
enum {
    KEY_SAP = 0x00,
    KEY_SLEEP = 0x03,
    KEY_PIP = 0x20,
    KEY_PICMODE = 0x28,
    KEY_SOUND_MODE = 0x2B,
    KEY_AIR_CABLE = 0x36,
    KEY_ASPECT = 0x3E,
    KEY_DTV = 0x43,
    KEY_INTERNET2 = 0x53,
    KEY_VGA = 0x69,
    KEY_SRS = 0x6E,
    KEY_ESAVING = 0x77,
    KEY_EXT1 = 0x84,
    KEY_COMPONENT = 0x86,
    KEY_HDMI_TOGGLE = 0x8B,
    KEY_OFF = 0x98,
    KEY_PIC_DYNAMIC = 0xBD, KEY_HDMI2,
    KEY_HDMI3 = 0xC2,
    KEY_HDMI4 = 0xC5,
    KEY_PIC_MOVIE = 0xDE, KEY_PIC_STANDARD,
    KEY_ASPECT43 = 0xE3, KEY_ASPECT169,
    KEY_HDMI1 = 0xE9,
    KEY_EXT2 = 0xEB, KEY_CVBS
};
// virtual button codes with unknown functions
enum {
    KEY_SERVICE = 0x3B,
    KEY_DIAG1 = 0xC0, KEY_DIAG2,
    KEY_DISP_VCR = 0x7A, KEY_DISP_CABLE,
    KEY_DISP_TV = 0x7D, KEY_DISP_DVD, KEY_DISP_STB
};
// no-function button code
#define KEY_NOTHING    0xFF    // or 0x1C, also good

#endif