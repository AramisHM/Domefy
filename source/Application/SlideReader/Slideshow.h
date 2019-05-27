/*	This file is part of the FpMED (Domefy) Project Copyright (c) 2014-2017
   Aramis Hornung Moraes See Fpmed.h for license and conditions for using this
   code
*/

#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include "SlideReader.h"

#include <Core/CoreParameters.h>
#include <iostream>
#include <string>
#include <vector>  // c++ standar vector calss
#include "Math/vector3.h"

namespace fpmed {

// This class is generaly used to describe font styles
class Slideshow {
   private:
    std::vector<Page> pages;
    std::vector<ElementStyle> styles;

   public:
    Slideshow();
    ~Slideshow();

    void readContentsXML(char *path);
    void addPage(Page p);
    void addStyle(ElementStyle p);
    fpmed::ElementStyle getStyle(int index);
    fpmed::ElementStyle getStyleByRefName(std::string refName);
    fpmed::Page getPage(int index);

    void clearPresentation();
};
}  // namespace fpmed
#endif