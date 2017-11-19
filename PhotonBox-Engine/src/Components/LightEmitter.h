#ifndef LIGHT_EMITTER_H
#define LIGHT_EMITTER_H

class Shader;
#include "../Core/Component.h"
#include "../Math/Vector3f.h"

class LightEmitter : public Component {
public:
	Vector3f color;
	float intensity;

	virtual Shader* getLightShader();

	//LightEmitter();

	//void destroy() override;
};

#endif // LIGHT_EMITTER_H
