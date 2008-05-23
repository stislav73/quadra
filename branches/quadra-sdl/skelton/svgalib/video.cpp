/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 *
 * Quadra, an action puzzle game
 * Copyright (C) 1998-2000  Ludus Design
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "video.h"

#include <assert.h>
#ifdef WIN32
#include <malloc.h>
#else
#include <alloca.h>
#endif
#include "SDL.h"
#include "video_dumb.h"
#include "bitmap.h"
#include "sprite.h"
#include "command.h"
#include "version.h"
#include "image_png.h"

Video* video = NULL;

class Video_SDL;

class Video_bitmap_SDL: public Video_bitmap {
public:
  Bitmap *fb;
  Video_bitmap_SDL(const int px, const int py,
                   const int w, const int h, const int rw);
  virtual ~Video_bitmap_SDL();
  virtual void rect(const int, const int, const int, const int, const int) const;
  virtual void box(const int, const int, const int, const int, const int) const;
  virtual void put_pel(const int, const int, const Byte) const;
  virtual void hline(const int, const int, const int, const Byte) const;
  virtual void vline(const int, const int, const int, const Byte) const;
  virtual void put_bitmap(const Bitmap&, const int, const int) const;
  virtual void put_sprite(const Sprite&, const int, const int) const;
  virtual void setmem();
	void clip_dirty(int x, int y, int w, int h) const;
};

Video_bitmap* Video_bitmap::New(const int px, const int py,
				const int w, const int h, const int rw) {
  return new Video_bitmap_SDL(px, py, w, h, rw);
}

Video_bitmap* Video_bitmap::New(const int px, const int py,
				const int w, const int h) {
  return new Video_bitmap_SDL(px, py, w, h, video->pitch);
}

class Video_SDL: public Video {
protected:
  int needsleep;
  int lastticks;
  bool fullscreen;
	bool mDirtyEmpty;
	int mDirtyX1;
	int mDirtyY1;
	int mDirtyX2;
	int mDirtyY2;
  void SetVideoMode();
public:
  SDL_Surface *screen_surf; // This is the real screen, 16-bit or 32-bit or whatever. We don't care.
  SDL_Surface *paletted_surf; // This is our temporary palette 'screen' where we actually draw

  Video_SDL();
  virtual ~Video_SDL();
  virtual void start_frame();
  virtual void end_frame();
  virtual void setpal(const Palette&);
  virtual void dosetpal(SPalette*, int);
  virtual void snap_shot(int, int, int, int);
  virtual void toggle_fullscreen();
  void set_dirty(int x1, int y1, int x2, int y2);
};

Video* Video::New(int w, int h, int b, const char *wname, bool dumb) {
  assert(w == 640 && h == 480 && b == 8);

  if(dumb)
    return Video_Dumb::New(w, h, b, wname);
  else
    return new Video_SDL;
}

Video_bitmap_SDL::Video_bitmap_SDL(const int px, const int py,
                                   const int w, const int h, const int rw) {
  width = w;
  height = h;
  pos_x = px;
  pos_y = py;
  fb = new Bitmap(NULL, w, h, rw);
}

Video_bitmap_SDL::~Video_bitmap_SDL() {
  if(fb)
    delete fb;
}

void Video_bitmap_SDL::rect(int x, int y, int w, int h, int color) const {
  SDL_Rect rect;

  if(clip(x, y, w, h))
    return;

  rect.x = clip_x1 + pos_x;
  rect.y = clip_y1 + pos_y;
  rect.w = clip_x2 - clip_x1 + 1;
  rect.h = clip_y2 - clip_y1 + 1;

  SDL_FillRect(static_cast<Video_SDL*>(video)->paletted_surf, &rect, color);
  clip_dirty(x, y, w, h); 
}

void Video_bitmap_SDL::box(int x, int y, int w, int h, int c) const {
  clip_dirty(x, y, w, h); 
  hline(y, x, w, c);
  hline(y + h - 1, x, w, c);
  vline(x, y, h, c);
  vline(x + w - 1, y, h, c);
}

void Video_bitmap_SDL::put_pel(int x, int y, Byte c) const {
  clip_dirty(x, y, 1, 1); 
  fb->put_pel(x, y, c);
}

void Video_bitmap_SDL::hline(int y, int x, int w, Byte c) const {
  clip_dirty(x, y, w, 1); 
  fb->hline(y, x, w, c);
}

void Video_bitmap_SDL::vline(int x, int y, int h, Byte c) const {
  clip_dirty(x, y, 1, h); 
  fb->vline(x, y, h, c);
}

void Video_bitmap_SDL::clip_dirty(int x, int y, int w, int h) const {
  if(clip(x, y, w, h))
    return;
  Video_SDL *vid = static_cast<Video_SDL*>(video);
  vid->set_dirty(pos_x+clip_x1, pos_y+clip_y1, pos_x+clip_x2, pos_y+clip_y2);
}

void Video_bitmap_SDL::put_bitmap(const Bitmap &d, int dx, int dy) const {
  clip_dirty(dx, dy, d.width, d.height); 
  d.draw(*fb, dx, dy);
}

void Video_bitmap_SDL::put_sprite(const Sprite &d, int dx, int dy) const {
  clip_dirty(dx, dy, d.width, d.height); 
  d.draw(*fb, dx, dy);
}

