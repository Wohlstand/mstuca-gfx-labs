/*********************************************************************
            Пример программы с использованием SDL и OpenGL
*********************************************************************/

#include <iostream>
#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#undef main
#include <GL/GLU.h>

/************************Глобальные переменные***************************/
const int   WIDTH  = 800; //!< ШИРИНА ОКНА
const int   HEIGHT = 600; //!< ВЫСОТА ОКНА

const char*  TITLE = "LAB-1: Per-pixel math"; //!< Заголовок окна

struct Rgb    //!< Цвет
{
    double r;//!< Красный
    double g;//!< Зелёный
    double b;//!< Синий
};

SDL_GLContext OpenGL_Context; //!< Контекст OpenGL



/***************************Общие функци*********************************/
void err(const char *message);
void line(int x1, int y1, int x2, int y2, Rgb colour);
void putPixel(int x, int y, Rgb colour);
void getPixel(int x, int y, Rgb &colour);
SDL_Window *initWindow(const char* title, int w, int h);

void Render();

template<typename T>
void unused(T const &) {}

/***************************Общие функци*********************************/
///
/// Функция завершения программы с ошибкой, указанной в
/// параметре message
///
void err(const char *message)
{
    std::cerr << message << std::endl;
    std::cin.sync();
    exit(1);
}



///
/// Функция рисование линии между двумя заданными точками
///
void line(int x1, int y1, int x2, int y2, Rgb colour)
{
    float w=WIDTH;
    float h=HEIGHT;

    float nx1 = x1 / (w / 2) - 1;
    float ny1 = (h - y1 - 1) / (h / 2) - 1;

    float nx2 = x2 / (w / 2) - 1;
    float ny2 = (h - y2 - 1) / (h / 2) - 1;

    glLineWidth(1);
    glColor3f(colour.r, colour.g, colour.b);
    glBegin(GL_LINES);
        glVertex3f(nx1, ny1, 0.0);
        glVertex3f(nx2, ny2, 0.0);
    glEnd();
}



///
/// Функция рисования одного пикселя на заданной точке экрана
///
void putPixel(int x, int y, Rgb colour)
{
    float w=WIDTH;
    float h=HEIGHT;

    float nx = (x) / (w / 2) - 1;
    float ny = (h - (y) - 1) / (h / 2) - 1;
    float nx2 = (x+1) / (w / 2) - 1;
    float ny2 = (h - (y-1) - 1) / (h / 2) - 1;

    glLineWidth(2);
    glColor3f(colour.r, colour.g, colour.b);
    glBegin(GL_LINES);
        glVertex3f(nx,   ny,   0.0);
        glVertex3f(nx2,  ny2,  0.0);
    glEnd();
}



///
/// Функция захвата цвета с указанной кординаты экрана
///
void getPixel(int x, int y, Rgb &colour)
{
    glReadBuffer(GL_BACK_RIGHT);
    float in[3] = {0,0,0};
    glReadPixels(x, HEIGHT-2-y, 1, 1, GL_RGB, GL_FLOAT, &in);
    colour.r = in[0];
    colour.g = in[1];
    colour.b = in[2];
    return;
}




///
/// Функция создания окна и инициализации OpenGL, принимает
/// в качестве параметров название и размеры окна, возвращает
/// созданное окно SDL
///
SDL_Window *initWindow(const char* title, int w, int h)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        err("Couldn't initialise SDL");

    // Устанавливаем версию использованной OpenGL (2.1)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_Window *window = SDL_CreateWindow(title,
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if (!window)
    err("Couldn't create SDL window");

    // Создадим OpenGL контекст, с которым будем работать
    OpenGL_Context = SDL_GL_CreateContext(window);
    if(!OpenGL_Context)
        err("Couldn't create OpenGL context");

    //Попытаемся включить вертикальную синхронизацию
    if(SDL_GL_SetSwapInterval(1) < 0)
        printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

    // Произведём инциализацию OpenGL
    // Будем помещать ошибки в эту переменную при их появлении
    GLenum error = GL_NO_ERROR;

    // нициализируем матрицы Projection и Modelview
    GLuint matrixes[2]= { GL_PROJECTION, GL_MODELVIEW };

    for (int i = 0; i < 2; i++)
    {
        glMatrixMode(matrixes[i]);
        glLoadIdentity();

        error = glGetError();
        if (error != GL_NO_ERROR) // Что-то пошло не так
            err(reinterpret_cast<const char *>(gluErrorString(error)));
    }
    return window;
}



int main(int argc, char* argv[])
{
    unused(argc);
    unused(argv);

    SDL_Window *window = initWindow(TITLE, WIDTH, HEIGHT);


    SDL_Event event;

    bool working=true;
    //Рабочий цикл, в кором выполняется сама программа
    while (working)
    {
        // Вечный цикл обработки событий
        // (движение мыши, перемещение и закрытие окон)
        while (SDL_PollEvent(&event) != 0)
        {
            // Если пользователь закрыл окно, выходим из программы
            if (event.type == SDL_QUIT)
            {
                working=false;
                return 0;
            }

            // Если пользователь нажал клавишу, выполним какое-нибудь действие
            // Например, если пользователь нажал клавишу ESC - выходим из программы
            else if (event.type  == SDL_KEYDOWN)
            {
                switch(event.key.keysym.sym)
                { //Проверка клавиш
                    case SDLK_ESCAPE:
                        working=false;
                    break;
                }
            }
        }

        //Отрисовать текущий кадр
        Render();

        //Выполнить отрисовку матрицы в буфере OpenGL
        glFlush();
        //Обновить содержимое окна
        SDL_GL_SwapWindow(window);

        //Подождать 10 миллисекунд
        SDL_Delay(30);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

/************Функция отрисовки*******************/
//Пример рисования белой линии на чёрном фоне
void Render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Очистим фон чёрным
    glClear(GL_COLOR_BUFFER_BIT);          // Применим изменение цвета

    //Нарисуем белую линию
    line(110, 110, 700, 400, {1.0, 1.0, 1.0});

    //Нарисуем зелёную точку
    putPixel(400, 300, {0.0, 1.0, 0.0});

    //Нарисуем синюю точку
    putPixel(200, 400, {0.0, 0.0, 1.0});

    Rgb color;

    //Возьмём цвет с кончика линии
    getPixel(110, 110, color);
    //Нарисуем точку прочитанного цвета в другом месте
    putPixel(500, 100, color);

    //Возьмём цвет с зелёной точки
    getPixel(400, 300, color);
    //И поставим его рядом с новой точкой
    putPixel(503, 100, color);

}

