#include <Camera.h>

Camera::Camera(QString n) : name(n) {
    // Initialize the matrices
    uMVMatrix.setToIdentity();
    uPMatrix.setToIdentity();
    uNMatrix.setToIdentity();
}

Camera::~Camera() {
}

void Camera::initialize() {
}

void Camera::setLook(QVector3D look) {
    this->look = look;
    this->direction = look - center;
    uMVMatrix.lookAt(look, center, up);
}

void Camera::setCenter(QVector3D center) {
    this->center = center;
    this->direction = look - center;
    uMVMatrix.lookAt(look, center, up);
}

void Camera::setUp(QVector3D up) {
    this->up = up;
    uMVMatrix.lookAt(look, center, up);
}

void Camera::lookAt(QVector3D look, QVector3D center, QVector3D up) {
    this->look = look;
    this->center = center;
    this->up = up;
    this->direction = look - center;
    uMVMatrix.lookAt(look, center, up);
}

void Camera::moveForward(double distance) {
    QVector3D diff = direction.normalized() * distance;
    center += diff;
    look += diff;
}

void Camera::moveBackward(double distance) {
    moveForward(-distance);
}

void Camera::turnLeft(double angle, QVector3D axis) {
    double radian = qDegreesToRadians(angle);
    QQuaternion q = QQuaternion(radian, axis);
    direction = q.rotatedVector(direction);
    look = center + direction;
    uMVMatrix.lookAt(look, center, up);
}

void Camera::turnRight(double angle, QVector3D axis) {
    turnLeft(-angle, axis);
}

void Camera::setLeft(double left) { 
    this->left = left; 
    uPMatrix.frustum(left, right, bottom, top, near, far); 
}

void Camera::setRight(double right) { 
    this->right = right; 
    uPMatrix.frustum(left, right, bottom, top, near, far); 
}

void Camera::setbottom(double bottom) { 
    this->bottom = bottom; 
    uPMatrix.frustum(left, right, bottom, top, near, far); 
}

void Camera::setTop(double top) {
    this->top = top; 
    uPMatrix.frustum(left, right, bottom, top, near, far); 
}

void Camera::setNear(double near) { 
    this->near = near; 
    uPMatrix.frustum(left, right, bottom, top, near, far); 
}

void Camera::setFar(double far) { 
    this->far = far; 
    uPMatrix.frustum(left, right, bottom, top, near, far); 
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
    uPMatrix.frustum(left, right, bottom, top, near, far);
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

    double halfHeight = near * qTan(qDegreesToRadians(fovy * 0.5));
    double halfWidth = halfHeight * aspect;

    left = -halfWidth;
    right = halfWidth;
    bottom = -halfHeight;
    bottom = +halfHeight;
    near = zNear;
    far = zFar;

    uPMatrix.frustum(left, right, bottom, top, near, far); 
}

void Camera::uniformMatrices(QOpenGLShaderProgram &program) {
    // uniform modelview matrix
    program.setUniformValue("uMVMatrix", uMVMatrix);
    // uniform project matrix
    program.setUniformValue("uPMatrix", uPMatrix);
    // uniform normal matrix
    program.setUniformValue("uNMatrix", uNMatrix);
}

Camera::Cullable Camera::isCullable(BoundingBox box) {
    // Not implemented yet
    // This method will be later be used in terrain culling
    return Camera::Cullable::NOT_CULLABLE;
}
