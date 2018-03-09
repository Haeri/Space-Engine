#include "../Core/Systems/Lighting.h"
#include "../Resources/ForwardAmbientLightShader.h"
#include "AmbientLight.h"

AmbientLight::AmbientLight()
{
	Lighting::addLight(this);
}

void AmbientLight::destroy()
{
	Lighting::removeLight(this);
}

Shader * AmbientLight::getLightShader()
{
	return ForwardAmbientLightShader::getInstance();
}