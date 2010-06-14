
#include"Text.h"
#include<math.h>

using namespace std;


static int nextpoweroftwo(int x)
{
	double logbase2 = log(x) / log(2);
	return round(pow(2,ceil(logbase2)));
}

static float colorDiff(int r1, int g1, int b1,  int r2, int g2, int b2){
	float dr = r1-r2;
	float dg = g1-g2;
	float db = b1-b2;

	return sqrt(dr*dr + dg*dg + db*db);
}

TextTexture::TextTexture(TTF_Font* font, std::string t, int size, SDL_Color fgcolor, SDL_Color bgcolor){
	//cerr <<"EEEE: TextTexture::TextTexture started"<<endl;
	fgColor=fgcolor;
	bgColor=bgcolor;

	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	text=t;
	fontsize=size;
	SDL_Surface *initial=NULL;
	SDL_Surface *intermediary=NULL;
	massert(font);
	//cerr <<"EEEE: TextTexture::TextTexture rendering"<<endl;
	initial = TTF_RenderText_Blended(font, text.c_str(), fgcolor);
	textwidth = initial->w;
	textheight = initial->h;
	//cerr << "EEEE: TextureText: width="<<textwidth<<endl;
	int w = nextpoweroftwo(initial->w);
	int h = nextpoweroftwo(initial->h);
	texturewidth=w;
	textureheight=h;
//	cerr << "EEEE: TextureText: power of two width="<<texturewidth<<endl;
	intermediary = SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);
	massert(intermediary);
	for (int i=0; i<w*h; i++){
		((unsigned char*)intermediary->pixels)[i*4+0]=bgcolor.r;
		((unsigned char*)intermediary->pixels)[i*4+1]=bgcolor.g;
		((unsigned char*)intermediary->pixels)[i*4+2]=bgcolor.b;
	}


	SDL_BlitSurface(initial, 0, intermediary, 0);

	//int maxcolor = fgcolor.r + fgcolor.g + fgcolor.b;
	float maxdiff = colorDiff(fgcolor.r, bgcolor.r,  fgcolor.g,bgcolor.g,  fgcolor.b,bgcolor.b);
	if (maxdiff<1.0)	//prevent division by zero
		maxdiff=1.0;

	//cerr <<"EEEE: TextureText::TextureText: setting alpha"<<endl;

	for (int i=0; i<w*h; i++){
		float cdiff = colorDiff(bgcolor.r, ((unsigned char*)intermediary->pixels)[i*4],
					bgcolor.g, ((unsigned char*)intermediary->pixels)[i*4+1],
					bgcolor.b, ((unsigned char*)intermediary->pixels)[i*4+2] );
		int alpha = cdiff*255/maxdiff;
		((unsigned char*)intermediary->pixels)[i*4+3]=alpha;
/*
		if (((unsigned char*)intermediary->pixels)[i*4]==bgcolor.r &&
				((unsigned char*)intermediary->pixels)[i*4+1]==bgcolor.g &&
				((unsigned char*)intermediary->pixels)[i*4+2]==bgcolor.b )
			((unsigned char*)intermediary->pixels)[i*4+3]=0;
*/		
	}

	//cerr <<"EEEE: TextureText::TextureText: generating gl texture"<<endl;

	glGenTextures(1, &texture_handle);
	massert(glGetError()==GL_NO_ERROR);
	//cerr <<"EEEE: TextureText::TextureText: binding texture"<<endl;
	glBindTexture(GL_TEXTURE_2D, texture_handle);
	massert(glGetError()==GL_NO_ERROR);

	//cerr <<"EEEE: TextureText::TextureText: uploading to gl"<<endl;
	//glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, intermediary->pixels );
//	cerr << "EEEE: doing glTexImage2d"<<endl;
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, intermediary->pixels );
	massert(glGetError()==GL_NO_ERROR);

	//cerr <<"EEEE: TextureText::TextureText: gl finish"<<endl;
	glFinish();
	massert(glGetError()==GL_NO_ERROR);
	//cerr <<"EEEE: TextureText::TextureText: sdl free"<<endl;
	SDL_FreeSurface(initial);
	SDL_FreeSurface(intermediary);
	//cerr << "EEEE: TextureText::TextureText: texture id="<<texture_handle<<" generated for text "<<t<<"/"<<size<<endl;

}

TextTexture::~TextTexture(){
	//cerr << "EE: glDeleteTextures "<<texture_handle<<endl;
	glDeleteTextures(1, (GLuint*)&texture_handle);
	massert(glGetError()==GL_NO_ERROR);
}

