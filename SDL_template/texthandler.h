#ifndef TEXTHANDLER_H
#define TEXTHANDLER_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <string>
#include <sstream>
#include <list>
#include <vector>

class TextHandler {
private:
    TTF_Font *m_defaultFont;
    SDL_Color m_defaultColor;
    SDL_Surface *m_screen;
    std::stringstream m_convertStream;
    static int m_line, m_collum;
    static std::vector<std::vector<SDL_Surface *> > m_surfaces;
    static std::vector<std::vector<SDL_Rect *> > m_possitions;
public:
    TextHandler(SDL_Surface *screen, std::string fontName, int size) {
        if (!TTF_WasInit()) {
            TTF_Init();
            TextHandler::m_line = 0;
            TextHandler::m_collum = 0;
            m_surfaces.push_back(std::vector<SDL_Surface *>());
            m_possitions.push_back(std::vector<SDL_Rect *>());
        }
        m_screen = screen;
        m_defaultColor.r = 255;
        m_defaultColor.g = 255;
        m_defaultColor.b = 255;
        m_defaultFont = TTF_OpenFont(fontName.c_str(), size);
    }

    TextHandler(SDL_Surface *screen, std::string fontName, int size, unsigned int r, unsigned int g, unsigned int b) {
        if (!TTF_WasInit()) {
            TTF_Init();
            TextHandler::m_line = 0;
            TextHandler::m_collum = 0;
            m_surfaces.push_back(std::vector<SDL_Surface *>());
            m_possitions.push_back(std::vector<SDL_Rect *>());
        }
        m_screen = screen;
        m_defaultColor.r = r;
        m_defaultColor.g = g;
        m_defaultColor.b = b;
        m_defaultFont = TTF_OpenFont(fontName.c_str(), size);
    }

    inline int writeOnScreen(int x, int y, const std::string mess) {
        SDL_Rect *offset = new SDL_Rect();
        offset->x = x;
        offset->y = y;
        m_possitions.back().push_back(offset);
        m_surfaces.back().push_back(TTF_RenderUTF8_Solid(m_defaultFont, mess.c_str(), m_defaultColor));
        return SDL_BlitSurface(m_surfaces.back().back(), NULL, m_screen, offset);
    }

    void setColor(unsigned int r, unsigned int g, unsigned int b) {
        m_defaultColor.r = r;
        m_defaultColor.g = g;
        m_defaultColor.b = b;
    }

    inline int nextLine() {
        TextHandler::m_collum = 0;
        int h = TTF_FontHeight(m_defaultFont);
        m_surfaces.push_back(std::vector<SDL_Surface *>());
        m_possitions.push_back(std::vector<SDL_Rect *>());
        if (m_line + h >= m_screen->h) {
            for (unsigned int i = 0; i < m_surfaces.front().size(); i++)
                SDL_FreeSurface(m_surfaces.front()[i]);
            m_surfaces.erase(m_surfaces.begin());
            m_possitions.erase(m_possitions.begin());
            SDL_FillRect(m_screen, NULL, 0x000000);
            for (unsigned int l = 0; l < m_surfaces.size(); l++) {
                for (unsigned int i = 0; i < m_surfaces[l].size(); i++) {
                    m_possitions[l][i]->y -= h;
                    SDL_BlitSurface(m_surfaces[l][i], NULL, m_screen, m_possitions[l][i]);
                }
            }
            return TextHandler::m_line;
        }
        return TextHandler::m_line += h;
    }

    ~TextHandler() {
        for (unsigned int l = 0; l < m_surfaces.size(); l++) {
            for (unsigned int i = 0; i < m_surfaces[l].size(); i++) {
                SDL_FreeSurface(m_surfaces[l][i]);
                delete m_possitions[l][i];

            }
        }
        TTF_CloseFont(m_defaultFont);
        TTF_Quit();
    }

    TextHandler &operator<<(const std::string s);

    TextHandler &operator<<(const SDLKey s);

    TextHandler &operator<<(const int s);

    TextHandler &operator<<(const unsigned int s);

    TextHandler &operator<<(const char s);

    TextHandler &operator<<(const float s);

    TextHandler &operator<<(const double s);

private:
    inline int writeOnScreen(const std::string mess) {
        int length = lengthOfMessage(mess);
        if (TextHandler::m_collum + length >= m_screen->w)
            nextLine();;
        int r = writeOnScreen(TextHandler::m_collum, TextHandler::m_line, mess);
        TextHandler::m_collum += length;
        return r;
    }

    inline int lengthOfMessage(const std::string mes) {
        int w;
        TTF_SizeUTF8(m_defaultFont, mes.c_str(), &w, NULL);
        return w;
    }
};

#endif // TEXTHANDLER_H
