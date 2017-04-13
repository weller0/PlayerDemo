#include "transform/sensor.h"

Sensor::Sensor(SettingsBean *settingsBean) {
    mSettingsBean = settingsBean;
}

Sensor::~Sensor() {

}

void Sensor::onSensor(TransformBean *bean, GLfloat x, GLfloat y, GLfloat z, GLuint64 timestamp) {
    if(mSettingsBean->mCtrlStyle != CS_SENSOR) return;
    if (mLastTime == 0) {
        mLastTime = timestamp;
        return;
    }
    GLfloat timeSpan = (timestamp - mLastTime) * 1.0 / 1000000000;

    bean->degreeX -= TO_DEGREES(x * timeSpan);
    bean->degreeY -= TO_DEGREES(y * timeSpan);
    bean->degreeZ -= TO_DEGREES(z * timeSpan);

    mLastTime = timestamp;
}