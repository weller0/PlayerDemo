#include "bean/region.h"

Region::Region() {
    degreeX = new _Region();
    degreeY = new _Region();
    degreeZ = new _Region();
    scale = new _Region();
    fov = new _Region();
    lookAtCenterZ = new _Region();
}

Region::~Region() {
    delete degreeX;
    delete degreeY;
    delete degreeZ;
    delete scale;
    delete fov;
    delete lookAtCenterZ;
}

void Region::setDefaultFov(GLfloat min, GLfloat max, GLfloat def) {
    fov->maxSetUpValue = max;
    fov->minSetUpValue = min;
    fov->maxActualValue = max;
    fov->minActualValue = min;
    fov->defaultValue = def;
    fov->value = def;
}

void Region::setDefaultX(GLfloat min, GLfloat max, GLfloat def) {
    degreeX->maxSetUpValue = max;
    degreeX->minSetUpValue = min;
    degreeX->maxActualValue = max;
    degreeX->minActualValue = min;
    degreeX->defaultValue = def;
    degreeX->value = def;
}

void Region::setDefaultY(GLfloat min, GLfloat max, GLfloat def) {
    degreeY->maxSetUpValue = max;
    degreeY->minSetUpValue = min;
    degreeY->maxActualValue = max;
    degreeY->minActualValue = min;
    degreeY->defaultValue = def;
    degreeY->value = def;
}

void Region::setDefaultZ(GLfloat min, GLfloat max, GLfloat def) {
    degreeZ->maxSetUpValue = max;
    degreeZ->minSetUpValue = min;
    degreeZ->maxActualValue = max;
    degreeZ->minActualValue = min;
    degreeZ->defaultValue = def;
    degreeZ->value = def;
}

void Region::setDefaultScale(GLfloat min, GLfloat max, GLfloat def) {
    scale->maxSetUpValue = max;
    scale->minSetUpValue = min;
    scale->maxActualValue = max;
    scale->minActualValue = min;
    scale->defaultValue = def;
    scale->value = def;
}

void Region::setDefaultCenterZ(GLfloat min, GLfloat max, GLfloat def) {
    lookAtCenterZ->maxSetUpValue = max;
    lookAtCenterZ->minSetUpValue = min;
    lookAtCenterZ->maxActualValue = max;
    lookAtCenterZ->minActualValue = min;
    lookAtCenterZ->defaultValue = def;
    lookAtCenterZ->value = def;
}

void Region::reset() {
    degreeX->value = degreeX->defaultValue;
    degreeY->value = degreeY->defaultValue;
    degreeZ->value = degreeZ->defaultValue;
    fov->value = fov->defaultValue;
    scale->value = scale->defaultValue;
    lookAtCenterZ->value = lookAtCenterZ->defaultValue;
}

void Region::limit(TransformBean *bean) {
    setFov(bean->fov, 1.7777778);
    setDegreeX(bean->degreeX);
    setDegreeY(bean->degreeY);
    setDegreeZ(bean->degreeZ);
    setScale(bean->scale);
    setCenterZ(bean->lookAtCenterZ);
    bean->fov = fov->value;
    bean->degreeX = degreeX->value;
    bean->degreeY = degreeY->value;
    bean->degreeZ = degreeZ->value;
    bean->scale = scale->value;
    bean->lookAtCenterZ = lookAtCenterZ->value;
}

void Region::setDegreeX(GLfloat x) {
    if (degreeX->maxActualValue > degreeX->minActualValue) {
        if (x > degreeX->maxActualValue) {
            x = degreeX->maxActualValue;
        } else if (x < degreeX->minActualValue) {
            x = degreeX->minActualValue;
        }
    }
    degreeX->value = x;
}

void Region::setDegreeY(GLfloat y) {
    if (degreeY->maxActualValue > degreeY->minActualValue) {
        if (y > degreeY->maxActualValue) {
            y = degreeY->maxActualValue;
        } else if (y < degreeY->minActualValue) {
            y = degreeY->minActualValue;
        }
    }
    degreeY->value = y;
}

void Region::setDegreeZ(GLfloat z) {
    if (degreeZ->maxActualValue > degreeZ->minActualValue) {
        if (z > degreeZ->maxActualValue) {
            z = degreeZ->maxActualValue;
        } else if (z < degreeZ->minActualValue) {
            z = degreeZ->minActualValue;
        }
    }
    degreeZ->value = z;
}

void Region::setFov(GLfloat fovValue, GLfloat ratio) {
    if (fov->maxActualValue > fov->minActualValue) {
        if (fovValue > fov->maxActualValue) {
            fovValue = fov->maxActualValue;
        } else if (fovValue < fov->minActualValue) {
            fovValue = fov->minActualValue;
        }
    }
    fov->value = fovValue;

    if (degreeX->maxActualValue > degreeX->minActualValue) {
        degreeX->maxActualValue = degreeX->maxSetUpValue - fov->value * ratio / 2;
        degreeX->minActualValue = degreeX->minSetUpValue + fov->value * ratio / 2;

    }
    if (degreeY->maxActualValue > degreeY->minActualValue) {
        degreeY->maxActualValue = degreeY->maxSetUpValue - fov->value / 2;
        degreeY->minActualValue = degreeY->minSetUpValue + fov->value / 2;
    }
}

void Region::setScale(GLfloat s) {
    if (scale->maxActualValue > scale->minActualValue) {
        if (s > scale->maxActualValue) {
            s = scale->maxActualValue;
        } else if (s < scale->minActualValue) {
            s = scale->minActualValue;
        }
    }
    scale->value = s;
}

void Region::setCenterZ(GLfloat z) {
    if (lookAtCenterZ->maxActualValue > lookAtCenterZ->minActualValue) {
        if (z > lookAtCenterZ->maxActualValue) {
            z = lookAtCenterZ->maxActualValue;
        } else if (z < lookAtCenterZ->minActualValue) {
            z = lookAtCenterZ->minActualValue;
        }
    }
    lookAtCenterZ->value = z;
}