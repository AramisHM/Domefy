/*	This file is part of the FpMED (Domefy) Project Copyright (c) 2014-2017
   Aramis Hornung Moraes See Fpmed.h for license and conditions for using this
   code
*/

#include <Application/SlideReader/Slideshow.h>

#include <Core/CoreParameters.h>
#include <Core/Environment.h>
#include <irrXML.h>
#include <algorithm>
#include <string>

// Irrlicht namespaces
using namespace irr;
using namespace io;

namespace fpmed {
Slideshow::Slideshow() {}

Slideshow::~Slideshow() {}

void Slideshow::addPage(Page p) { this->pages.push_back(p); }
void Slideshow::clearPresentation() { this->pages.clear(); }

fpmed::Page Slideshow::getPage(int index) { return this->pages[index]; }

void Slideshow::addStyle(ElementStyle p) { this->styles.push_back(p); }
fpmed::ElementStyle Slideshow::getStyle(int index) {
    return this->styles[index];
}
fpmed::ElementStyle Slideshow::getStyleByRefName(std::string refName) {
    for (int i = 0; i < this->styles.size(); ++i) {
        if (this->styles[i].getStyleName() == refName) return this->styles[i];
    }
}

/* Parse a flat odp file for slides information */
void Slideshow::readContentsXML(char* path) {
    // temporay varas
    ElementStyle tempStyle;

    IrrXMLReader* xml = createIrrXMLReader(path);

    // strings for storing the data we want to get out of the file
    std::string modelFile;
    std::string messageText;
    std::string caption;

    if (!xml) return;

    while (xml && xml->read()) {
        switch (xml->getNodeType()) {
            case EXN_ELEMENT: {
                // We want to get the information about the slideshow dimensions
                // on ODF files it can be extracted from styles.xml
                // office:automatic-styles
                // ---------------------------------------------------------------------------------------------------------------
                if (!strcmp("office:automatic-styles", xml->getNodeName())) {
                    do {
                        if (xml->getNodeType() == EXN_ELEMENT) {
                            if (!strcmp("style:style", xml->getNodeName())) {
                                bool isTextstyle = false;
                                if (!strcmp("text", xml->getAttributeValue(
                                                        "style:family"))) {
                                    isTextstyle = true;
                                    printf(
                                        "\nFont reference: %s\n",
                                        xml->getAttributeValue("style:name"));
                                    tempStyle.setStyleName(
                                        xml->getAttributeValue("style:name"));
                                }
                                do {
                                    if (xml->getNodeType() == EXN_ELEMENT) {
                                        if (!strcmp("style:text-properties",
                                                    xml->getNodeName()) &&
                                            isTextstyle) {
                                            printf(
                                                "Font size:%s\nFont "
                                                "color:%s\n\n",
                                                xml->getAttributeValue(
                                                    "fo:font-size"),
                                                xml->getAttributeValue(
                                                    "fo:color"));
                                            std::string strNumFS =
                                                xml->getAttributeValue(
                                                    "fo:font-size");
                                            strNumFS.erase(
                                                std::remove_if(strNumFS.begin(),
                                                               strNumFS.end(),
                                                               isalpha),
                                                strNumFS.end());
                                            tempStyle.setFontSize(
                                                atof(strNumFS.c_str()));

                                            tempStyle.setColorString(
                                                xml->getAttributeValue(
                                                    "fo:color"));

                                            this->addStyle(tempStyle);
                                        }
                                    }
                                    xml->read();
                                } while (
                                    xml->getNodeType() != EXN_ELEMENT_END &&
                                    strcmp("style:style", xml->getNodeName()));
                            }
                        }
                        xml->read();
                    } while (
                        xml->getNodeType() != EXN_ELEMENT_END &&
                        strcmp("office:automatic-styles", xml->getNodeName()));
                }
                // END office:automatic-styles
                // ------------------------------------------------------------------------------------------------------------
                // office:body
                // ---------------------------------------------------------------------------------------------------------------
                if (!strcmp("office:body", xml->getNodeName())) {
                    do {
                        if (xml->getNodeType() == EXN_ELEMENT) {
                            if (!strcmp("office:presentation",
                                        xml->getNodeName())) {
                                do {
                                    if (xml->getNodeType() == EXN_ELEMENT) {
                                        if (!strcmp("draw:page",
                                                    xml->getNodeName())) {
                                            fpmed::Page page;
                                            printf("Page:%s\n",
                                                   xml->getAttributeValue(
                                                       "draw:name"),
                                                   xml->getAttributeValue(
                                                       "fo:color"));

                                            do {
                                                if (xml->getNodeType() ==
                                                    EXN_ELEMENT) {
                                                    if (!strcmp(
                                                            "draw:custom-shape",
                                                            xml->getNodeName())) {
                                                        std::string
                                                            composedString;

                                                        fpmed::Text slideText;
                                                        std::string strNum;

                                                        if (xml->getAttributeValue(
                                                                "svg:width")) {
                                                            strNum =
                                                                xml->getAttributeValue(
                                                                    "svg:"
                                                                    "width");
                                                            strNum.erase(
                                                                std::remove_if(
                                                                    strNum
                                                                        .begin(),
                                                                    strNum
                                                                        .end(),
                                                                    isalpha),
                                                                strNum.end());
                                                            slideText.setWidth(
                                                                atof(
                                                                    strNum
                                                                        .c_str()));
                                                        }

                                                        if (xml->getAttributeValue(
                                                                "svg:height")) {
                                                            strNum =
                                                                xml->getAttributeValue(
                                                                    "svg:"
                                                                    "height");
                                                            strNum.erase(
                                                                std::remove_if(
                                                                    strNum
                                                                        .begin(),
                                                                    strNum
                                                                        .end(),
                                                                    isalpha),
                                                                strNum.end());
                                                            slideText.setHeight(
                                                                atof(
                                                                    strNum
                                                                        .c_str()));
                                                        }

                                                        if (xml->getAttributeValue(
                                                                "svg:x")) {
                                                            strNum =
                                                                xml->getAttributeValue(
                                                                    "svg:x");
                                                            strNum.erase(
                                                                std::remove_if(
                                                                    strNum
                                                                        .begin(),
                                                                    strNum
                                                                        .end(),
                                                                    isalpha),
                                                                strNum.end());
                                                            slideText.setPosX(atof(
                                                                strNum
                                                                    .c_str()));
                                                        }
                                                        if (xml->getAttributeValue(
                                                                "svg:x")) {
                                                            strNum =
                                                                xml->getAttributeValue(
                                                                    "svg:y");
                                                            strNum.erase(
                                                                std::remove_if(
                                                                    strNum
                                                                        .begin(),
                                                                    strNum
                                                                        .end(),
                                                                    isalpha),
                                                                strNum.end());
                                                            slideText.setPosY(atof(
                                                                strNum
                                                                    .c_str()));
                                                        }

                                                        printf(
                                                            "w:%s h:%s x:%s "
                                                            "y:%s %s\n",
                                                            xml->getAttributeValue(
                                                                "svg:width"),
                                                            xml->getAttributeValue(
                                                                "svg:height"),
                                                            xml->getAttributeValue(
                                                                "svg:x"),
                                                            xml->getAttributeValue(
                                                                "svg:y"),
                                                            (xml->getAttributeValue(
                                                                 "draw:"
                                                                 "transform")
                                                                 ? xml->getAttributeValue(
                                                                       "draw:"
                                                                       "transfo"
                                                                       "rm")
                                                                 : ""));

                                                        do {
                                                            if (xml->getNodeType() ==
                                                                EXN_ELEMENT) {
                                                                if (!strcmp(
                                                                        "text:"
                                                                        "p",
                                                                        xml->getNodeName())) {
                                                                    do {
                                                                        if (xml->getNodeType() ==
                                                                            EXN_ELEMENT) {
                                                                            if (!strcmp(
                                                                                    "text:span",
                                                                                    xml->getNodeName())) {
                                                                                do {
                                                                                    if (xml->getNodeType() ==
                                                                                        EXN_ELEMENT) {
                                                                                        if (!strcmp(
                                                                                                "text:span",
                                                                                                xml->getNodeName())) {
                                                                                            printf(
                                                                                                "Estilo: %s",
                                                                                                xml->getAttributeValue(
                                                                                                    "text:style-name"));
                                                                                            xml->read();
                                                                                            printf(
                                                                                                " - texto: %s",
                                                                                                xml->getNodeData());

                                                                                            if (xml->getAttributeValue(
                                                                                                    "text:style-name"))  // only compose text if have style associated
                                                                                            {
                                                                                                slideText
                                                                                                    .setStyleRefName(
                                                                                                        xml->getAttributeValue(
                                                                                                            "text:style-name"));

                                                                                                composedString
                                                                                                    .append(
                                                                                                        xml->getNodeData());
                                                                                                composedString
                                                                                                    .append(
                                                                                                        "\n");
                                                                                                slideText
                                                                                                    .setText(
                                                                                                        composedString);
                                                                                            }

                                                                                            printf(
                                                                                                "\n");
                                                                                        }
                                                                                    }
                                                                                    xml->read();
                                                                                } while (
                                                                                    xml->getNodeType() !=
                                                                                        EXN_ELEMENT_END ||
                                                                                    strcmp(
                                                                                        "text:span",
                                                                                        xml->getNodeName()));
                                                                            }
                                                                        }
                                                                        xml->read();
                                                                    } while (
                                                                        xml->getNodeType() !=
                                                                            EXN_ELEMENT_END ||
                                                                        strcmp(
                                                                            "te"
                                                                            "xt"
                                                                            ":"
                                                                            "p",
                                                                            xml->getNodeName()));
                                                                }
                                                            }
                                                            xml->read();
                                                        } while (
                                                            xml->getNodeType() !=
                                                                EXN_ELEMENT_END ||
                                                            strcmp(
                                                                "draw:custom-"
                                                                "shape",
                                                                xml->getNodeName()));
                                                        page.addShape(
                                                            slideText);
                                                    }
                                                }
                                                xml->read();
                                            } while (
                                                xml->getNodeType() !=
                                                    EXN_ELEMENT_END ||
                                                strcmp("draw:page",
                                                       xml->getNodeName()));

                                            // add the page, for we are done
                                            // with it
                                            this->addPage(page);
                                        }
                                    }
                                    xml->read();
                                } while (xml->getNodeType() !=
                                             EXN_ELEMENT_END ||
                                         strcmp("office:presentation",
                                                xml->getNodeName()));
                            }
                        }
                        xml->read();
                    } while (xml->getNodeType() != EXN_ELEMENT_END ||
                             strcmp("office:body", xml->getNodeName()));
                }
                // END office:body
                // ------------------------------------------------------------------------------------------------------------

            } break;
            case EXN_ELEMENT_END:
                if (!strcmp("office:document-content", xml->getNodeName())) {
                    printf("End of FODP file.\n");
                    break;
                }
                break;
        }
    }
    delete xml;
}
}  // namespace fpmed