/*	This file is part of the FpMED (Domefy) Project Copyright (c) 2014-2017
   Aramis Hornung Moraes See Fpmed.h for license and conditions for using this
   code
*/

#include "SlideReader.h"

// Irrlicht namespaces
#include "irrXML.h"
using namespace irr;
using namespace io;

namespace fpmed {

// This class is generaly used to describe font styles

ElementStyle::ElementStyle() {}
ElementStyle::~ElementStyle() {}
void ElementStyle::setStyleName(std::string a1) { this->styleName = a1; }
void ElementStyle::setStyleFamily(std::string a1) { this->styleFamily = a1; }
void ElementStyle::setColorString(std::string a1) { this->colorString = a1; }
void ElementStyle::setFontSize(int a1) { this->fontSize = a1; }
std::string ElementStyle::getStyleName() { return this->styleName; }
std::string ElementStyle::getStyleFamily() { return this->styleFamily; }
std::string ElementStyle::getColorString() { return this->colorString; }
int ElementStyle::getFontSize() { return this->fontSize; }

// Elements on the slide, this should be a abstract class
void Shape::setWidth(float a1) { this->width = a1; }
void Shape::setHeight(float a1) { this->height = a1; }
void Shape::setPosX(float a1) { this->posX = a1; }
void Shape::setPosY(float a1) { this->posY = a1; }
void Shape::setStyleRefName(std::string es) { this->styleRefName = es; }
void Shape::setShapeType(ElementShapeType a1) { this->est = a1; }
float Shape::getWidth() { return this->width; }
float Shape::getHeight() { return this->height; }
float Shape::getPosX() { return this->posX; }
float Shape::getPosY() { return this->posY; }
std::string Shape::getStyleRefName() { return this->styleRefName; }
Shape::Shape() {}
Shape::~Shape() {}
ElementShapeType Shape::GetShapeType() { return this->est; }

std::string Text::getText() { return this->text; }
void Text::setText(std::string s) { this->text = s; }
Text::Text() {}
Text::~Text() {}

Page::Page() {}
Page::~Page() {}
void Page::addShape(Text s) { this->slideElements.push_back(s); }
void Page::clearPage() { this->slideElements.clear(); }
unsigned int Page::numberOfElements() { return this->slideElements.size(); }
fpmed::Text Page::getShape(int index) { return slideElements[index]; }

fpmed::Slides::Slides() { this->nSlides = 0; }

void fpmed::Slides::LoadSlides(std::string filePath) {
    // temporay varas
    ElementStyle tempStyle;

    IrrXMLReader *xml = createIrrXMLReader(filePath.c_str());

    // strings for storing the data we want to get out of the file
    std::string modelFile;
    std::string messageText;
    std::string caption;

    if (!xml) return;

    while (xml && xml->read()) {
        switch (xml->getNodeType()) {
            case EXN_ELEMENT: {
                if (!strcmp("SlideGroup", xml->getNodeName())) {
                    do {
                        if (xml->getNodeType() == EXN_ELEMENT) {
                            if (!strcmp("Slide", xml->getNodeName())) {
                                slideElement se;
                                fpmed::Vec3<float> vector(-1.0f, -1.0f, -1.0f);
                                se.interestPointIndex = -1;
                                fpmed::Vec3<float> refPointV;

                                char *overrrideCoord =
                                    (char *)xml->getAttributeValue(
                                        "override_coord");
                                if (overrrideCoord)
                                    vector.atovec3(overrrideCoord);

                                char *refPointC =
                                    (char *)xml->getAttributeValue(
                                        "reference_point");
                                if (refPointC) {
                                    refPointV.atovec3(refPointC);
                                    se.hasRefPoint = true;
                                    se.refPoint = refPointV;
                                } else {
                                    se.hasRefPoint = false;
                                }

                                char *text =
                                    (char *)xml->getAttributeValue("text");
                                if (text) {
                                    se.text = text;
                                } else {
                                    text = "";
                                }

                                ++this->nSlides;
                                se.materialPath =
                                    xml->getAttributeValue("img_file");
                                se.coord = vector;

                                this->slides.push_back(se);
                                printf("%s",
                                       xml->getAttributeValue("img_file"));
                            }
                        }
                        xml->read();
                    } while (xml->getNodeType() != EXN_ELEMENT_END &&
                             strcmp("SlideGroup", xml->getNodeName()));
                }
            } break;
            case EXN_ELEMENT_END:
                if (!strcmp("FDS", xml->getNodeName())) {
                    printf("End of set of slides file\n");
                    break;
                }
                break;
        }
    }
    delete xml;

    printf("Number of slides loaded until now: %d\n\n", this->nSlides);
    for (std::vector<slideElement>::iterator it = this->slides.begin();
         it != slides.end(); ++it) {
        // defub information about the content of this tag
        printf("Slide image: %s\n", (*it).materialPath.c_str());
    }
}

unsigned int fpmed::Slides::getNumberOfSlides() { return nSlides; }
}  // namespace fpmed