void Video_bitmap_SDL::setmem() {
  Video_SDL *vid = static_cast<Video_SDL*>(video);
  unsigned char *vfb = static_cast<unsigned char *>(vid->paletted_surf->pixels);
  if(fb)
    fb->setmem(vfb + (pos_y * vid->paletted_surf->pitch) + pos_x);
}

Video_SDL::Video_SDL() {
  vb = Video_bitmap::New(0, 0, 640, 480, 640);
  width = 640;
  height = 480;
  framecount = 0;

  fullscreen = false;
  if(!command.token("nofullscreen"))
  {
	fullscreen = true;
  }
  SetVideoMode();

  lastticks = SDL_GetTicks();
  needsleep = 0;
  mDirtyEmpty = true;
}

Video_SDL::~Video_SDL() {
  delete vb;
}

void Video_SDL::start_frame() {
  if(vb)
    vb->setmem();
}

#include "cursor.h"

void Video_SDL::end_frame() {
  if (newpal) {
    pal.set();
    newpal = false;
    set_dirty(0, 0, width, height);
  }

	// Draw and convert only the dirty region to screen
	if(!mDirtyEmpty)
	{
		SDL_Rect rect;
		rect.x = mDirtyX1;
		rect.y = mDirtyY1;
		rect.w = mDirtyX2 - mDirtyX1 + 1;
		rect.h = mDirtyY2 - mDirtyY1 + 1;
		SDL_BlitSurface(paletted_surf, &rect, screen_surf, &rect);
		SDL_UpdateRect(screen_surf, rect.x, rect.y, rect.w, rect.h);
		mDirtyEmpty = true;
	}

	// RV: Make system sleep a little bit to keep framerate around 100 FPS
	{
		int CurrentTicks = SDL_GetTicks();
		int LastDeltaTicks = CurrentTicks - lastticks;
		// If a fairly large delay occured, ignore it and clamp it down to a 1/60.
		// This prevents application from trying to skip several seconds in case
		// system was halted or user dragged the window
		if(LastDeltaTicks > 100)
		{
			LastDeltaTicks = 10;
		}
		lastticks = CurrentTicks;

		// Try to get about 100 FPS (1000 msec / 100 = 10)
		needsleep += (1000/100)-LastDeltaTicks;
		// If needed, sleep a bit to slow down execution speed
		if(needsleep > 0)
		{
			SDL_Delay(needsleep);
		}
		else // Else, simply briefly yield to system
		{
			SDL_Delay(0);
		}
	}

  ++framecount;
}

void Video_SDL::setpal(const Palette &p) {
  pal = p;
  newpal = true;
}

void Video_SDL::dosetpal(SPalette pal[256], int size) {
	SDL_Color *colors;
	int i;

	colors = static_cast<SDL_Color*>(alloca(sizeof(SDL_Color) * size));

	for(i = 0; i < size; i++) {
		colors[i].r = pal[i].peRed;
		colors[i].g = pal[i].peGreen;
		colors[i].b = pal[i].peBlue;
	}

	SDL_SetPalette(paletted_surf, SDL_LOGPAL, colors, 0, size);
}

void Video_SDL::snap_shot(int, int, int, int) {
  assert(false);
}

void Video_SDL::toggle_fullscreen() {
  fullscreen = !fullscreen;
  SetVideoMode();
}

void Video_SDL::SetVideoMode()
{
	// Set window title and window icon using SDL
	{
		char st[256];
		sprintf(st, "Quadra %s", VERSION_STRING);
		SDL_WM_SetCaption(st, NULL);
		Res_doze res("window_newicon.png");
		Png img(res);
		SDL_Surface* surf = SDL_CreateRGBSurfaceFrom(img.pic(), img.width(), img.height(), 8, img.width(), 0, 0, 0, 0);
		{
			SDL_Color *colors = static_cast<SDL_Color*>(alloca(sizeof(SDL_Color) * img.palettesize()));

			for(int i = 0; i < img.palettesize(); i++) {
				Byte* palindex = img.pal() + i*3;
				colors[i].r = palindex[0];
				colors[i].g = palindex[1];
				colors[i].b = palindex[2];
			}
			SDL_SetPalette(surf, SDL_LOGPAL, colors, 0, img.palettesize());
		}
		// Fetch colorkey from top-left pixel value
		SDL_SetColorKey(surf, SDL_SRCCOLORKEY, img.pic()[0]);

		SDL_WM_SetIcon(surf, NULL);
		SDL_FreeSurface(surf);
	}

  int flags = SDL_HWSURFACE;
  if(fullscreen) flags |= SDL_FULLSCREEN;
  screen_surf = SDL_SetVideoMode(640, 480, 0, flags);
  assert(screen_surf);
  paletted_surf = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 8, 0, 0, 0, 0);
  pitch = paletted_surf->pitch;
  need_paint = 2;
  newpal = true;
}

void Video_SDL::set_dirty(int x1, int y1, int x2, int y2)
{
	if(mDirtyEmpty)
	{
		mDirtyX1 = x1;
		mDirtyY1 = y1;
		mDirtyX2 = x2;
		mDirtyY2 = y2;
		mDirtyEmpty = false;
	}
	else
	{
		if(x1 < mDirtyX1) mDirtyX1 = x1;
		if(y1 < mDirtyY1) mDirtyY1 = y1;
		if(x2 > mDirtyX2) mDirtyX2 = x2;
		if(y2 > mDirtyY2) mDirtyY2 = y2;
	}
}