#include <gl/glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>

// ID шейдерной программы
GLuint Program;
// ID атрибута
GLint Attrib_vertex;
// ID вершинного буфера
GLuint VBO;

// Структура для хранения вершин
struct Vertex
{
	GLfloat x;
	GLfloat y;
};

// Вершинный шейдер
const char* VertexShaderSource = R"(
#version 330 core
in vec2 coord;
void main() {
// Передаем координаты вершин
gl_Position = vec4(coord, 0.0, 1.0);
}
)";

// Фрагментный шейдер
const char* FragShaderSource = R"(
#version 330 core
out vec4 color;
void main() {
// Задаем цвет фрагмента (зеленый)
color = vec4(0, 1, 0, 1);
}
)";

// Функция для установки иконки приложения
void SetIcon(sf::Window& wnd);
// Функция для проверки ошибок
void checkOpenGLerror();
//
void ShaderLog(unsigned int shader);
// Функция для загрузки шейдеров
void InitShader();
// Функция для инициализации вершинного буфера
void InitVBO();
// Функция для инициализации ресурсов
void Init();
// Функция для отрисовки
void Draw();
// Функция для очистки шейдеров
void ReleaseShader();
// Функция для очистки вершинного буфера
void ReleaseVBO();
// Функция для очистки ресурсов
void Release();

void Init()
{
	// Инициализируем шейдеры
	InitShader();
	// Инициализируем вершинный буфер
	InitVBO();
}

int main()
{
	setlocale(LC_ALL, "Russian");
	sf::Window window(sf::VideoMode(600, 600), "Threeugolnik", sf::Style::Default, sf::ContextSettings(24));
	SetIcon(window);
	window.setVerticalSyncEnabled(true); // Вертикальная синхронизация
	window.setActive(true); // Устанавливаем контекст OpenGL
	glewInit(); // Инициализируем GLEW
	Init(); // Инициализируем ресурсы
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) // Если пользователь закрыл окно
			{
				window.close(); // Закрываем окно
				goto EXIT_IS_RIGHT_HERE; // Выходим из цикла
			}
			else if (event.type == sf::Event::Resized) // Если пользователь изменил размер окна
			{
				glViewport(0, 0, event.size.width, event.size.height); // Устанавливаем область вывода
			}
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Очищаем буфер цвета и буфер глубины
		Draw(); // Рисуем
		window.display(); // Выводим на экран
	}
EXIT_IS_RIGHT_HERE: // Метка выхода
	Release(); // Очищаем ресурсы
	return 0; // Выходим из программы
}

void InitVBO()
{
	glGenBuffers(1, &VBO); // Генерируем буфер
	// Вершины треугольника
	Vertex triangle[3] = {
		{-1.0f, -1.0f},
		{0.0f, 1.0f},
		{1.0f, -1.0f} };
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Привязываем буфер
	// Загружаем данные в буфер
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
	checkOpenGLerror();
}

void InitShader()
{
	// Создаем вершинный шейдер
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	// Загружаем исходный код шейдера
	glShaderSource(vShader, 1, &VertexShaderSource, NULL);
	// Компилируем шейдер
	glCompileShader(vShader);
	// Проверяем на ошибки
	std::cout << "vertex shader \n";
	ShaderLog(vShader);
	// Создаем фрагментный шейдер
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Загружаем исходный код шейдера
	glShaderSource(fShader, 1, &FragShaderSource, NULL);
	// Компилируем шейдер
	glCompileShader(fShader);
	// Проверяем на ошибки
	std::cout << "fragment shader \n";
	ShaderLog(fShader);

	// Создаем шейдерную программу
	Program = glCreateProgram();
	// Прикрепляем шейдеры к программе
	glAttachShader(Program, vShader);
	glAttachShader(Program, fShader);
	// Линкуем шейдерную программу
	glLinkProgram(Program);
	int link_ok;
	glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
	// Проверяем на ошибки
	if (!link_ok)
	{
		std::cout << "error attach shaders \n";
		return;
	}
	// Вытягиваем ID атрибута из шейдерной программы
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
	glUseProgram(Program); // Устанавливаем шейдерную программу
	glEnableVertexAttribArray(Attrib_vertex); // Включаем атрибут
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Привязываем буфер
	glVertexAttribPointer(Attrib_vertex, 2, GL_FLOAT, GL_FALSE, 0, 0); 	// Указываем данные атрибута
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Отвязываем буфер
	glDrawArrays(GL_TRIANGLES, 0, 3); // Рисуем треугольник
	glDisableVertexAttribArray(Attrib_vertex); // Отключаем атрибут
	glUseProgram(0); // Отключаем шейдерную программу
	checkOpenGLerror(); // Проверяем на ошибки
}

void Release()
{
	ReleaseShader(); // Очищаем шейдеры
	ReleaseVBO(); // Очищаем буфер
}

void ReleaseVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Отвязываем буфер
	glDeleteBuffers(1, &VBO); // Удаляем буфер
}

void ReleaseShader()
{
	glUseProgram(0); // Отключаем шейдерную программу
	glDeleteProgram(Program); // Удаляем шейдерную программу
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

// Вычислить x по заданному y с помощью уравнения прямой
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
