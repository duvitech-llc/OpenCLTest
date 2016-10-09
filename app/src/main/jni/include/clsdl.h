/*
                                _                       
                       __ _  __| |_   _____ _ __   __ _ 
                      / _` |/ _` \ \ / / _ \ '_ \ / _` |
                     | (_| | (_| |\ V /  __/ | | | (_| |
                      \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                  |___/ 
*/

#ifndef _CLSDL_H
#define _CLSDL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <fstream>

#include <clobject.h>

#ifdef _WITH_SDL
#include <SDL/SDL.h>
#ifdef _WITH_SDL_TTF
#include <SDL_TTF.h>
#endif //_WITH_SDL_TTF
#endif //_WITH_SDL

class ClSDL : public ClObject {
private:
#ifdef _WITH_SDL
	inline void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
		if((x >= x_max) || (x < 0))
			return;
		if((y >= y_max) || (y < 0))
			return;
		int bpp = surface->format->BytesPerPixel;
		/* Here p is the address to the pixel we want to set */
		Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
		switch(bpp) {
			case 1:
				*p = pixel;
				break;

			case 2:
				*(Uint16 *)p = pixel;
				break;

			case 3:
				if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
					p[0] = (pixel >> 16) & 0xff;
					p[1] = (pixel >> 8) & 0xff;
					p[2] = pixel & 0xff;
				} else {
					p[0] = pixel & 0xff;
					p[1] = (pixel >> 8) & 0xff;
					p[2] = (pixel >> 16) & 0xff;
				}
				break;

			case 4:
				*(Uint32 *)p = pixel;
				break;
		}
	}
#endif //_WITH_SDL
protected:
#ifdef _WITH_SDL
	SDL_Surface* m_screen;
#endif //_WITH_SDL
	int x_max;
	int y_max;
public:
	void set_max(int x, int y) {
		x_max = x;
		y_max = y;
	}
#ifdef _WITH_SDL
	SDL_Event m_event;
#ifdef _WITH_SDL_TTF
	TTF_Font *m_font;
#endif //_WITH_SDL_TTF
#endif //_WITH_SDL
	ClSDL() {
	};
	~ClSDL() {
	};
	inline void open(const char *title, int width, int height) {
#ifdef _WITH_SDL
		SDL_Init(SDL_INIT_EVERYTHING);
		SDL_WM_SetCaption(title, NULL);
		m_screen = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE);
#ifdef _WITH_SDL_TTF
		TTF_Init();
		// /opt/wine-staging/share/wine/fonts/cour.ttf
		// /usr/share/fonts/truetype/msttcorefonts/cour.ttf
		m_font = TTF_OpenFont( "cour.ttf", 12 );
#endif //_WITH_SDL_TTF
		set_open();
#endif //_WITH_SDL
	};
	inline void close() {
#ifdef _WITH_SDL
#ifdef _WITH_SDL_TTF
		TTF_CloseFont(m_font);
		TTF_Quit();
#endif //_WITH_SDL_TTF
		SDL_Quit();
#endif //_WITH_SDL
	};
	void text(char *text) {
#ifdef _WITH_SDL_TTF
		SDL_Color clrFg = {0,0,255,0}; 
		SDL_Surface *sText = TTF_RenderText_Solid(m_font, text, clr);
		SDL_Rect rcDest = {0,0,0,0};
		SDL_BlitSurface(sText, NULL, m_screen, &rcDest);
#endif //_WITH_SDL_TTF
	}
	inline void update_screen() {
#ifdef _WITH_SDL
		SDL_UpdateRect(m_screen, 0, 0, 0, 0);
#endif //_WITH_SDL
	}
	inline void draw_rgb(int x, int y, char r, char g, char b) {
#ifdef _WITH_SDL
		Uint32 pixel = SDL_MapRGB(m_screen->format, r, g, b);
		if ( SDL_MUSTLOCK(m_screen) ) {
			if ( SDL_LockSurface(m_screen) < 0 ) {
				TRACE("lock failed %s\n", SDL_GetError());
			}
		}

		putpixel(m_screen, x, y, pixel);

		if ( SDL_MUSTLOCK(m_screen) ) {
			SDL_UnlockSurface(m_screen);
		}
#endif //_WITH_SDL
	};
	inline bool poll() {
		PERF_START("sdlpoll");
#ifdef _WITH_SDL
		m_event.key.keysym.sym = (SDLKey)0;
		if(SDL_PollEvent(&m_event) == 0)
			return true;
		if(m_event.type == SDL_QUIT)
			return false;
		if(m_event.type == SDL_KEYDOWN) {
			switch(m_event.key.keysym.sym) {
				case SDLK_KP7:
				case SDLK_KP1:
				case SDLK_KP8:
				case SDLK_KP2:
				case SDLK_KP9:
				case SDLK_KP3:
				case SDLK_KP4:
				case SDLK_KP6:
				case SDLK_KP_PLUS:
				case SDLK_KP_MINUS:
				case SDLK_KP5:
				case SDLK_KP0:
				case SDLK_KP_PERIOD:
				case SDLK_KP_DIVIDE:
				case SDLK_KP_MULTIPLY:
				case SDLK_DOWN:
				case SDLK_RIGHT:
				case SDLK_LEFT:
				case SDLK_UP:
				case SDLK_F2:
				case SDLK_F3:
				case SDLK_F4:
				case SDLK_F5:
				case SDLK_F6:
				case SDLK_F7:
				case SDLK_F8:
				case SDLK_F9:
				case SDLK_F10:
				case SDLK_F11:
				case SDLK_F12:
					return true;
			default: // anykey quit
				fflush(stdout);
				return false;
			}
		}
#endif //_WITH_SDL
		PERF_END("sdlpoll");
		return true;
	}
