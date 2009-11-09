#ifndef _TEXT_H
#define _TEXT_H

#include<libmutil/MemObject.h>

#ifdef __APPLE__
#define DARWIN 1
#endif

#ifdef _MSC_VER
        #include<Winsock2.h>
        #include<windows.h>
        #include"../../SDL-1.2.13/include/SDL.h"
        #include"../../SDL-1.2.13/include/SDL_opengl.h"
        #warn include SDL_ttf header here

#else
	#include <SDL/SDL.h>
        #include <SDL/SDL_opengl.h>
        #ifdef __APPLE__
                #include<SDL_ttf.h>
        #else
                #include<SDL/SDL_ttf.h>
        #endif
#endif

#define TEXT_ALIGN_LEFT 1
#define TEXT_ALIGN_CENTER 2
#define TEXT_ALIGN_RIGHT 3

class TextTexture : public MObject {
	public:
		TextTexture(TTF_Font* font, std::string t, int size, SDL_Color fgc, SDL_Color bgc);
		~TextTexture();

		GLuint getTexture();//{return texture_handle;}
		std::string getText(){return text;}
		int getSize(){return fontsize;}

		int getTextWidth(){return textwidth;}	
		int getTextHeight(){return textheight;}	

		int getTextureWidth(){return texturewidth;}	
		int getTextureHeight(){return textureheight;}	
		
		SDL_Color getFgColor(){return fgColor;}
		SDL_Color getBgColor(){return bgColor;}

		int textwidth;
		int textheight;

		int texturewidth;
		int textureheight;


	private:
		std::string text;
		int fontsize;
		SDL_Color bgColor;
		SDL_Color fgColor;
		GLuint texture_handle;

};

class MFont : public MObject{
	public:
		MFont(TTF_Font* f, int s){font=f; size=s;}
		~MFont(){ TTF_CloseFont(font); }

		TTF_Font* getFont(){return font;}
		int getSize(){return size;}

	private:
		TTF_Font *font;
		int size;
};

class ScreenLocation{
	public:
	        float x1,y1, x2,y2, x3,y3, x4,y4;
		bool contains(float x, float y);
		std::string cmd;
};


class Text : public MObject{
	public:
		Text(std::string font);
		~Text();

		int getTexture(std::string text, int size, SDL_Color fgcolor, SDL_Color bgcolor);
		MRef<TextTexture*> getTextureObject(std::string text, int size, SDL_Color fgcolor, SDL_Color bgcolor);

		void draw2D(int x, int y, std::string text, int size, SDL_Color fgc, SDL_Color bgc);
		void draw3D(float x, float y, float z, float scale, std::string text, int size, SDL_Color fgc, SDL_Color bgc, ScreenLocation *where=NULL);
		void draw3D(float x1, float y1, float z1, float x2, float y2, float z2, std::string text, int size, SDL_Color fgc, SDL_Color bgc, int align);
		int getTextureWidth(std::string text, int size, SDL_Color fgc, SDL_Color bgc);
		int getTextureHeight(std::string text, int size,  SDL_Color fgc, SDL_Color bgc);

		//The actual text within a texture is smaller than the
		//texture itself (that must be a power of two in dimensions).
		int getTextWidth(std::string text, int size, SDL_Color fgc, SDL_Color bgc);
		int getTextHeight(std::string text, int size,  SDL_Color fgc, SDL_Color bgc);

		void restartGl();
	private:
		TTF_Font* getFont(int size);
		std::string fontPath;

		std::list<MRef<MFont*> > fonts;
		std::list<MRef<TextTexture*> > textures;


};

#endif
