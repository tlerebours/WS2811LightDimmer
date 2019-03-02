/*
 * Light Dimmer Library for Arduino
 *
 * Copyright Jean-Luc Béchennec 2018
 *
 * This software is distributed under the GNU Public Licence v2 (GPLv2)
 *
 * Please read the LICENCE file
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __WS2811_LIGHT_DIMMER__
#define __WS2811_LIGHT_DIMMER__

#include "Arduino.h"

class _LightDimmer
{
  private:
    enum { LD_OFF, LD_ON, LD_RISING, LD_FALLING };

    uint8_t  mState;
    uint8_t  mMax;
    uint16_t mRiseTime;
    uint16_t mFallTime;
    uint16_t mOnTime;
    uint16_t mPeriod;
    uint32_t mNextEventDate;
    bool     mBlink;
    
  public:
    _LightDimmer();
    void setMax(const uint8_t inMax) { mMax = inMax; };
    void setFadingTime(const uint16_t inFallTime) { mFallTime = inFallTime > 0 ? inFallTime : 1; };
    void setBrighteningTime(const uint16_t inRiseTime) { mRiseTime = inRiseTime > 0 ? inRiseTime : 1; };
    void setOnTime(const uint16_t inOnTime) { mOnTime = inOnTime; };
    void setPeriod(const uint16_t inPeriod) { mPeriod = inPeriod; };
    void setupMax(const uint8_t inMax);    
    void on();
    void off();
    void startBlink();
    void stopBlink() { mBlink = false; };

    bool isOn()                { return (mState == LD_ON); }
    bool isOff()               { return (mState == LD_OFF); }
    bool isFading()            { return (mState == LD_FALLING); }
    bool isBrightening()       { return (mState == LD_RISING); }
    bool isBlinking()          { return mBlink; }
    uint8_t maximum()          { return mMax; }
    uint16_t fadingTime()      { return mFallTime; }
    uint16_t brighteningTime() { return mRiseTime; }
    uint16_t onTime()          { return mOnTime; }
    uint16_t period()          { return mPeriod; }

    void updateState(const uint32_t inCurrentDate, uint8_t& value);
};

class WS2811LightDimmer
{
  private:
    uint8_t mPin;
    uint8_t mNbLight;
    uint8_t *mLightValue;

    void ws2811_sendarray_mask(uint8_t *array,uint16_t length, uint8_t pinmask,uint8_t *port, uint8_t *portreg);
    const volatile uint8_t *ws2811_port;
    volatile uint8_t *ws2811_port_reg;
    uint8_t pinMask;     

  public:
    WS2811LightDimmer(const uint8_t inNbLight);
    void begin(const uint8_t inPin);
    void update();
    
    _LightDimmer *light;
};

#endif