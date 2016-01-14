
#include "la.h"
#include "openGL/canvas.h"
#include <QOpenGLTexture>

Canvas::Canvas(){

    initShader();
    initVBO();
}

void Canvas::initShader(){

    program.addShaderFromSourceFile( QOpenGLShader::Vertex  , ":/glsl/progressive.vert.glsl" );
    program.addShaderFromSourceFile( QOpenGLShader::Fragment, ":/glsl/progressive.frag.glsl" );
    program.link();

    attribute_position = program.attributeLocation( "vs_position" );
    attribute_texcoord = program.attributeLocation( "vs_texcoord" );
    uniform_texture = program.uniformLocation( "mytexture" );

}

void Canvas::initVBO(){

    static const glm::vec3 position[]{
        { -1.f, -1.f, .5f },
        { 1.f, -1.f, .5f },
        { 1.f, 1.f, .5f },
        { -1.f, 1.f, .5f },
    };

    static const glm::vec2 texcoord[]{
        { 0.f, 0.f },
        { 1.f, 0.f },
        { 1.f, 1.f },
        { 0.f, 1.f },
    };

    static const unsigned index[]{
        0, 2, 1,
        0, 2, 3,
    };

    vbo_position.create();
    vbo_position.bind();
    vbo_position.setUsagePattern( QOpenGLBuffer::StaticDraw );
    vbo_position.allocate( position, sizeof position );

    vbo_texcoord.create();
    vbo_texcoord.bind();
    vbo_texcoord.setUsagePattern( QOpenGLBuffer::StaticDraw );
    vbo_texcoord.allocate( texcoord, sizeof texcoord );

    vbo_index.create();
    vbo_index.bind();
    vbo_index.setUsagePattern( QOpenGLBuffer::StaticDraw );
    vbo_index.allocate( index, sizeof index );

}

void Canvas::draw( GLWidget277 *f ){

    program.bind();
    program.setUniformValue( uniform_texture, 0 );

    QOpenGLTexture texture( QImage( ":/../wahoo.jpg" ) );

    texture.setMinificationFilter( QOpenGLTexture::LinearMipMapLinear );
    texture.setMagnificationFilter( QOpenGLTexture::Linear );
    texture.bind();
    f->glActiveTexture( GL_TEXTURE0 );

    vbo_position.bind();
    program.enableAttributeArray( attribute_position );
    f->glVertexAttribPointer( attribute_position, 3, GL_FLOAT, false, 0, NULL );

    vbo_texcoord.bind();
    program.enableAttributeArray( attribute_texcoord );
    f->glVertexAttribPointer( attribute_texcoord, 2, GL_FLOAT, false, 0, NULL );

    vbo_index.bind();
    f->glDrawElements( GL_TRIANGLES, 2, GL_UNSIGNED_INT, 0 );

    f->glDisableVertexAttribArray( attribute_texcoord );
    f->glDisableVertexAttribArray( attribute_position );

    texture.destroy();
}
