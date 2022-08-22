/***********      .---.         .-"-.      *******************\
* -------- *     /   ._.       / � ` \     * ---------------- *
* Author's *     \_  (__\      \_�v�_/     * humus@rogers.com *
*   note   *     //   \\       //   \\     * ICQ #47010716    *
* -------- *    ((     ))     ((     ))    * ---------------- *
*          ****--""---""-------""---""--****                  ********\
* This file is a part of the work done by Humus. You are free to use  *
* the code in any way you like, modified, unmodified or copy'n'pasted *
* into your own work. However, I expect you to respect these points:  *
*  @ If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  @ For use in anything commercial, please request my approval.      *
*  @ Share your work and ideas too as much as you can.                *
\*********************************************************************/

#ifndef _TEXTUREPACKER_H_
#define _TEXTUREPACKER_H_

#include "set.h"

struct TextureRectangle {
    unsigned int x, y;
    unsigned int width, height;
};

class TexturePacker {
public:
    ~TexturePacker();

    void addRectangle(unsigned int width, unsigned int height);

    bool assignCoords(unsigned int *width, unsigned int *height);

    TextureRectangle *getRectangle(unsigned int index) const { return rects[index]; }

protected:
    Set<TextureRectangle *> rects;
};

#endif // _TEXTUREPACKER_H_
