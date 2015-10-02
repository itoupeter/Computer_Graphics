#include "camera.h"

#include <la.h>
#include <iostream>
#include <iomanip>

void printMat4( const glm::mat4 &m ){
    std::cout << std::endl;
    for( int i = 0; i < 4; ++i ){
        for( int j = 0; j < 4; ++j ){
            std::cout << std::setw( 4 ) << m[ i ][ j ] << " ";
        }
        std::cout << std::endl;
    }
}

Camera::Camera():
    Camera(400, 400)
{
//    look = glm::vec3(0,0,-1);
//    up = glm::vec3(0,1,0);
//    right = glm::vec3(1,0,0);
}

Camera::Camera(unsigned int w, unsigned int h):
    Camera(w, h, glm::vec3(0,0,10), glm::vec3(0,0,0), glm::vec3(0,1,0))
{}

Camera::Camera(unsigned int w, unsigned int h, const glm::vec3 &e, const glm::vec3 &r, const glm::vec3 &worldUp):
    fovy(45),
    width(w),
    height(h),
    near_clip(0.1f),
    far_clip(1000),
    eye(e),
    ref(r),
    world_up(worldUp)
{
    RecomputeAttributes();
}

Camera::Camera(const Camera &c):
    fovy(c.fovy),
    width(c.width),
    height(c.height),
    near_clip(c.near_clip),
    far_clip(c.far_clip),
    aspect(c.aspect),
    eye(c.eye),
    ref(c.ref),
    look(c.look),
    up(c.up),
    right(c.right),
    world_up(c.world_up),
    V(c.V),
    H(c.H)
{}

void Camera::CopyAttributes(const Camera &c)
{
    fovy = c.fovy;
    near_clip = c.near_clip;
    far_clip = c.far_clip;
    eye = c.eye;
    ref = c.ref;
    look = c.look;
    up = c.up;
    right = c.right;
    width = c.width;
    height = c.height;
    aspect = c.aspect;
    V = c.V;
    H = c.H;
}

void Camera::RecomputeAttributes()
{
    //---Q2---
    //TODO
    float fovyInRadians = glm::radians( fovy );

    look = glm::normalize( ref - eye );
    right = glm::normalize( glm::cross( look, world_up ) );
    up = glm::normalize( glm::cross( right, look ) );
    aspect = width * 1.f / height;

    float len = glm::length( ref - eye );

    V = len * tanf( fovyInRadians * .5f ) * up;
    H = len * aspect * tanf( fovyInRadians * .5f ) * right;
}

glm::mat4 Camera::getViewProj()
{
    return this->PerspectiveProjectionMatrix() * this->ViewMatrix();
}

glm::mat4 Camera::PerspectiveProjectionMatrix()
{
    //---Q1---
    //TODO
    static const float PI = 3.1415927f;
    float fovyInRadians = fovy * PI / 180;
    float top = near_clip * tan( fovyInRadians * .5f );
    float bottom = -top;
    float right = top * aspect;
    float left = -right;

    glm::mat4 result;

    result[ 0 ][ 0 ] = 2.f * near_clip / ( right - left );
    result[ 1 ][ 0 ] = 0;
    result[ 2 ][ 0 ] = -( right + left ) / ( right - left );
    result[ 3 ][ 0 ] = 0;

    result[ 0 ][ 1 ] = 0;
    result[ 1 ][ 1 ] = 2.f * near_clip / ( top - bottom );
    result[ 2 ][ 1 ] = -( top + bottom ) / ( top - bottom );
    result[ 3 ][ 1 ] = 0;

    result[ 0 ][ 2 ] = 0;
    result[ 1 ][ 2 ] = 0;
    result[ 2 ][ 2 ] = far_clip / ( far_clip - near_clip );
    result[ 3 ][ 2 ] = -far_clip * near_clip / ( far_clip - near_clip );

    result[ 0 ][ 3 ] = 0;
    result[ 1 ][ 3 ] = 0;
    result[ 2 ][ 3 ] = 1;
    result[ 3 ][ 3 ] = 0;

    return result;
}

glm::mat4 Camera::ViewMatrix()
{
    //---Q1---
    //TODO
    glm::mat4 result;

    result[ 0 ][ 0 ] = right[ 0 ];
    result[ 1 ][ 0 ] = right[ 1 ];
    result[ 2 ][ 0 ] = right[ 2 ];
    result[ 3 ][ 0 ] = -glm::dot( right, eye );

    result[ 0 ][ 1 ] = up[ 0 ];
    result[ 1 ][ 1 ] = up[ 1 ];
    result[ 2 ][ 1 ] = up[ 2 ];
    result[ 3 ][ 1 ] = -glm::dot( up, eye );

    result[ 0 ][ 2 ] = look[ 0 ];
    result[ 1 ][ 2 ] = look[ 1 ];
    result[ 2 ][ 2 ] = look[ 2 ];
    result[ 3 ][ 2 ] = -glm::dot( look, eye );

    result[ 0 ][ 3 ] = 0;
    result[ 1 ][ 3 ] = 0;
    result[ 2 ][ 3 ] = 0;
    result[ 3 ][ 3 ] = 1;

    return result;
}

