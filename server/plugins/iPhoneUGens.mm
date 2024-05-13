/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <TargetConditionals.h>

#if defined(SC_IPHONE) && !TARGET_IPHONE_SIMULATOR

#    import <UIKit/UIKit.h>

#    include "SC_PlugIn.h"

/*
#if TARGET_IPHONE_SIMULATOR
const double pi = 3.14f;
#endif
*/

static InterfaceTable* ft;

@interface AccelerometerDelegate : NSObject <UIAccelerometerDelegate> {
    UIAccelerationValue accel_x, accel_y, accel_z;
}
- (float)accel_x;
- (float)accel_y;
- (float)accel_z;
@end

static AccelerometerDelegate* delegate = 0;


struct AccelerometerUGen : public Unit {
    float m_y1, m_b1, m_lag;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
void load(InterfaceTable* inTable);

void AccelerometerX_next(AccelerometerUGen* unit, int inNumSamples);
void AccelerometerY_next(AccelerometerUGen* unit, int inNumSamples);
void AccelerometerZ_next(AccelerometerUGen* unit, int inNumSamples);

void AccelerometerX_Ctor(AccelerometerUGen* unit);
void AccelerometerY_Ctor(AccelerometerUGen* unit);
void AccelerometerZ_Ctor(AccelerometerUGen* unit);
};

//////////////////////////////////////////////////////////////////////////////////////////////////

void AccelerometerX_next(AccelerometerUGen* unit, int inNumSamples) {
    // minval, maxval, warp, lag

    float minval = ZIN0(0);
    float maxval = ZIN0(1);
    float warp = ZIN0(2);
    float lag = ZIN0(3);

    float y1 = unit->m_y1;
    float b1 = unit->m_b1;

    if (lag != unit->m_lag) {
        unit->m_b1 = lag == 0.f ? 0.f : exp(log001 / (lag * unit->mRate->mSampleRate));
        unit->m_lag = lag;
    }
    float y0 = ([delegate accel_x] + 1.0f) * 0.5f;
    if (warp == 0.0) {
        y0 = (maxval - minval) * y0 + minval;
    } else {
        y0 = pow(maxval / minval, y0) * minval;
    }
    ZOUT0(0) = y1 = y0 + b1 * (y1 - y0);
    unit->m_y1 = zapgremlins(y1);
}

void AccelerometerX_Ctor(AccelerometerUGen* unit) {
    SETCALC(AccelerometerX_next);
    unit->m_b1 = 0.f;
    unit->m_lag = 0.f;
    AccelerometerX_next(unit, 1);
}


void AccelerometerY_next(AccelerometerUGen* unit, int inNumSamples) {
    // minval, maxval, warp, lag

    float minval = ZIN0(0);
    float maxval = ZIN0(1);
    float warp = ZIN0(2);
    float lag = ZIN0(3);

    float y1 = unit->m_y1;
    float b1 = unit->m_b1;

    if (lag != unit->m_lag) {
        unit->m_b1 = lag == 0.f ? 0.f : exp(log001 / (lag * unit->mRate->mSampleRate));
        unit->m_lag = lag;
    }
    float y0 = ([delegate accel_y] + 1.0f) * 0.5f;
    if (warp == 0.0) {
        y0 = (maxval - minval) * y0 + minval;
    } else {
        y0 = pow(maxval / minval, y0) * minval;
    }
    ZOUT0(0) = y1 = y0 + b1 * (y1 - y0);
    unit->m_y1 = zapgremlins(y1);
}

void AccelerometerY_Ctor(AccelerometerUGen* unit) {
    SETCALC(AccelerometerY_next);
    unit->m_b1 = 0.f;
    unit->m_lag = 0.f;
    AccelerometerY_next(unit, 1);
}

void AccelerometerZ_next(AccelerometerUGen* unit, int inNumSamples) {
    // minval, maxval, warp, lag

    float minval = ZIN0(0);
    float maxval = ZIN0(1);
    float warp = ZIN0(2);
    float lag = ZIN0(3);

    float y1 = unit->m_y1;
    float b1 = unit->m_b1;

    if (lag != unit->m_lag) {
        unit->m_b1 = lag == 0.f ? 0.f : exp(log001 / (lag * unit->mRate->mSampleRate));
        unit->m_lag = lag;
    }
    float y0 = ([delegate accel_z] + 1.0f) * 0.5f;
    if (warp == 0.0) {
        y0 = (maxval - minval) * y0 + minval;
    } else {
        y0 = pow(maxval / minval, y0) * minval;
    }
    ZOUT0(0) = y1 = y0 + b1 * (y1 - y0);
    unit->m_y1 = zapgremlins(y1);
}

void AccelerometerZ_Ctor(AccelerometerUGen* unit) {
    SETCALC(AccelerometerZ_next);
    unit->m_b1 = 0.f;
    unit->m_lag = 0.f;
    AccelerometerZ_next(unit, 1);
}

@implementation AccelerometerDelegate

- (id)init {
    if (self = [super init]) {
    }
    return self;
}

- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration {
    accel_x = acceleration.x;
    accel_y = acceleration.y;
    accel_z = acceleration.z;

    /*
        accel_x += ([acceleration x] - accel_x)*0.8f;
        accel_y += ([acceleration y] - accel_y)*0.8f;
        accel_z += ([acceleration z] - accel_z)*0.8f;
    */
}

- (float)accel_x {
    return (float)accel_x;
}

- (float)accel_y {
    return (float)accel_y;
}

- (float)accel_z {
    return (float)accel_z;
}

@end

PluginLoad(iPhone) {
    ft = inTable;

    delegate = [[AccelerometerDelegate alloc] init];
    UIAccelerometer* accel = [UIAccelerometer sharedAccelerometer];
    [accel setUpdateInterval:0.01f];
    [accel setDelegate:delegate];
    DefineUnit("AccelerometerX", sizeof(AccelerometerUGen), (UnitCtorFunc)&AccelerometerX_Ctor, 0, 0);
    DefineUnit("AccelerometerY", sizeof(AccelerometerUGen), (UnitCtorFunc)&AccelerometerY_Ctor, 0, 0);
    DefineUnit("AccelerometerZ", sizeof(AccelerometerUGen), (UnitCtorFunc)&AccelerometerZ_Ctor, 0, 0);
}

#endif
