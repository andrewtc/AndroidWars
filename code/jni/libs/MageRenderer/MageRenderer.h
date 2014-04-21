#pragma once

#include "Texture.h"
#include "Font.h"
#include "BitmapFont.h"

#define MAGE_USE_GLRENDERER

#include "Shader.h"
#include "Effect.h"
#include "Uniform.h"

#include "Surface.h"

#include "BloomEffect.h"

#include "RenderContext.h"
#include "GLRenderer/GLContext.h"
#ifdef ANDROID
#	include "GLRenderer/ESContext.h"
#endif
#include "Renderer.h"

#include "Stroke.h"

#include "ParticleManager.h"

#include "PointMass.h"
#include "Spring.h"
#include "SpringGrid.h"

#include "Camera.h"

// Scene
#include "DrawAxesNode.h"
#include "SpriteNode.h"
#include "CameraNode.h"
#include "DrawBoneNode.h"
#include "IKVisualizer.h"
#include "BasicNodeVisualizer.h"

#include "TileMap.h"
#include "Sprite.h"
#include "SpriteManager.h"
