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

#include "TexturePacker.h"

struct TextureNode {
    TextureNode(unsigned int x, unsigned int y, unsigned w, unsigned int h) {
        left = right = NULL;

        rect = new TextureRectangle;
        rect->x = x;
        rect->y = y;
        rect->width = w;
        rect->height = h;
    }

    ~TextureNode() {
        delete left;
        delete right;
        delete rect;
    }

    bool assignRectangle(TextureRectangle *rect);

    TextureNode *left;
    TextureNode *right;

    TextureRectangle *rect;
};

bool TextureNode::assignRectangle(TextureRectangle *newRect) {
    if (rect == NULL) {
        if (left->assignRectangle(newRect)) return true;
        return right->assignRectangle(newRect);
    } else {
        if (newRect->width <= rect->width && newRect->height <= rect->height) {
            newRect->x = rect->x;
            newRect->y = rect->y;

            left = new TextureNode(rect->x, rect->y + newRect->height, newRect->width, rect->height - newRect->height);
            right = new TextureNode(rect->x + newRect->width, rect->y, rect->width - newRect->width, rect->height);

            delete rect;
            rect = NULL;
            return true;
        }
        return false;
    }
}

TexturePacker::~TexturePacker() {
    for (unsigned int i = 0; i < rects.getCount(); i++) {
        delete rects[i];
    }
}


void TexturePacker::addRectangle(unsigned int width, unsigned int height) {
    TextureRectangle *rect = new TextureRectangle;

    rect->width = width;
    rect->height = height;

    rects.add(rect);
}

int compare(TextureRectangle *const &elem0, TextureRectangle *const &elem1) {
    return elem1->width * elem1->height - elem0->width * elem0->height;
}

bool TexturePacker::assignCoords(unsigned int *width, unsigned int *height) {
    Set<TextureRectangle *> sortedRects;
    sortedRects.setCount(rects.getCount());
    memcpy(sortedRects.getArray(), rects.getArray(), rects.getCount() * sizeof(TextureRectangle *));

    sortedRects.sort(compare);

    TextureNode *top = new TextureNode(0, 0, *width, *height);

    *width = 0;
    *height = 0;
    for (unsigned int i = 0; i < sortedRects.getCount(); i++) {
        if (top->assignRectangle(sortedRects[i])) {
            unsigned int x = sortedRects[i]->x + sortedRects[i]->width;
            unsigned int y = sortedRects[i]->y + sortedRects[i]->height;
            if (x > *width) *width = x;
            if (y > *height) *height = y;
        } else {
            delete top;
            return false;
        }
    }

    delete top;
    return true;
}
