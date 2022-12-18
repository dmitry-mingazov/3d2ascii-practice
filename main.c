#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define SCREEN_WIDTH 100
#define SCREEN_HEIGHT 100
// distance between screen and object
#define K2 10
#define RADIUS 2
// distance between user and screen
#define K1 SCREEN_WIDTH*K2*3/(8*RADIUS*2)

#define PHI_SPACING 0.05f
#define THETA_SPACING 0.1f

typedef struct s_point_3D {
    double x;
    double y;
    double z;
} point_3D;

typedef struct s_point_2D {
    int x;
    int y;
} point_2D;

void print_screen(char** screen, int w, int h);
point_2D project_3D_to_2D(point_3D p);
int project_point(double coord, double z);
void draw_circle(point_3D center, double radius, char** screen, int w, int h);

int main(void) {
    int z = 5;
    char** screen = (char**)malloc(sizeof(char*)*SCREEN_WIDTH);
    double** zbuffer = (double**)malloc(sizeof(double*)*SCREEN_WIDTH);
    for(int i=0; i<SCREEN_WIDTH; i++) {
        screen[i] = (char*)malloc(sizeof(char*)*SCREEN_HEIGHT);
        zbuffer[i] = (double*)malloc(sizeof(double*)*SCREEN_HEIGHT);
        for(int j=0; j<SCREEN_HEIGHT; j++) {
            screen[i][j] = ' ';
            zbuffer[i][j] = 0;
        }
    }
    point_3D circle_center;
    circle_center.x = 0;
    circle_center.y = 0;
    circle_center.z = 0;
    draw_circle(circle_center, RADIUS, screen, SCREEN_WIDTH, SCREEN_HEIGHT);
    print_screen(screen, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void draw_circle(point_3D center, double radius, char** screen, int w, int h) {

    // loop to rotate the circle drawn
    for(double theta=0; theta < 2*M_PI; theta += THETA_SPACING) {
        // this loop will draw a 2d circle and apply the rotation dictated by 
        // the outer loop
        double costheta = cos(theta);
        double sintheta = sin(theta);
        for(double phi=0; phi < 2*M_PI; phi += PHI_SPACING) {
            double cosphi = cos(phi);
            double sinphi = sin(phi);

            // circle perpendicular to viewer, no rotations applied
            double x = center.x + cosphi * radius;
            double y = center.y + sinphi * radius;
            double z = center.z;

            // rotation matrix on y-axis
            // [ costheta   0   sintheta ]
            // [    0       1       0    ]
            // [ -sintheta  0   costheta ]
            //
            // applying rotation around y-axis
            point_3D curr_point;
            curr_point.x = costheta*x + sintheta*z;
            curr_point.y = y;
            curr_point.z = (-sintheta)*x + costheta*z;
            printf("phi: %.2f - (%.2f, %.2f, %.2f)\n", phi, curr_point.x, curr_point.y, curr_point.z);


            point_2D pp = project_3D_to_2D(curr_point);
            // printf("x: %02d - y: %02d  --- phi: %f\n", pp.x, pp.y, phi);

            pp.x += w/2;
            pp.y = h/2 - pp.y;
            if (pp.x < w && pp.y < h && pp.x >= 0 && pp.y >= 0) {
                screen[pp.y][pp.x] = '@';
            } else {
                printf("Out of bound (%d, %d)\n", pp.x, pp.y);
            }
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

int project_point(double coord, double z) {
    // printf("%d * %.2f / %.2f\n", K1, coord, z);
    return (int) ((K1*coord)/(z+K2));
}

void print_screen(char** screen, int w, int h) {
    system("clear");
    for(int y=0; y<w; y++) {
        for(int x=0; x<h; x++) {
            printf(" %c", screen[y][x]);
        }
        printf("\n");
    }
    printf("\n");
}

