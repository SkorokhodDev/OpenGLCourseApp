#pragma once

#include "ShadowMap.h"

class OmniShadowMap : public ShadowMap
{
public:
	
	OmniShadowMap();

	virtual bool init(GLuint width, GLuint height) override;

	virtual void Write() override;
	virtual void Read(GLenum textureUnit) override;
};

