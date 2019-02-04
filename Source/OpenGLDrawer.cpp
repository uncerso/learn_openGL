#include "OpenGLDrawer.h"

auto glGenVertexArrays = (void(*)(GLsizei n, const GLuint *arrays))OpenGLHelpers::getExtensionFunction("glGenVertexArrays");
auto glBindVertexArray = (void(*)(GLuint p1))OpenGLHelpers::getExtensionFunction("glBindVertexArray");
constexpr auto pi = 3.14159265358979323846264338327950;

OpenGLDrawer::OpenGLDrawer(OpenGLContext * openGLContext)
	: openGLContext(openGLContext)
{
	jassert(openGLContext != nullptr);
	openGLContext->setRenderer(this);
	openGLContext->setContinuousRepainting(true);
}

#include <iostream>

void OpenGLDrawer::newOpenGLContextCreated() {
	createShaders();
	genVao();
}

void OpenGLDrawer::openGLContextClosing() noexcept {
	shader.reset();
}

// void OpenGLDrawer::setBounds(int shiftFromLeftSide, int shiftFromBottomSide, float parentWidthInPixels, float parentHeightInPixels) noexcept {
// 	shiftFromLeftSideInPixels = shiftFromLeftSide;
// 	shiftFromBottomSideInPixels = shiftFromBottomSide;
// 	glViewWidthInPixels = parentWidthInPixels;
// 	glViewHeightInPixels = parentHeightInPixels;
// }

void OpenGLDrawer::genVao() noexcept {
	constexpr auto T = pi/2;
	GLfloat vertices[] = {
		 0.5f,  0.5f, 0.0f, 0*T, 
		 0.5f, -0.5f, 0.0f, 1*T,
		-0.5f, -0.5f, 0.0f, 2*T,
		-0.5f,  0.5f, 0.0f, 3*T
	};
	GLuint indexes[] = {
		0, 1, 3,
		1, 2, 3
	};  
	GLuint vbo, ebo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	openGLContext->extensions.glGenBuffers(1, &vbo);
	openGLContext->extensions.glBindBuffer(GL_ARRAY_BUFFER, vbo);
	openGLContext->extensions.glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	openGLContext->extensions.glGenBuffers(1, &ebo);
	openGLContext->extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	openGLContext->extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

	openGLContext->extensions.glVertexAttribPointer(position->attributeID, 3, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (GLvoid*)0);
	openGLContext->extensions.glEnableVertexAttribArray(position->attributeID);

	openGLContext->extensions.glVertexAttribPointer(shift->attributeID, 1, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	openGLContext->extensions.glEnableVertexAttribArray(shift->attributeID);

	glBindVertexArray(0);
}

void OpenGLDrawer::renderOpenGL() {
	jassert(openGLContext->isActive());

	OpenGLHelpers::clear(Colour::fromFloatRGBA(0.2, 0.3, 0.3, 1.0));
	
	shader->use();

	openGLContext->extensions.glUniform1f(alpha->uniformID, frameCounter * 0.01);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	++frameCounter;
}

#include <iostream>

void OpenGLDrawer::createShaders() {
	constexpr auto vertexShaderSource = 
		#include "shaders/vertexShader.glsl"
		;
	auto fragmentShaderSource = 
		#include "shaders/fragmentShader.glsl"
		;

	auto newShader = std::make_unique<OpenGLShaderProgram>(*openGLContext);
	if (newShader->addVertexShader(OpenGLHelpers::translateVertexShaderToV3(vertexShaderSource))
		&& newShader->addFragmentShader(OpenGLHelpers::translateFragmentShaderToV3(fragmentShaderSource))
		&& newShader->link()) {
		shader.reset(newShader.release());
		shader->use();

		jassert(openGLContext->extensions.glGetAttribLocation(shader->getProgramID(), "position") >= 0);
		position.reset(new OpenGLShaderProgram::Attribute(*shader, "position"));

		jassert(openGLContext->extensions.glGetAttribLocation(shader->getProgramID(), "shift") >= 0);
		shift.reset(new OpenGLShaderProgram::Attribute(*shader, "shift"));

		jassert(openGLContext->extensions.glGetUniformLocation(shader->getProgramID(), "alpha") >= 0);
		alpha.reset(new OpenGLShaderProgram::Uniform(*shader, "alpha"));
	}
	else
		std::cerr << newShader->getLastError();
}