#ifdef _WITH_SDL
	void set_pixel(int x, int y, unsigned int pixel) {
		putpixel(m_screen, x, y, pixel);
//		    Uint8 *target_pixel = m_screen.pixels + y * m_screen.pitch + x * 4;
//			     *(Uint32 *)target_pixel = pixel;
	}
#endif //_WITH_SDL
	inline void draw_circle(int n_cx, int n_cy, int radius, char r, char g, char b) {
#ifdef _WITH_SDL
		Uint32 pixel = SDL_MapRGB(m_screen->format, r, g, b);
		// if the first pixel in the screen is represented by (0,0) (which is in sdl)
		// remember that the beginning of the circle is not in the middle of the pixel
		// but to the left-top from it:

		double error = (double)-radius;
		double x = (double)radius -0.5;
		double y = (double)0.5;
		double cx = n_cx - 0.5;
		double cy = n_cy - 0.5;

		while (x >= y) {
			set_pixel((int)(cx + x), (int)(cy + y), pixel);
			set_pixel((int)(cx + y), (int)(cy + x), pixel);

			if (x != 0) {
				set_pixel((int)(cx - x), (int)(cy + y), pixel);
				set_pixel((int)(cx + y), (int)(cy - x), pixel);
			}

			if (y != 0) {
				set_pixel((int)(cx + x), (int)(cy - y), pixel);
				set_pixel((int)(cx - y), (int)(cy + x), pixel);
			}

			if (x != 0 && y != 0) {
				set_pixel((int)(cx - x), (int)(cy - y), pixel);
				set_pixel((int)(cx - y), (int)(cy - x), pixel);
			}

			error += y;
			++y;
			error += y;

			if (error >= 0) {
				--x;
				error -= x;
				error -= x;
			}
		}
#endif //_WITH_SDL
	};
	unsigned char *get(int x, int y) {
#ifdef _WITH_SDL
		SDL_Surface *surface = m_screen;
		int bpp = surface->format->BytesPerPixel;
		return (unsigned char *)surface->pixels + y * surface->pitch + x * bpp;
#else
		return 0;
#endif //_WITH_SDL
	}
	void set_frame(ClFrame &frame) {
		unsigned char *p = get(0, 0);
		memcpy(p, frame.get(), frame.length());
	};
	void set_frame_crop(ClFrame &frame) {
		int x = 0; 
		int y = 0; 
		size_t origin[3];
		size_t region[3];
		frame.get_origin(origin);
		frame.get_region(region);
		int ox = origin[0];
		int len = region[1] * 4;
		//x = ox;
		for(size_t oy = origin[1]; oy < (origin[1] + region[1]); oy++) {
			//TRACE("(%d,%d) - (%d,%d) (%zu,%zu)\n", ox, oy, x, y, region[0], region[1]);
			memcpy(get(ox, oy), frame.at(x, y), len);
			y++;
		}
	};
};


#endif //_CLSDL_H

