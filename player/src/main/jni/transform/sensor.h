#ifndef SENSOR_H
#define SENSOR_H

#include <GLES3/gl3.h>
#include <math.h>
#include <stdlib.h>
#include "bean/bean_base.h"
#include "log.h"

class Sensor {
public:
    Sensor(SettingsBean *settingsBean);

    ~Sensor();

    void onSensor(TransformBean *bean, GLfloat x, GLfloat y, GLfloat z, GLuint64 timestamp);

private:
    SettingsBean *mSettingsBean;
    GLuint64 mLastTime;

};

#endif //SENSOR_H
