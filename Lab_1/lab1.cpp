/*********************************************************************
                   Пример лабораторной работы №1
*********************************************************************/

#include <iostream>
#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#undef main
#include <GL/GLU.h>

#ifdef _WIN32
#include <conio.h>
#else
#include <ncurses.h>
#endif

/************************Глобальные переменные***************************/
const int   WIDTH  = 800; //!< ШИРИНА ОКНА
const int   HEIGHT = 600; //!< ВЫСОТА ОКНА

const char*  TITLE = "LAB-1: Per-pixel math"; //!< Заголовок окна

/*
 * Rgb - структура, содержащая три параметра:
 * .r - уровень красного от 0.0 до 1.0
 * .g - уровень зелёного от 0.0 до 1.0
 * .b - уровень синего от 0.0 до 1.0
*/
struct Rgb    //!< Цвет
{
    double r;//!< Красный
    double g;//!< Зелёный
    double b;//!< Синий
};

//Некоторые цвета
#define RED     {1.0, 0.0, 0.0}
#define BLUE    {0.0, 0.0, 1.0}
#define GREEN   {0.0, 1.0, 0.0}
#define YELLOW  {1.0, 1.0, 0.0}
#define WHITE   {1.0, 1.0, 1.0}
#define BLACK   {0.0, 0.0, 0.0}

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

/************************************************************************/
float Xr[5], Yr[5];

void initCoord();
void drawRect(Rgb color);
void move(int,int);
void rotate(float);
void resMatrix(float,float,float,float,float);
float rad(float);
void mash(float,float);
float vx;
float vy;
float l;
/************************************************************************/

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
    float oX = nx-nx2;
    float oY = ny-ny2;

    glLineWidth(2);
    glColor3f(colour.r, colour.g, colour.b);
    glBegin(GL_QUADS);
        glVertex3f(nx+oX, ny+oY,   0.0);
        glVertex3f(nx+oX, ny-oY,   0.0);
        glVertex3f(nx-oX, ny-oY,   0.0);
        glVertex3f(nx-oX, ny+oY,   0.0);
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

    std::cout << "Vvedite vershinu kvadrata x:";
    std::cin >> vx;

    std::cout << "Vvedite vershinu kvadrata y:";
    std::cin >> vy;

    std::cout << "Vvedite dlinu storoni kvadrata l:";
    std::cin >> l;

    std::cout << "Zapusk graphiki...\n";


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
        SDL_Delay(10);
    }

    std::cout << "Bye!\n";

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


float grad = 0;
float radx = 0;
/************Функция отрисовки*******************/
void Render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Очистим фон чёрным
    glClear(GL_COLOR_BUFFER_BIT);          // Применим изменение цвета

    if (grad==0) grad=360;
    if (radx==0) radx=360;

    initCoord();
    drawRect(YELLOW);
    move(-(vx+l/1),-(vy+l/1));
    rotate(rad(-grad));
    mash(1,1);
    move(vx+l/1,vy+l/1);
    drawRect(YELLOW);
    //------------Рисуем матрицу---------------------------------
    initCoord();
    move(l+100,0);
    drawRect(RED);
    resMatrix(vx+l+100+l, vy+l, 1, 1, rad(-grad));
    drawRect(RED);

    putPixel(vx+l, vy+l, RED);

    //------------Вращение вокруг начала координат--------------------
    initCoord();

      move(l+100,l+100);
       drawRect(GREEN);
        //initCoord();
        //move(0,0);
    rotate(rad(radx));
    drawRect(GREEN);
        //mash(2,2);
        //drawRect();
    grad+=1;
    radx+=1;
}



void mash(float sx, float sy)
{
    for(int i=0;i<5;i++)
    {
        Xr[i]*=sx;
        Yr[i]*=sy;
    }
}

///
/// Функция преобразования градусов в радианы
///
float rad(float grad)
{
    return (atanf(1.0f)/45.0f)*grad;
}

///
/// Функция инициализации исходных состояний матрицы
///
void initCoord()
{
    float X0[5] = {vx, vx+l,vx+l,vx,vx};
    float Y0[5] = {vy,vy,vy+l,vy+l,vy};
    for(int i=0;i<5;i++)
    {
        Xr[i] = X0[i];Yr[i]=Y0[i];
    }
}

///
/// Функция рисования квадрата
///
void drawRect(Rgb color)
{
    for(int i=0;i<4;i++)
    {
        line(Xr[i], Yr[i], Xr[i+1], Yr[i+1], color);
    }
}

///
/// Функция перемещения матрицы
///
void move(int a, int b)
{
    for(int i=0;i<5;i++)
    {
        Xr[i]+=a;Yr[i]+=b;
    }
}

///
/// Функция вращения матрицы на угол Альфа
///
void rotate(float alpha)
{
    float X;
    for(int i=0;i<5;i++)
    {
        X=Xr[i];
        Xr[i] = X*cosf(alpha) - Yr[i]*sinf(alpha);
        Yr[i] = X*sinf(alpha) + Yr[i]*cosf(alpha);
    }
}

///
/// Функция вращения матрицы на угол Альфа
///
void resMatrix(float a, float b,float sx, float sy, float alpha)
{
    float X;
    for(int i=0; i<5; i++)
    {
        X=Xr[i];
        Xr[i] = X*sx*cosf(alpha) - Yr[i]*sy*sinf(alpha) - a*sx*cosf(alpha) + b*sy*sinf(alpha) + a;
        Yr[i] = X*sx*sinf(alpha) + Yr[i]*sy*cosf(alpha) - a*sx*sinf(alpha) - b*sy*cosf(alpha) + b;
    }
}