GLuint TextTexture::getTexture(){
	return texture_handle;
}



Text::Text(std::string fontpath){
	//cerr << "EE: Text() opening font"<<endl;
	fontPath=fontpath;
}

Text::~Text(){
}


static bool ceq(SDL_Color c1, SDL_Color c2){
	return c1.r==c2.r && c1.g==c2.g && c1.b==c2.b;
}

MRef<TextTexture*> Text::getTextureObject(std::string text, int size, SDL_Color fgcolor, SDL_Color bgcolor){
	std::list<MRef<TextTexture*> >::iterator i;
	//cerr <<"EEEE: Text::getTextureObject: start"<<endl;
	for (i=textures.begin(); i!=textures.end(); i++){
		if (    	(*i)->getText()==text && 
				(*i)->getSize()==size && 
				ceq( (*i)->getFgColor(), fgcolor) && 
				ceq( (*i)->getBgColor(), bgcolor)  ){
			//cerr <<"EEEE: Text::getTextureObject: return fached"<<endl;
			return *i;
		}
	}
	//cerr <<"EEEE: Text::getTextureObject: creating texture"<<endl;
	MRef<TextTexture*> t = new TextTexture(getFont(size), text, size, fgcolor,bgcolor);
	//cerr <<"EEEE: Text::getTextureObject: creating done texture"<<endl;
	textures.push_back(t);
	//cerr << "EE: after getTextureObject: nr textures: "<<textures.size()<<endl;
	return t;
	
}

int Text::getTexture(std::string text, int size, SDL_Color fgcolor, SDL_Color bgcolor){
	return getTextureObject(text,size, fgcolor, bgcolor)->getTexture();
}

