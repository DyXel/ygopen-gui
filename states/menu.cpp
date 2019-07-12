#include "menu.hpp"
#include "../drawing/gl_mesh.hpp"
#include "../drawing/gl_shader_and_program.hpp"

namespace YGOpen
{
namespace State
{

static const GLchar* vertex_shader_source =
"#version 100\n"
"attribute vec3 pos;\n"
"attribute vec4 color;\n"
"varying vec4 fsColor;\n"
"void main() {\n"
"   gl_Position = vec4(pos, 1.0);\n"
"   fsColor = color;\n"
"}\n";
static const GLchar* fragment_shader_source =
"#version 100\n"
"precision mediump float;\n"
"varying vec4 fsColor;\n"
"void main() {\n"
"   gl_FragColor = fsColor;\n"
"}\n";

Menu::Menu()
{
	// Create program (vertex shader + fragment shader)
	program = std::make_shared<OGL::Program>();
	{
		OGL::Shader vs(GL_VERTEX_SHADER, vertex_shader_source);
		OGL::Shader fs(GL_FRAGMENT_SHADER, fragment_shader_source);
		program->Attach(vs).Attach(fs).Link();
	}
	
	// Create circle
	circle = std::make_shared<OGL::Mesh>();
	OGL::Mesh::VerticesContainer vertices;
	OGL::Mesh::ColorsContainer colors;
	const float full = 2.0f * M_PI;
	for(float i = 0.0f; i < full; i += full / 100.0f)
	{
		vertices.push_back({std::sin(i), std::cos(i), 0.0f});
		colors.push_back({1.0f, 0.0f, 0.0f, 1.0f});
	}
	circle->SetVerticesBuffer(vertices).SetColorsBuffer(colors);
	vertexCount = vertices.size();
}

Menu::~Menu()
{

}

void Menu::OnEvent([[maybe_unused]] const SDL_Event& e)
{

}

void Menu::Tick()
{

}

void Menu::Draw()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	GLuint vaoId = 0;
	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);
	
	program->Use();
	circle->BindBuffers();
	glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
	
	glDeleteVertexArrays(1, &vaoId);
}

} // State
} // YGOpen