void Camera::RotateAboutUp(float deg)
{
    deg *= DEG2RAD;
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), deg, up);
    ref = ref - eye;
    ref = glm::vec3(rotation * glm::vec4(ref, 1));
    ref = ref + eye;
    RecomputeAttributes();
}
void Camera::RotateAboutRight(float deg)
{
    deg *= DEG2RAD;
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), deg, right);
    ref = ref - eye;
    ref = glm::vec3(rotation * glm::vec4(ref, 1));
    ref = ref + eye;
    RecomputeAttributes();
}

void Camera::TranslateAlongLook(float amt)
{
    glm::vec3 translation = look * amt;
    eye += translation;
    ref += translation;
}

void Camera::TranslateAlongRight(float amt)
{
    glm::vec3 translation = right * amt;
    eye += translation;
    ref += translation;
}
void Camera::TranslateAlongUp(float amt)
{
    glm::vec3 translation = up * amt;
    eye += translation;
    ref += translation;
}

Ray Camera::Raycast(const glm::vec2 &pt)
{
    return Raycast(pt.x, pt.y);
}

Ray Camera::Raycast(float x, float y)
{
    //---Q4---
    //TODO
    float sx = x * 2.0 / width - 1.f;
    float sy = 1.f - y * 2.0 / height;

    return RaycastNDC( sx, sy );
}

Ray Camera::RaycastNDC(float ndc_x, float ndc_y)
{
    //---Q4---
    //TODO
    return Ray( eye, ref + ndc_x * H + ndc_y * V - eye );
}

void Camera::create()
{
    std::vector<glm::vec3> pos;
    std::vector<glm::vec3> col;
    std::vector<GLuint> idx;

    //0: Eye position
    pos.push_back(eye);
    //1 - 4: Near clip
        //Lower-left
        Ray r = this->RaycastNDC(-1,-1);
        pos.push_back(eye + r.direction * near_clip);
        //Lower-right
        r = this->RaycastNDC(1,-1);
        pos.push_back(eye + r.direction * near_clip);
        //Upper-right
        r = this->RaycastNDC(1,1);
        pos.push_back(eye + r.direction * near_clip);
        //Upper-left
        r = this->RaycastNDC(-1,1);
        pos.push_back(eye + r.direction * near_clip);
    //5 - 8: Far clip
        //Lower-left
        r = this->RaycastNDC(-1,-1);
        pos.push_back(eye + r.direction * far_clip);
        //Lower-right
        r = this->RaycastNDC(1,-1);
        pos.push_back(eye + r.direction * far_clip);
        //Upper-right
        r = this->RaycastNDC(1,1);
        pos.push_back(eye + r.direction * far_clip);
        //Upper-left
        r = this->RaycastNDC(-1,1);
        pos.push_back(eye + r.direction * far_clip);

    for(int i = 0; i < 9; i++){
        col.push_back(glm::vec3(1,1,1));
    }

    //Frustum lines
    idx.push_back(1);idx.push_back(5);
    idx.push_back(2);idx.push_back(6);
    idx.push_back(3);idx.push_back(7);
    idx.push_back(4);idx.push_back(8);
    //Near clip
    idx.push_back(1);idx.push_back(2);
    idx.push_back(2);idx.push_back(3);
    idx.push_back(3);idx.push_back(4);
    idx.push_back(4);idx.push_back(1);
    //Far clip
    idx.push_back(5);idx.push_back(6);
    idx.push_back(6);idx.push_back(7);
    idx.push_back(7);idx.push_back(8);
    idx.push_back(8);idx.push_back(5);

    //Camera axis
    pos.push_back(eye); col.push_back(glm::vec3(0,0,1)); idx.push_back(9);
    pos.push_back(eye + look); col.push_back(glm::vec3(0,0,1));idx.push_back(10);
    pos.push_back(eye); col.push_back(glm::vec3(1,0,0));idx.push_back(11);
    pos.push_back(eye + right); col.push_back(glm::vec3(1,0,0));idx.push_back(12);
    pos.push_back(eye); col.push_back(glm::vec3(0,1,0));idx.push_back(13);
    pos.push_back(eye + up); col.push_back(glm::vec3(0,1,0));idx.push_back(14);

    count = idx.size();

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(idx.data(), count * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(pos.data(), pos.size() * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(col.data(), col.size() * sizeof(glm::vec3));
}

GLenum Camera::drawMode(){return GL_LINES;}
