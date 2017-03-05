#pragma once

#include "Graphics.h"

namespace Resources {
	extern Graphics::MyGeometry aventadorBody, aventadorBodyGlow, aventadorUnder, aventadorWheel, aventadorWheelGlow;
	extern Graphics::MyGeometry plane, cube, centeredCube;
	extern Graphics::MyShader standardShader;

	extern Graphics::StandardShaderMaterial defaultMaterialObject;
	extern Graphics::StandardShaderMaterial emmisiveMaterialObject;
	extern Graphics::StandardShaderMaterial blackMaterialObject;

	void init();

	void nullMaterial();

	void defaultMaterial();

	void darkGreyMaterial();

	void pureBlackMaterial();

	void emmisiveBlueMaterial();

	void emmisiveMaterial();
}