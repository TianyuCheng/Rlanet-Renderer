#include <Camera.h>

Camera::Camera(QString n) : name(n), dirty(true) {
    // Initialize the matrices
    uMVMatrix.setToIdentity();
    uPMatrix.setToIdentity();
    uNMatrix.setToIdentity();
}

Camera::~Camera() {
}

void Camera::initialize() {
}

void Camera::setCenter(QVector3D l) {
    this->lookAt(eye, l, up);
}

void Camera::setEye(QVector3D e) {
    this->lookAt(e, center, up);
}

void Camera::setUp(QVector3D u) {
    this->lookAt(eye, center, u);
}

void Camera::lookAt(QVector3D eye, QVector3D center, QVector3D up) {
    this->eye = eye;
    this->center = center;
    this->up = up;
    this->look = center - eye;

    this->up.normalize();

    uMVMatrix.setToIdentity();
    uMVMatrix.lookAt(eye, center, up);

    dirty = true;

    // modelviewInfo();
}

void Camera::moveForward(double distance) {
    QVector3D diff = look.normalized() * distance;
    eye += diff;
    center += diff;
    uMVMatrix.setToIdentity();
    uMVMatrix.lookAt(eye, center, up);

    dirty = true;
    // modelviewInfo();
}

void Camera::moveBackward(double distance) {
    moveForward(-distance);
}

void Camera::turnLeft(double angle, QVector3D axis) {
    QQuaternion q = QQuaternion::fromAxisAndAngle(axis, angle);
    look = q.rotatedVector(look);
    center = eye + look;
    uMVMatrix.setToIdentity();
    uMVMatrix.lookAt(eye, center, up);
    
    dirty = true;
    // modelviewInfo();
}

void Camera::turnRight(double angle, QVector3D axis) {
    turnLeft(-angle, axis);
}

void Camera::setLeft(double left) { 
    this->left = left; 
    uPMatrix.setToIdentity();
    uPMatrix.frustum(left, right, bottom, top, near, far); 
    dirty = true;
}

void Camera::setRight(double right) { 
    this->right = right; 
    uPMatrix.setToIdentity();
    uPMatrix.frustum(left, right, bottom, top, near, far); 
    dirty = true;
}

void Camera::setbottom(double bottom) { 
    this->bottom = bottom; 
    uPMatrix.setToIdentity();
    uPMatrix.frustum(left, right, bottom, top, near, far); 
    dirty = true;
}

void Camera::setTop(double top) {
    this->top = top; 
    uPMatrix.setToIdentity();
    uPMatrix.frustum(left, right, bottom, top, near, far); 
    dirty = true;
}

void Camera::setNear(double near) { 
    this->near = near; 
    uPMatrix.setToIdentity();
    uPMatrix.frustum(left, right, bottom, top, near, far); 
    dirty = true;
}

void Camera::setFar(double far) { 
    this->far = far; 
    uPMatrix.setToIdentity();
    uPMatrix.frustum(left, right, bottom, top, near, far); 
    dirty = true;
}

void Camera::setFrustum(double left,    double right,
                double bottom,  double top,
                double near,    double far) {
    this->left = left; 
    this->right = right;
    this->bottom = bottom; 
    this->top = top;
    this->near = near; 
    this->far = far;
    uPMatrix.setToIdentity();
    uPMatrix.frustum(left, right, bottom, top, near, far);
    dirty = true;
}

void Camera::setFOV(double fov) {
    setPerspective(
            fov, 
            double((right - left) / (top - bottom)), 
            near, far);
}

void Camera::setAspect(double aspect) {
    setPerspective(
            2 * qAtan(0.5 * (top - bottom) / near),
            aspect,
            near, far);
}

void Camera::setPerspective(double fovy, double aspect, double zNear, double zFar) {

    double halfHeight = zNear * qTan(qDegreesToRadians(fovy * 0.5));
    double halfWidth = halfHeight * aspect;

    left = -halfWidth;
    right = halfWidth;
    bottom = -halfHeight;
    top = +halfHeight;
    near = zNear;
    far = zFar;

    uPMatrix.setToIdentity();
    uPMatrix.frustum(left, right, bottom, top, near, far); 
    
    dirty = true;
    // projectionInfo();
}

