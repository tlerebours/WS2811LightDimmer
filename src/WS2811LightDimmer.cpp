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


#include "WS2811LightDimmer.h"



_LightDimmer::_LightDimmer()
  : mState(LD_OFF),
    mMax(255u),
    mRiseTime(250u),
    mFallTime(250u),
    mOnTime(200u),
    mPeriod(900u),
    mNextEventDate(0u),
    mBlink(false)
{
}

void _LightDimmer::setupMax(const uint8_t inMax)
{
  mMax = inMax;
  mNextEventDate = millis();
  mState = LD_RISING;
  mBlink = false;  
}

void _LightDimmer::on()
{
  switch (mState)
  {
    case LD_OFF:
      mNextEventDate = millis() + mRiseTime;
      mState = LD_RISING;
      break;
    case LD_FALLING:
      uint32_t remainingTime = mNextEventDate - millis();
      mNextEventDate = millis() + (uint32_t)mRiseTime * remainingTime / (uint32_t)mFallTime;
      mState = LD_RISING;
      break;
  }
  mBlink = false;
}

void _LightDimmer::off()
{
  switch (mState)
  {
    case LD_ON:
      mNextEventDate = millis() + mFallTime;
      mState = LD_FALLING;
      break;
    case LD_RISING:
      uint32_t remainingTime = mNextEventDate - millis();
      mNextEventDate = millis() + (uint32_t)mFallTime * remainingTime / (uint32_t)mRiseTime;
      mState = LD_FALLING;
      break;
  }
  mBlink = false;
}

void _LightDimmer::startBlink()
{
  if (mPeriod >= mRiseTime + mOnTime + mFallTime) {
    mBlink = true;
  }
}

/*
 * _LightDimmer::updateState implements the state machine associated to each
 * LED. 
 */
void _LightDimmer::updateState(const uint32_t inCurrentDate, uint8_t& value)
{
  switch (mState) {
    case LD_OFF:
      if (mBlink) {
        if (inCurrentDate >= mNextEventDate) {
          mNextEventDate = inCurrentDate + mRiseTime;
          mState = LD_RISING;
        }
      }
      break;
    case LD_RISING:
      if (inCurrentDate < mNextEventDate) {
        value = mMax * (inCurrentDate - (mNextEventDate - mRiseTime)) / mRiseTime;
      }
      else {
        value = mMax ;
        mNextEventDate = inCurrentDate + mPeriod - mOnTime - mRiseTime -mFallTime;
        mState = LD_ON;
      }
      break;
    case LD_ON:
      if (mBlink) {
        if (inCurrentDate >= mNextEventDate) {
          mNextEventDate = inCurrentDate + mFallTime;
          mState = LD_FALLING;
        }
      }
      break;
    case LD_FALLING:
      if (inCurrentDate < mNextEventDate) {
        value = mMax - mMax * (inCurrentDate - (mNextEventDate - mFallTime)) / mFallTime;
      }
      else {
        value = 0;
        mState = LD_OFF;
        mNextEventDate = inCurrentDate + mPeriod - mOnTime - mRiseTime - mFallTime;
      }
      break;
  }
}

WS2811LightDimmer::WS2811LightDimmer(const uint8_t inNbLight)
  : mPin(63),
    mNextRefreshDate(0u)
{
  mNbLight = ((inNbLight + 2) / 3) * 3; //3 Led on each WS2811
  mLight = new _LightDimmer[mNbLight];
  mLightValue = new uint8_t[mNbLight];
  for (uint8_t i = 0; i < mNbLight; i++) { 
    mLightValue[i] = 0;
  }
}

void WS2811LightDimmer::begin(const uint8_t inPin)
{
  mPin = inPin;
  //pinMode(mPin, OUTPUT);
  pinMask = digitalPinToBitMask(mPin);
  ws2811_port = portOutputRegister(digitalPinToPort(mPin));
  ws2811_port_reg = portModeRegister(digitalPinToPort(mPin));  
}

void WS2811LightDimmer::update()
{
  uint32_t currentDate = millis();
  for (uint8_t i = 0; i < mNbLight; i++) { 
    mLight[i].updateState(currentDate, mLightValue[i]);
  }
  *ws2811_port_reg |= pinMask; // Enable DDR
  ws2811_sendarray_mask(mLightValue, mNbLight, pinMask, (uint8_t*) ws2811_port,(uint8_t*) ws2811_port_reg);  
}

void WS2811LightDimmer::update(const uint8_t inRefreshPeriod)
{
  uint32_t currentDate = millis();  
  if (millis() >= mNextRefreshDate) {
    mNextRefreshDate = currentDate + inRefreshPeriod;
    for (uint8_t i = 0; i < mNbLight; i++) { 
      mLight[i].updateState(currentDate, mLightValue[i]);
    }
    *ws2811_port_reg |= pinMask; // Enable DDR
    ws2811_sendarray_mask(mLightValue, mNbLight, pinMask, (uint8_t*) ws2811_port,(uint8_t*) ws2811_port_reg);  
  }  
}