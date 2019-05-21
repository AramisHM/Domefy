/*	This file is part of the FpMED (Domefy) Project Copyright (c) 2014-2017
   Aramis Hornung Moraes See Fpmed.h for license and conditions for using this
   code
*/

#ifndef SLIDEREADER_H
#define SLIDEREADER_H

#include <iostream>
#include <string>
#include <vector>  // c++ standar vector calss
#include "CoreParameters.h"
#include "vector3.h"

// witch vector is best? does it make any sense use our custom vector class?
// TODO
#include "vector2.h"
//#include <Urho3D/Math/Vector2.h>

enum ElementShapeType { EST_TEXT = 0, EST_IMAGE = 1 };
enum SlideType {
    ST_TITLE = 0,         // has only a title and a sub-title
    ST_TITLE_CONTENT = 1  // Has title, sub-title and a text/image content
};

typedef struct slideElement {
    std::string materialPath;
    // Urho3D::Vector2 coord; // camera coordinates on this slide
    fpmed::Vec3<float> coord;  // camera coordinates on this slide
    fpmed::Vec3<float> refPoint;
    int interestPointIndex;  // points to the index of slide text node
    bool hasRefPoint;
    std::string text;
};

namespace fpmed {
// This class is generaly used to describe font styles
class ElementStyle {
   private:
    std::string styleName;
    std::string styleFamily;
    std::string colorString;
    int fontSize;

   public:
    ElementStyle();
    ~ElementStyle();

    void setStyleName(std::string a1);
    void setStyleFamily(std::string a1);
    void setColorString(std::string a1);
    void setFontSize(int a1);

    std::string getStyleName();
    std::string getStyleFamily();
    std::string getColorString();
    int getFontSize();
};

// Elements on the slide, this should be a abstract class
class Shape {
   private:
    float width, height, posX, posY;

    std::string styleRefName;
    ElementShapeType est;

   public:
    void setWidth(float a1);
    void setHeight(float a1);
    void setPosX(float a1);
    void setPosY(float a1);
    void setStyleRefName(std::string es);
    void setShapeType(ElementShapeType est);

    float getWidth();
    float getHeight();
    float getPosX();
    float getPosY();
    std::string getStyleRefName();
    ElementShapeType GetShapeType();

    Shape();
    ~Shape();
};

class Text : public Shape {
   private:
    std::string text;

   public:
    std::string getText();
    void setText(std::string s);

    Text();
    ~Text();
};

class Page {
   private:
    std::vector<Text> slideElements;

   public:
    void addShape(Text s);
    fpmed::Text getShape(int index);
    void clearPage();
    unsigned int numberOfElements();
    Page();
    ~Page();
};

// class that stores all the slides that we can laod from a fds file.
//
class Slides {
   private:
    // the number of slides loaded from .fds file.
    unsigned int nSlides;

   public:
    // set of materials/images for the slides
    std::vector<slideElement> slides;
    // constructor
    Slides();
    // returns the number of slides
    unsigned int getNumberOfSlides();
    // function that loads a xml file that describes the slide set
    void LoadSlides(std::string filePath);
    // returns the number of slides
};
}  // namespace fpmed

#endif