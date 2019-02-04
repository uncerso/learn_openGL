#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CustomTimer.h"

class OpenGLDrawer
	: private OpenGLRenderer
{
public:
	OpenGLDrawer(OpenGLContext * openGLContext);
	~OpenGLDrawer() noexcept = default;

//	void setBounds(int shiftFromLeftSide, int shiftFromBottomSide, float parentWidthInPixels, float parentHeightInPixels) noexcept;

private:
	OpenGLContext * openGLContext;
	GLuint vao;

	std::unique_ptr<OpenGLShaderProgram> shader;
	std::unique_ptr<OpenGLShaderProgram::Attribute> position;
	std::unique_ptr<OpenGLShaderProgram::Attribute> shift;
	std::unique_ptr<OpenGLShaderProgram::Uniform> alpha;

	unsigned int frameCounter = 0;


	void createShaders();

	void newOpenGLContextCreated() override;
	void renderOpenGL() override;
	void openGLContextClosing() noexcept override;

	void genVao() noexcept;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenGLDrawer)
};
