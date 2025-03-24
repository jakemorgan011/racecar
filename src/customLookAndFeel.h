/*
  ==============================================================================

    customLookAndFeel.h
    Created: 21 Mar 2025 5:22:03pm
    Author:  Jake Morgan

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

// color pallette maximum...
// can be anything
const size_t NUM_COLORS = 5;

class customLookAndFeel : public juce::LookAndFeel_V4{
public:
    // in case i want to change the colors dynamically
    std::vector<juce::Colour*> cRef;
    
    // call this in the editor constructor so that we get a bunch of goodness
    void setReferences(){
        cRef.reserve(NUM_COLORS);
        cRef = {&background, &line, &accentG, &accentY, &accentR};
    }
    
    
    std::vector<juce::Colour*> getColours(){
        return cRef;
    }
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override {
        auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
        auto centreX = (float) x + (float) width * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto underRW = rw * 1.25f;
        auto insideRW = rw * 0.82;
        auto insideX = centreX - insideRW/2;
        auto insideY = centreY - insideRW/2;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        //
        const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();
        
        g.setColour (background);
        g.fillEllipse(rx, ry, rw, rw);
        // draw slider;
        g.setColour (line);
        g.drawEllipse(rx, ry, rw, rw, lineThickness);
        juce::Path fill;
        fill.addCentredArc(0, 0, rw/2, rw/2, 0, rotaryStartAngle, angle);
        fill.applyTransform(juce::AffineTransform::translation(centreX, centreY));
        
        //
        juce::Path p;
        auto pointerLength = (rw - insideRW)/2;
        auto pointerThickness = lineThickness;
        p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform (juce::AffineTransform::rotation (rotaryStartAngle).translated (centreX, centreY));
        g.fillPath(p);
        
        //
        juce::Path p2;
        p2.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p2.applyTransform (juce::AffineTransform::rotation (rotaryEndAngle).translated (centreX, centreY));
        g.fillPath(p2);
        
        //
        g.fillPath(fill);
        g.setColour (background);
        g.fillEllipse(insideX, insideY, insideRW, insideRW);
        g.setColour (line);
        g.drawEllipse(insideX, insideY, insideRW, insideRW, lineThickness);
        
        // draw amount and text
    }
private:
    // making it static so that if i for some reason need multiple versions of the look and feel they will all match up
    inline static auto background = juce::Colour(252,252,252);
    inline static auto line = juce::Colour(38,84,124);
    inline static auto accentG = juce::Colour(6,214,160);
    inline static auto accentY = juce::Colour(255,209,102);
    inline static auto accentR = juce::Colour(239,71,111);
    
    const float lineThickness = 5.0f;
};



namespace termite {
struct vectorXY {
    float x,y;
};
class stripe : public juce::Component{
    // animate the strip going across screen
public:
    stripe(){}
    ~stripe(){}
    // when declaring this make sure you set the size to only take up a portion of the screen height wise
    stripe(int ofst, float spd) : pixel_offset(ofst){
        currentPos.x = 0 + pixel_offset;
        currentPos.y = 0;
        width = getWidth();
        height = getHeight();
        // these will determine how long the rect is
        stretch.y = height;
        stretch.x = width/8;
        //
        p1.x = 0;
        p1.y = 0;
        p2.x = width;
        p2.y = height;
        //only using x speed vector since we aren't moving up and down
        speed.x = spd;
        speed.y = 0;
    }
    void setColour(juce::Colour& c){
        colour = c;
    }
    void paint(juce::Graphics& g) override {
        g.setColour(colour);
        //g.fillRoundedRectangle(currentPos.x, currentPos.y, stretch.x, stretch.y, 20);
        g.fillRoundedRectangle(currentPos.x, currentPos.y, getWidth()/5, getHeight(), 15);
    }
    // this will get called with each timerCallback()
    void animate(){
        if(!init){
            if(currentPos.x < 0){
                speed.x *= -1;
            }if(currentPos.x+getWidth()/5 > getWidth()){
                speed.x *= -1;
            }
            //currentPos = vectorAdd(currentPos,vectorMult(vectorSub(p2, currentPos),speed));
            currentPos.x = currentPos.x + speed.x;

            stretch = vectorAdd(stretch,vectorMult(vectorSub(p2, stretch),speed));
        }else{
            init = false;
        }
    }
    
    
private:
    int pixel_offset = 0;
    juce::Colour colour;
    vectorXY speed;
    vectorXY currentPos;
    vectorXY stretch;
    vectorXY p1;
    vectorXY p2;
    float counter;
    bool init = true;
    float width;
    float height;
    //
    vectorXY vectorAdd(vectorXY v1, vectorXY v2){
        vectorXY temp;
        temp.x = v1.x + v2.x;
        temp.y = v1.y + v2.y;
        return temp;
    }
    vectorXY vectorSub(vectorXY v1, vectorXY v2){
        vectorXY temp;
        temp.x = v1.x - v2.x;
        temp.y = v1.y - v2.y;
        return temp;
    }
    vectorXY vectorMult(vectorXY v1, vectorXY v2){
        vectorXY temp;
        temp.x = v1.x * v2.x;
        temp.y = v1.y * v2.y;
        return temp;
    }
    vectorXY vectorDiv(vectorXY v1, vectorXY v2){
        vectorXY temp;
        temp.x = v1.x / v2.x;
        temp.y = v1.y / v2.y;
        return temp;
    }
};
// okay i'm just gonna hard code this. cause resizing text dynamically sucks
class textScroll : public juce::Component {
public:
    textScroll(){}
    ~textScroll(){}
    void setText(std::string& t){
        text = t;
    }
    void setColour(juce::Colour& c){
        colour = c;
    }
    void paint(juce::Graphics& g) override {
        g.setFont(18);
        g.setColour(colour);
        g.drawText(scrolledText, 0, 0, getWidth(), getHeight(), juce::Justification::centredBottom);
    }
    // updates every timerCallback()
    void scroll(){
        if(!init){
            // there is 1000% a better way to do this.
            // here i just create a fake loop with the text lol
            if(scroll_index+SCROLL_LENGTH > text.size()){
                scroll_index = 2;
                scrolledText = text.substr(scroll_index,SCROLL_LENGTH);
                scroll_index++;
            }else if (scroll_index+SCROLL_LENGTH <= text.size()){
                scrolledText = text.substr(scroll_index,SCROLL_LENGTH);
                scroll_index++;
            }
        }else{
            scrolledText = text.substr(scroll_index,SCROLL_LENGTH);
            init = false;
        }
    }
    
private:
    const size_t SCROLL_LENGTH = 14;
    bool init = true;
    std::string text;
    std::string scrolledText;
    juce::Colour colour;
    size_t scroll_index = 0;
};
}

class timer : public juce::Timer {
public:
    // probably change this to a bunch of stripes
    void setStripes(std::vector<termite::stripe*> strps){
        stripes = strps;
    }
    void timerCallback() override {
        for(int i = 0; i < stripes.size(); i++){
            stripes[i]->animate();
            stripes[i]->repaint();
        }
    }
private:
    std::vector<termite::stripe*> stripes;
};
class scrollTimer : public juce::Timer {
public:
    void setScroll(termite::textScroll* s){
        scroll = s;
    }
    void timerCallback() override {
        scroll->scroll();
        scroll->repaint();
    }
private:
    termite::textScroll* scroll;
};
