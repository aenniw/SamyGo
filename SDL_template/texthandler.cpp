#include "texthandler.h"

int TextHandler::m_line;
int TextHandler::m_collum;
std::vector<std::vector<SDL_Surface *> >TextHandler::m_surfaces;
std::vector<std::vector<SDL_Rect *> >TextHandler::m_possitions;

TextHandler &TextHandler::operator<<(const std::string s) {
    writeOnScreen(s);
    return *this;
}

TextHandler &TextHandler::operator<<(const SDLKey s) {
    m_convertStream << s;
    writeOnScreen(m_convertStream.str());
    m_convertStream.str("");
    return *this;
}

TextHandler &TextHandler::operator<<(const int s) {
    m_convertStream << s;
    writeOnScreen(m_convertStream.str());
    m_convertStream.str("");
    return *this;
}

TextHandler &TextHandler::operator<<(const unsigned int s) {
    m_convertStream << s;
    writeOnScreen(m_convertStream.str());
    m_convertStream.str("");
    return *this;
}

TextHandler &TextHandler::operator<<(const char s) {
    m_convertStream << s;
    writeOnScreen(m_convertStream.str());
    m_convertStream.str("");
    return *this;
}

TextHandler &TextHandler::operator<<(const float s) {
    m_convertStream << s;
    writeOnScreen(m_convertStream.str());
    m_convertStream.str("");
    return *this;
}

TextHandler &TextHandler::operator<<(const double s) {
    m_convertStream << s;
    writeOnScreen(m_convertStream.str());
    m_convertStream.str("");
    return *this;
}
