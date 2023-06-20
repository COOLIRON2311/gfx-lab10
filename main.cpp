#include <gl/glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>

// ID ��������� ���������
GLuint Program;
// ID ��������
GLint Attrib_vertex;
// ID ���������� ������
GLuint VBO;

// ��������� ��� �������� ������
struct Vertex
{
	GLfloat x;
	GLfloat y;
};

// ��������� ������
const char* VertexShaderSource = R"(
#version 330 core
in vec2 coord;
void main() {
// �������� ���������� ������
gl_Position = vec4(coord, 0.0, 1.0);
}
)";

// ����������� ������
const char* FragShaderSource = R"(
#version 330 core
out vec4 color;
void main() {
// ������ ���� ��������� (�������)
color = vec4(0, 1, 0, 1);
}
)";

// ������� ��� ��������� ������ ����������
void SetIcon(sf::Window& wnd);
// ������� ��� �������� ������
void checkOpenGLerror();
//
void ShaderLog(unsigned int shader);
// ������� ��� �������� ��������
void InitShader();
// ������� ��� ������������� ���������� ������
void InitVBO();
// ������� ��� ������������� ��������
void Init();
// ������� ��� ���������
void Draw();
// ������� ��� ������� ��������
void ReleaseShader();
// ������� ��� ������� ���������� ������
void ReleaseVBO();
// ������� ��� ������� ��������
void Release();

void Init()
{
	// �������������� �������
	InitShader();
	// �������������� ��������� �����
	InitVBO();
}

int main()
{
	setlocale(LC_ALL, "Russian");
	sf::Window window(sf::VideoMode(600, 600), "Threeugolnik", sf::Style::Default, sf::ContextSettings(24));
	SetIcon(window);
	window.setVerticalSyncEnabled(true); // ������������ �������������
	window.setActive(true); // ������������� �������� OpenGL
	glewInit(); // �������������� GLEW
	Init(); // �������������� �������
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) // ���� ������������ ������ ����
			{
				window.close(); // ��������� ����
				goto EXIT_IS_RIGHT_HERE; // ������� �� �����
			}
			else if (event.type == sf::Event::Resized) // ���� ������������ ������� ������ ����
			{
				glViewport(0, 0, event.size.width, event.size.height); // ������������� ������� ������
			}
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������� ����� ����� � ����� �������
		Draw(); // ������
		window.display(); // ������� �� �����
	}
EXIT_IS_RIGHT_HERE: // ����� ������
	Release(); // ������� �������
	return 0; // ������� �� ���������
}

void InitVBO()
{
	glGenBuffers(1, &VBO); // ���������� �����
	// ������� ������������
	Vertex triangle[3] = {
		{-1.0f, -1.0f},
		{0.0f, 1.0f},
		{1.0f, -1.0f} };
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // ����������� �����
	// ��������� ������ � �����
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
	checkOpenGLerror();
}

void InitShader()
{
	// ������� ��������� ������
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	// ��������� �������� ��� �������
	glShaderSource(vShader, 1, &VertexShaderSource, NULL);
	// ����������� ������
	glCompileShader(vShader);
	// ��������� �� ������
	std::cout << "vertex shader \n";
	ShaderLog(vShader);
	// ������� ����������� ������
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	// ��������� �������� ��� �������
	glShaderSource(fShader, 1, &FragShaderSource, NULL);
	// ����������� ������
	glCompileShader(fShader);
	// ��������� �� ������
	std::cout << "fragment shader \n";
	ShaderLog(fShader);

	// ������� ��������� ���������
	Program = glCreateProgram();
	// ����������� ������� � ���������
	glAttachShader(Program, vShader);
	glAttachShader(Program, fShader);
	// ������� ��������� ���������
	glLinkProgram(Program);
	int link_ok;
	glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
	// ��������� �� ������
	if (!link_ok)
	{
		std::cout << "error attach shaders \n";
		return;
	}
	// ���������� ID �������� �� ��������� ���������
	const char* attr_name = "coord";
	Attrib_vertex = glGetAttribLocation(Program, attr_name);
	if (Attrib_vertex == -1)
	{
		std::cout << "could not bind attrib" << attr_name << std::endl;
		return;
	}
	checkOpenGLerror();
}

void Draw()
{
	glUseProgram(Program); // ������������� ��������� ���������
	glEnableVertexAttribArray(Attrib_vertex); // �������� �������
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // ����������� �����
	glVertexAttribPointer(Attrib_vertex, 2, GL_FLOAT, GL_FALSE, 0, 0); 	// ��������� ������ ��������
	glBindBuffer(GL_ARRAY_BUFFER, 0); // ���������� �����
	glDrawArrays(GL_TRIANGLES, 0, 3); // ������ �����������
	glDisableVertexAttribArray(Attrib_vertex); // ��������� �������
	glUseProgram(0); // ��������� ��������� ���������
	checkOpenGLerror(); // ��������� �� ������
}

void Release()
{
	ReleaseShader(); // ������� �������
	ReleaseVBO(); // ������� �����
}

void ReleaseVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0); // ���������� �����
	glDeleteBuffers(1, &VBO); // ������� �����
}

void ReleaseShader()
{
	glUseProgram(0); // ��������� ��������� ���������
	glDeleteProgram(Program); // ������� ��������� ���������
}

void ShaderLog(unsigned int shader)
{
	int infologLen = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
	if (infologLen > 1)
	{
		int charsWritten = 0;
		std::vector<char> infoLog(infologLen);
		glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog.data());
		std::cout << "InfoLog: " << infoLog.data() << std::endl;
	}
}

void checkOpenGLerror()
{
	GLenum errCode;
	const GLubyte* errString;
	if ((errCode = glGetError()) != GL_NO_ERROR)
	{
		errString = gluErrorString(errCode);
		std::cout << "OpenGL error: " << errString << std::endl;
	}
}

// ��������� x �� ��������� y � ������� ��������� ������
inline int leq_x(int x1, int y1, int x2, int y2, int y)
{
	return (y - y1) * (x2 - x1) / (y2 - y1) + x1;
}

void SetIcon(sf::Window& wnd)
{
	sf::Image image;
	image.create(16, 16);
	for (int i = 0; i < 16; i++)
	{
		int a = leq_x(0, 16, 8, 0, i);
		int b = leq_x(8, 0, 16, 16, i);

		if (a > b)
			std::swap(a, b);

		for (int j = a; j <= b; j++)
			image.setPixel(j, i, sf::Color::Green);
	}

	wnd.setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());
}
