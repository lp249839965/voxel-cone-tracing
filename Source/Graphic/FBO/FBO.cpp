

#include "FBO.h"
#include "Texture.h"
#include "OpenGL_Includes.h"


GLint FBO::setupRenderTarget(Texture* target)
{
    target->SetWrap(textureProperties.wrap);
    
    target->SetWidth(dimensions.width);
    target->SetHeight(dimensions.height);
    target->SetMinFilter(textureProperties.minFilter);
    target->SetMagFilter(textureProperties.magFilter);
    target->SetPixelFormat(textureProperties.pixelFormat);
    target->SetDataType(textureProperties.dataFormat);
    
    target->SaveTextureState();
    
    return target->GetTextureID();
}

void FBO::ClearRenderTextures()
{
    for(Texture* texture : renderTextures)
    {
        texture->Clear();
    }
}

FBO::~FBO()
{
    for(Texture* texture: renderTextures)
    {
        delete texture;
    }
}

///////////////////////////////////////////////////FBO::Commands

FBO* FBO::Commands::fbo = nullptr;

FBO::Commands::Commands(FBO* _fbo):
previousViewportWidth(0),
previousViewportHeight(0)
{
    assert(fbo == nullptr && "You must call FBO::Commands::end() to indicate end of frame rendering before starting a new frame commands");
    fbo = _fbo;
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->frameBuffer);
    
    GLuint colorAttachment[50];
    
    if(fbo->renderTextures.size() > 0)
    {
        for(GLint i = 0; i < fbo->renderTextures.size(); ++i)
        {
            colorAttachment[i] = GL_COLOR_ATTACHMENT0 + i;
        }

        glDrawBuffers(fbo->renderTextures.size(), colorAttachment);
    }

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    getPreviousViewportSize();
    
    fbo->dimensions.width = fbo->dimensions.width == 0 ? previousViewportWidth : fbo->dimensions.width;
    fbo->dimensions.height= fbo->dimensions.height == 0 ? previousViewportHeight : fbo->dimensions.height;
    
    setViewport(fbo->dimensions.width, fbo->dimensions.height);
}

void FBO::Commands::setViewport(GLint width, GLint height)
{
     glViewport(0, 0, width, height);
    
}

void FBO::Commands::getPreviousViewportSize()
{
    GLint dims[4];
    glGetIntegerv(GL_VIEWPORT, dims);
    previousViewportWidth = dims[2];
    previousViewportHeight = dims[3];
}

void FBO::Commands::colorMask(bool _value)
{
    GLboolean value = _value ? GL_TRUE : GL_FALSE;
    glColorMask(value, value, value, value);
}

void FBO::Commands::depthMask(bool _value)
{
    GLboolean value = _value ? GL_TRUE : GL_FALSE;
    glDepthMask(value);
}

void FBO::Commands::activateCulling(bool value)
{
    value ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}

void FBO::Commands::enableDepthTest(bool value)
{
    value ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void FBO::Commands::enableBlend(bool value)
{
    value ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
}

void FBO::Commands::enableAdditiveBlending()
{
    enableBlend(true);
    glBlendFunc(GL_ONE, GL_ONE);
}

void FBO::Commands::backFaceCulling(bool _value)
{
    if(_value)
    {
        glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
}

void FBO::Commands::end()
{
    glBindFramebuffer(GL_FRAMEBUFFER, DEFAULT_FRAMEBUFFER);
    setViewport(previousViewportWidth, previousViewportHeight);
    fbo = nullptr;
}

void FBO::Commands::clearRenderTarget()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
}

void FBO::Commands::setClearColor(glm::vec4 color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void FBO::Commands::setDetphClearValue(GLfloat depth)
{
    glClearDepth(depth);
}

void FBO::Commands::blendSrcAlphaOneMinusSrcAlpha()
{
    enableBlend(true);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

FBO::Commands::~Commands()
{
    assert(fbo == nullptr &&
           "You must call end() to signal end of frame rendering before FBO::Commands object gets destroyed");
}





