#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define SCREEN_WIDTH 100
#define SCREEN_HEIGHT 100
// distance between user and screen
#define K1 2
// distance between screen and object
#define K2 5
#define PI 3.14f
#define RADIUS 5

#define PHI_SPACING 0.05f

typedef struct s_point_3D {
    float x;
    float y;
    float z;
} point_3D;

typedef struct s_point_2D {
    int x;
    int y;
} point_2D;

void print_screen(char** screen, int w, int h);
point_2D project_3D_to_2D(point_3D p);
int project_point(float coord, float z);
void draw_circle(point_3D center, float radius, char** screen, int w, int h);

int main(void) {
    int z = 5;
    char** screen = (char**)malloc(sizeof(char*)*SCREEN_WIDTH);
    float** zbuffer = (float**)malloc(sizeof(float*)*SCREEN_WIDTH);
    for(int i=0; i<SCREEN_WIDTH; i++) {
        screen[i] = (char*)malloc(sizeof(char*)*SCREEN_HEIGHT);
        zbuffer[i] = (float*)malloc(sizeof(float*)*SCREEN_HEIGHT);
        for(int j=0; j<SCREEN_HEIGHT; j++) {
            screen[i][j] = ' ';
            zbuffer[i][j] = 0;
        }
    }
    point_3D circle_center;
    circle_center.x = 0;
    circle_center.y = 0;
    circle_center.z = 0;
    draw_circle(circle_center, 40, screen, SCREEN_WIDTH, SCREEN_HEIGHT);
    print_screen(screen, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void draw_circle(point_3D center, float radius, char** screen, int w, int h) {

    for(float phi=0; phi < 2*PI; phi += PHI_SPACING) {
        float cosphi = cosf(phi);
        float sinphi = sinf(phi);

        point_3D curr_point;
        curr_point.x = center.x + cosphi * radius;
        curr_point.y = center.y + sinphi * radius;
        curr_point.z = center.z + K2;

        point_2D pp = project_3D_to_2D(curr_point);
        printf("x: %02d - y: %02d  --- phi: %f\n", pp.x, pp.y, phi);

        pp.x += w/2;
        pp.y += h/2;
        if (pp.x < w && pp.y < h) {
            screen[pp.x][pp.y] = '@';
        } else {
            printf("Out of bound (%d, %d)", pp.x, pp.y);
        }
    }
}

// project a point from 3D to 2D screen
point_2D project_3D_to_2D(point_3D p) {
    point_2D res;
    res.x = project_point(p.x, p.z);
    res.y = project_point(p.y, p.z);
    // printf("Projecting: 3D (%.2f, %.2f) --> 2D (%d, %d)\n", p.x, p.y, res.x, res.y);
    return res;
}

int project_point(float coord, float z) {
    printf("%d * %.2f / %.2f\n", K1, coord, z);
    return (int) ((K1*coord)/z);
}

void print_screen(char** screen, int w, int h) {
    system("clear");
    for(int i=0; i<w; i++) {
        for(int j=0; j<h; j++) {
            printf(" %c", screen[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