void glEnable2D()
{
        int vPort[4];
  
        glGetIntegerv(GL_VIEWPORT, vPort);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
  
        glOrtho(0, vPort[2], 0, vPort[3], -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
	massert(glGetError()==GL_NO_ERROR);

	//static int i=0;
	//float xrad=10.0f*sin(i/300.0f);
	//float zrad=-25.0f-10.0f*cos(i/300.0f);
	//glTranslatef( xrad, 0.0f, zrad);
	//glRotatef(i/1.0f, i, 1.0f, 0.0f);

}

void glDisable2D()
{
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();  
}


static void getScreenCoord(GLdouble x, GLdouble y, GLdouble z, GLdouble* winx, GLdouble* winy, GLdouble* winz){

	GLdouble model[16];
	GLdouble proj[16];
	GLint view[4];

	glGetDoublev(GL_PROJECTION_MATRIX, &proj[0]);
	glGetDoublev(GL_MODELVIEW_MATRIX, &model[0]);
	glGetIntegerv(GL_VIEWPORT, &view[0]);

	GLdouble tmpz;

//	cerr <<"EEEEEEEE: projecting "<<x<<" x "<<y<<" x "<<z<<endl;
	gluProject(x,y,z, model, proj, view, winx,winy,&tmpz);
	massert(glGetError()==GL_NO_ERROR);
//	cerr << "EEEEEEEE: projected to "<<(int)*winx<<" x "<<(int)*winy<<endl;
	if (winz)
		*winz=tmpz;
}

void Text::draw2D( int x, int y, std::string text, int size, SDL_Color fgc, SDL_Color bgc ){
	MRef<TextTexture*> t =  getTextureObject(text, size, fgc, bgc);
	glEnable2D();
	massert(glGetError()==GL_NO_ERROR);
	glDisable(GL_DEPTH_TEST);

	//glBlendFunc(GL_ONE, GL_ONE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_BLEND);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	

	glEnable(GL_TEXTURE_2D);

	int tex = t->getTexture();
//	cerr <<"EE: draw: drawing texture "<< tex << " for text "<<text<<endl;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex );

	glBindTexture(GL_TEXTURE_2D, tex );
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	//glTexCoord2f(0.0f, 0.0f);
	glVertex2f(x ,y);

	glTexCoord2f(1.0f, 1.0f);
	//glTexCoord2f(0.0f, 1.0f);
	glVertex2f(x + t->getTextureWidth(), y);

	glTexCoord2f(1.0f, 0.0f);
	//glTexCoord2f(1.0f, 1.0f);
	glVertex2f(x + t->getTextureWidth(), y + t->getTextureHeight());

	glTexCoord2f(0.0f, 0.0f);
	//glTexCoord2f(1.0f, 0.0f);
	glVertex2f(x,                 y + t->getTextureHeight() );
	glEnd();

	glDisable2D();
}


void Text::draw3D( float x, float y, float z, float scale, std::string text, int size, SDL_Color fgc, SDL_Color bgc, ScreenLocation* where ){
	MRef<TextTexture*> t =  getTextureObject(text, size, fgc, bgc);

	//glBlendFunc(GL_ONE, GL_ONE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	massert(glGetError()==GL_NO_ERROR);

	glEnable(GL_BLEND);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

	glEnable(GL_TEXTURE_2D);

	int tex = t->getTexture();
//	cerr <<"EE: draw: drawing texture "<< tex << " for text "<<text<<endl;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex );
	massert(glGetError()==GL_NO_ERROR);
	//glColor3f(c.r/255.0, c.g/255.0, c.b/255.0);

	glBindTexture(GL_TEXTURE_2D, tex );
	massert(glGetError()==GL_NO_ERROR);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	//glTexCoord2f(0.0f, 0.0f);


	glVertex2f(x*scale ,y*scale);


	glTexCoord2f(1.0f, 1.0f);
	//glTexCoord2f(0.0f, 1.0f);
	glVertex2f((x + t->getTextureWidth())*scale, y*scale);


	glTexCoord2f(1.0f, 0.0f);
	//glTexCoord2f(1.0f, 1.0f);
	glVertex2f((x + t->getTextureWidth())*scale, (y + t->getTextureHeight())*scale );


	glTexCoord2f(0.0f, 0.0f);
	//glTexCoord2f(1.0f, 0.0f);
	glVertex2f(x*scale,                 (y + t->getTextureHeight())*scale );

	glEnd();
	massert(glGetError()==GL_NO_ERROR);

	GLdouble X,Y;

//	cerr << "EEEE: x=" << x <<" y=" << y <<" height=" << t->getTextHeight() << " scale=" << scale << endl;
//	getScreenCoord(x*scale, -1.0*y*scale - t->getTextHeight()*scale*2.0, 0.0 , &X, &Y, NULL);
	getScreenCoord(x*scale, -1.0*y*scale + ((float)t->getTextHeight())*scale/16, 0.0 , &X, &Y, NULL);
	if (where){
		where->x1=X;
		where->y1=Y;
	}
	getScreenCoord((x + t->getTextWidth())*scale, -1.0*y*scale + t->getTextHeight()*scale/16 , 0.0, &X, &Y, NULL);
	if (where){
		where->x2=X;
		where->y2=Y;
	}

	getScreenCoord( (x + t->getTextWidth())*scale, -1.0*(y + t->getTextHeight())*scale , 0, &X, &Y, NULL);
	if (where){
		where->x3=X;
		where->y3=Y;
	}

	getScreenCoord( x*scale, -1.0*(y + t->getTextHeight())*scale , 0.0, &X, &Y, NULL);
	if (where){
		where->x4=X;
		where->y4=Y;
	}

}




static void rectToCoord(float &x1, float&y1, float &x2, float &y2, float lx1, float ly1, float lx2, float ly2, float aratio, int halign)
{

        float l_aratio=(lx2-lx1)/(ly2-ly1);

	float xalign=0;
	//float yalign=0;

        if (aratio > l_aratio) { // fill full width, center vertically
                float h=(lx2-lx1)/aratio;
                float lh=ly2-ly1;
                x1=lx1;
                y1=ly1+((lh-h)/2.0);
                x2=lx2;
                y2=y1+h;
		float xalign=0;
        }else{  //fill full height

                y1=ly1;
                float w=(ly2-ly1)*aratio;
                float lw=lx2-lx1;

                x1=lx1+(lw-w)/2.0;
                y2=ly2;
                x2=x1+w;
		switch(halign){
			case TEXT_ALIGN_LEFT:
				xalign=-( (lx2-lx1)-(x2-x1) )/2.0;
				break;
			case TEXT_ALIGN_RIGHT:
				xalign=( (lx2-lx1)-(x2-x1) )/2.0;
			case TEXT_ALIGN_CENTER:
				break;
		}
        }

	x1=x1+xalign;
	x2=x2+xalign;
	

//	cerr <<"EEEE: mapped "<<lx1<<","<<ly1<<"->"<<lx2<<","<<ly2<<" to " << x1<<","<<y1<<"->"<<x2<<","<<y2<<endl;
}


void Text::draw3D( float x1, float y1, float z1, float x2, float y2, float z2, std::string text, int size, SDL_Color fgc, SDL_Color bgc, int align ){
	//cerr <<"EEEE: Text::draw3D: start"<<endl;
	MRef<TextTexture*> t =  getTextureObject(text, size, fgc, bgc);
	//cerr <<"EEEE: Text::draw3D: gl parameters"<<endl;

	//glBlendFunc(GL_ONE, GL_ONE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	massert(glGetError()==GL_NO_ERROR);

	glEnable(GL_BLEND);
	massert(glGetError()==GL_NO_ERROR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

	glEnable(GL_TEXTURE_2D);
	massert(glGetError()==GL_NO_ERROR);

	//cerr <<"EEEE: Text::draw3D: gettexture"<<endl;
	int tex = t->getTexture();
	//cerr <<"EE: draw: drawing texture "<< tex << " for text "<<text<<endl;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex );
	massert(glGetError()==GL_NO_ERROR);
	//glColor3f(c.r/255.0, c.g/255.0, c.b/255.0);


//	cerr <<"EEEE: textwidth="<<(float)t->textwidth<<endl;
//	cerr <<"EEEE: textheight="<<(float)t->textheight<<endl;
	rectToCoord(x1,y1,x2,y2,x1,y1,x2,y2, (float)t->textwidth/(float)t->textheight, align);

	glBindTexture(GL_TEXTURE_2D, tex );
	massert(glGetError()==GL_NO_ERROR);
	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, /*1.0f*/ (float)t->textheight/(float)t->textureheight);
	glVertex3f(x1 ,y1, z1);


	glTexCoord2f(/*1.0f*/ (float)t->textwidth/(float)t->texturewidth , /*1.0f*/ (float)t->textheight/(float)t->textureheight);
	//glTexCoord2f(0.0f, 1.0f);
	glVertex3f(x2, y1, z1);


	glTexCoord2f(/*1.0f*/ (float)t->textwidth/(float)t->texturewidth, 0.0f);
	//glTexCoord2f(1.0f, 1.0f);
	glVertex3f(x2, y2,z1 );


	glTexCoord2f(0.0f, 0.0f);
	//glTexCoord2f(1.0f, 0.0f);
	glVertex3f(x1, y2, z1 );

	glEnd();
	massert(glGetError()==GL_NO_ERROR);
	//cerr <<"EEEE: Text::draw3D: end"<<endl;
}


int Text::getTextureWidth(std::string text, int size, SDL_Color fgc, SDL_Color bgc ){
	MRef<TextTexture*> t =  getTextureObject(text, size, fgc, bgc);
	return t->getTextureWidth();
}

int Text::getTextureHeight(std::string text, int size, SDL_Color fgc, SDL_Color bgc ){
	MRef<TextTexture*> t =  getTextureObject(text, size, fgc, bgc);
	return t->getTextureHeight();
}

int Text::getTextWidth(std::string text, int size, SDL_Color fgc, SDL_Color bgc ){
	MRef<TextTexture*> t =  getTextureObject(text, size, fgc, bgc);
	return t->getTextWidth();
}

int Text::getTextHeight(std::string text, int size, SDL_Color fgc, SDL_Color bgc ){
	MRef<TextTexture*> t =  getTextureObject(text, size, fgc, bgc);
	return t->getTextHeight();
}




void Text::restartGl(){
	textures.clear();
}

TTF_Font* Text::getFont(int size){
	list<MRef<MFont*> >::iterator i;
	for (i=fonts.begin(); i!=fonts.end(); i++)
		if ( (*i)->getSize()==size)
			return (*i)->getFont();


	TTF_Font* font;
	if(!(font = TTF_OpenFont(fontPath.c_str(), size))) {
		printf("Error loading font: %s", TTF_GetError());
		exit(1);
	}
	fonts.push_back(new MFont(font, size));
	return font;
}

bool ScreenLocation::contains(float x, float y){
//	cerr << "ScreenLocation: comparing "<<x<<"x"<<y<<" with "<<x1<<"x"<<y1<<" "<<x2<<"x"<<y2<<" "<<x3<<"x"<<y3<<" "<<x4<<"x"<<y4<<endl;
//	cerr << (x>=x1) <<" "<< (x<=x2) <<" "<< (y>=y1)<<" "<< (y<=y3) <<endl;

	if (x>=x1 && x<=x2 && y>=y3 && y<=y1)	// FIXME: assumes 2D rectangle
		return true;
	else 
		return false;
}

