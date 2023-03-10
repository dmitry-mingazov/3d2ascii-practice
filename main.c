#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define SCREEN_WIDTH 80 
#define SCREEN_HEIGHT 80
// distance between screen and object
#define K2 10
#define RADIUS 2
// distance between user and screen
#define K1 SCREEN_WIDTH*K2*1/(2*RADIUS*2)

#define PHI_SPACING 0.02f
#define THETA_SPACING 0.02f
#define ALPHA_SPACING 0.02f

typedef struct s_point_3D {
    double x;
    double y;
    double z;
} point_3D;

typedef struct s_point_2D {
    int x;
    int y;
} point_2D;

const char* light_chars = ".,-~:;=!*#$@";

point_3D r_on_xaxis(point_3D p, double theta);
point_3D r_on_yaxis(point_3D p, double theta);

point_3D init_3D_point(double x, double y, double z); 
void print_screen(char** screen, int w, int h);
point_2D project_3D_to_2D(point_3D p);
int project_point(double coord, double z);
void draw_sphere(point_3D offset, double radius, int w, int h, double** zbuffer, point_3D ls, char** screen);

int main(void) {
    int z = 5;
    char** screen = (char**)malloc(sizeof(char*)*SCREEN_WIDTH);
    double** zbuffer = (double**)malloc(sizeof(double*)*SCREEN_WIDTH);
    for(int i=0; i<SCREEN_HEIGHT; i++) {
        screen[i] = (char*)malloc(sizeof(char*)*SCREEN_HEIGHT);
        zbuffer[i] = (double*)malloc(sizeof(double*)*SCREEN_HEIGHT);
        for(int j=0; j<SCREEN_WIDTH; j++) {
            screen[i][j] = ' ';
            zbuffer[i][j] = 0;
        }
    }
    point_3D origin_ls = init_3D_point(0, 0, -1); // starting light source

    double lmax = RADIUS;

    point_3D first_sphere = init_3D_point(0, 0, 0);
    point_3D second_sphere = init_3D_point(RADIUS*2, 0, 0);

    system("clear");
    for(;;){
        for(double alpha=0; alpha < 2*M_PI; alpha += ALPHA_SPACING) {
            // applying rotation on x-axis
            point_3D ls = r_on_xaxis(origin_ls, alpha); // light source

            // clear matrices
            for(int i=0; i<SCREEN_HEIGHT; i++) {
                for(int j=0; j<SCREEN_WIDTH; j++) {
                    screen[i][j] = ' ';
                    zbuffer[i][j] = 0;
                }
            }

            draw_sphere(first_sphere, RADIUS, SCREEN_WIDTH, SCREEN_HEIGHT, zbuffer, ls, screen);
            draw_sphere(second_sphere, RADIUS/2, SCREEN_WIDTH, SCREEN_HEIGHT, zbuffer, ls, screen);
            print_screen(screen, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
    }
}

void draw_sphere(point_3D offset, double radius, int w, int h, double** zbuffer, point_3D ls, char** screen) {
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
            double x = cosphi * radius;
            double y = sinphi * radius;
            double z = 0;

            // printf("phi: %.2f - (%.2f, %.2f, %.2f)\n", phi, curr_point.x, curr_point.y, curr_point.z);

            // rotation on y axis applied
            point_3D curr_point = r_on_yaxis(init_3D_point(x, y, z), theta);

            curr_point.x += offset.x;
            curr_point.y += offset.y;
            curr_point.z += offset.z;

            point_2D pp = project_3D_to_2D(curr_point);
            // printf("x: %02d - y: %02d  --- phi: %f\n", pp.x, pp.y, phi);

            pp.x += w/2;
            pp.y = h/2 - pp.y;

            if(pp.x >= w || pp.x < 0 || pp.y >= h || pp.y < 0) {
                continue;
            }

            double luminance = (ls.x * curr_point.x + ls.y * curr_point.y + ls.z * curr_point.z);
            if (luminance > radius) {
                printf("luminance %.2f at (%.2f, %.2f, %.2f) with ls (%.2f, %.2f, %.2f)\n", luminance, curr_point.x, curr_point.y, curr_point.z, ls.x, ls.y, ls.z);
                getchar();
            }



            // if 0 or below, this point isn't reached by light
            if (luminance > 0) {
                if (curr_point.z < zbuffer[pp.y][pp.x]) {
                    zbuffer[pp.y][pp.x] = curr_point.z;

                    double lunit = radius/12;
                    int luminance_index = luminance / lunit;
                    if (luminance_index >= 0 && luminance_index < 12) {
                        screen[pp.y][pp.x] = light_chars[luminance_index];
                    } else {
                        // printf("Index out of bound! L: %d  - luminance: %.2f  - lunit: %.2f  - lmax: %.2f\n", luminance_index, light_map[y][x], lunit, lmax);
                    }
                }
            // but we want to show it anyway, so we set luminance to 0.01
            } else {
                if (curr_point.z < zbuffer[pp.y][pp.x]) {
                    zbuffer[pp.y][pp.x] = curr_point.z;
                    screen[pp.y][pp.x] = light_chars[0];
                }
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
    printf("\x1b[H");
    for(int y=0; y<h; y++) {
        for(int x=0; x<w; x++) {
            printf(" %c", screen[y][x]);
        }
        printf("\n");
    }
    printf("\n");
}

// rotation matrix on x-axis
// [    1          0           0     ]
// [    0       costheta   -sintheta ]
// [    0       sintheta    costheta ]
//
// applying rotation around y-axis
point_3D r_on_xaxis(point_3D p, double theta) {
    double costheta = cos(theta), sintheta = sin(theta);
    point_3D res = {
        .x = p.x,
        .y = costheta*p.y - sintheta*p.z,
        .z = sintheta*p.y + costheta*p.z,
    };
    return res;
}

// rotation matrix on y-axis
// [ costheta   0   sintheta ]
// [    0       1       0    ]
// [ -sintheta  0   costheta ]
//
// applying rotation around y-axis
point_3D r_on_yaxis(point_3D p, double theta) {
    double costheta = cos(theta), sintheta = sin(theta);
    point_3D res = {
        .x = costheta*p.x + sintheta*p.z,
        .y = p.y,
        .z = -sintheta*p.x + costheta*p.z,
    };
    return res;
}

point_3D init_3D_point(double x, double y, double z) {
    point_3D p = {
        .x = x,
        .y = y,
        .z = z
    };
    return p;
}