void Camera::uniformMatrices(QOpenGLShaderProgram &program) {
    // uniform matrices
    program.setUniformValue("uMVMatrix", uMVMatrix);
    program.setUniformValue("uPMatrix", uPMatrix);
    program.setUniformValue("uNMatrix", uNMatrix);
}

Camera::Cullable Camera::isCullable(BoundingBox box) {

    // If the frustum has been changed, update it
    if (dirty) updateFrustum();

    QVector3D* corners = box.getCorners();

    int count = 8;

    /**
     * The idea is simple, you go through all corners,
     * and check whether that corner is behind all six planes
     * of the frustum.
     * If yes, then the corner is in the frustum.
     * If no, then the corner is not in the frustum.
     *
     * NOT CULLABLE = number of corners in frustum == 8
     * PARTIALLY_CULLABLE = 0 < number of corners in frustum < 8
     * TOTALLY_CULLABLE = number of corners in frustum == 0
     * */

    // Iterate through every corner
    for (int i = 0; i < 8; i++) {
        QVector3D corner = corners[i];

        // Test with all planes of frustums
        for (int j = 0; j < 6; j++) {
            QPair<QVector3D, QVector3D> plane = frustum[j];
            if (QVector3D::dotProduct(plane.first, corner - plane.second) > 0) {  // out
                count--;
                break;
            }
        }

    }

    if (count == 8) return Camera::Cullable::NOT_CULLABLE;

    if (count > 0) return Camera::Cullable::PARTIALLY_CULLABLE;

    return Camera::Cullable::TOTALLY_CULLABLE;
}


void Camera::updateFrustum() {
    QVector3D corners[8];

    QVector3D normalizedDir = look.normalized();
    QVector3D center = eye + normalizedDir * near;

    QVector3D baseX = QVector3D::crossProduct(normalizedDir, up).normalized();
    QVector3D baseY = up;

    /**
     * 1------0
     * |      | corners in the near plane
     * 2------3
     * */
    corners[0] = center + 0.5 * (right - left) * baseX + 0.5 * (top - bottom) * baseY;
    corners[1] = center - 0.5 * (right - left) * baseX + 0.5 * (top - bottom) * baseY;
    corners[2] = center - 0.5 * (right - left) * baseX - 0.5 * (top - bottom) * baseY;
    corners[3] = center + 0.5 * (right - left) * baseX - 0.5 * (top - bottom) * baseY;

    center = eye + normalizedDir * far;
    baseX *= far / near;
    baseY *= far / near;

    /**
     * 5------4
     * |      | corners in the far plane
     * 6------7
     * */
    corners[4] = center + 0.5 * (right - left) * baseX + 0.5 * (top - bottom) * baseY;
    corners[5] = center - 0.5 * (right - left) * baseX + 0.5 * (top - bottom) * baseY;
    corners[6] = center - 0.5 * (right - left) * baseX - 0.5 * (top - bottom) * baseY;
    corners[7] = center + 0.5 * (right - left) * baseX - 0.5 * (top - bottom) * baseY;

    frustum.clear();

    // near plane and far plane
    frustum << qMakePair(-normalizedDir, corners[0]);
    frustum << qMakePair(normalizedDir, corners[4]);

    // left plane and right plane
    QVector3D leftNormal = QVector3D::crossProduct(corners[5] - corners[1], corners[2] - corners[1]);
    QVector3D rightNormal = QVector3D::crossProduct(corners[7] - corners[3], corners[0] - corners[3]);
    frustum << qMakePair(leftNormal, corners[1]);
    frustum << qMakePair(rightNormal, corners[0]);

    // top plane and bottom plane
    QVector3D topNormal = QVector3D::crossProduct(corners[4] - corners[0], corners[1] - corners[0]);
    QVector3D bottomNormal = QVector3D::crossProduct(corners[6] - corners[2], corners[3] - corners[2]);
    frustum << qMakePair(topNormal, corners[0]);
    frustum << qMakePair(bottomNormal, corners[2]);

    dirty = false;
}
