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

    uMVMatrix.setToIdentity();
    uMVMatrix.lookAt(eye, center, up);

    // modelviewInfo();
}

void Camera::moveForward(double distance) {
    QVector3D diff = look.normalized() * distance;
    eye += diff;
    center += diff;
    uMVMatrix.setToIdentity();
    uMVMatrix.lookAt(eye, center, up);

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
    
    // modelviewInfo();
}

void Camera::turnRight(double angle, QVector3D axis) {
    turnLeft(-angle, axis);
}

void Camera::setLeft(double left) { 
    this->left = left; 
    uPMatrix.setToIdentity();
    uPMatrix.frustum(left, right, bottom, top, near, far); 
}

void Camera::setRight(double right) { 
    this->right = right; 
    uPMatrix.setToIdentity();
    uPMatrix.frustum(left, right, bottom, top, near, far); 
}

void Camera::setbottom(double bottom) { 
    this->bottom = bottom; 
    uPMatrix.setToIdentity();
    uPMatrix.frustum(left, right, bottom, top, near, far); 
}

void Camera::setTop(double top) {
    this->top = top; 
    uPMatrix.setToIdentity();
    uPMatrix.frustum(left, right, bottom, top, near, far); 
}

void Camera::setNear(double near) { 
    this->near = near; 
    uPMatrix.setToIdentity();
    uPMatrix.frustum(left, right, bottom, top, near, far); 
}

void Camera::setFar(double far) { 
    this->far = far; 
    uPMatrix.setToIdentity();
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
    uPMatrix.setToIdentity();
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
    
    // projectionInfo();
}

void Camera::uniformMatrices(QOpenGLShaderProgram &program) {
    // uniform matrices
    program.setUniformValue("uMVMatrix", uMVMatrix);
    program.setUniformValue("uPMatrix", uPMatrix);
    program.setUniformValue("uNMatrix", uNMatrix);
}

Camera::Cullable Camera::isCullable(BoundingBox box) {
    // Not implemented yet
    // This method will be later be used in terrain culling
    return Camera::Cullable::NOT_CULLABLE;
}
