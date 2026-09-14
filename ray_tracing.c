#include <stdio.h>
#include "tigr/tigr.h"
#include <math.h>
#define PI 3.14159265358979323846

// Define windows scale
#define HEIGHT 800
#define WIDTH 1200
// Define playground border
#define UP_BORDER 20
#define DOWN_BORDER (HEIGHT -37)
#define LEFT_BORDER 0
#define RIGHT_BORDER WIDTH
typedef struct
{
    int up;
    int down;
    int left;
    int right;
} Border;
Border border = {UP_BORDER, DOWN_BORDER, LEFT_BORDER, RIGHT_BORDER};

// Define Colors
#define GREY tigrRGB(0x80, 0x90, 0xa0)
#define BLACK tigrRGB(0x00, 0x00, 0x00)
#define WHITE tigrRGB(0xff, 0xff, 0xff)
#define RED     tigrRGB(0xff, 0x00, 0x00)
#define GREEN   tigrRGB(0x00, 0xff, 0x00)
#define BLUE    tigrRGB(0x00, 0x00, 0xff)
#define YELLOW  tigrRGBA(0xff, 0xff, 0x00, 0xff)
#define CYAN    tigrRGB(0x00, 0xff, 0xff)
#define MAGENTA tigrRGB(0xff, 0x00, 0xff)
#define SKY_BLUE tigrRGB(0x87, 0xce, 0xeb)
#define GOLD    tigrRGB(0xff, 0xd7, 0x00)
#define SILVER  tigrRGB(0xc0, 0xc0, 0xc0)

// Define Ray Number and Reflect Times
#define RAY_NUM 360
#define REF_NUM 1
#define SAMPLE_RADIUS 10

// Define refleciton albedo
#define BORDER_ALBEDO 0.75
#define OBSTACLE_ALBEDO 0.75


// Define Cirle struct
typedef struct
{
    double x;
    double y;
    double r;
    TPixel color;
} Circle;
Circle bulb;
Circle obstacle;


// Define Mouse Event
typedef struct
{
    int x;
    int y;
    int button;
} Mouse_Event;
Mouse_Event mouse;


// Define 2-D Vecter
typedef struct {
    double x;
    double y;
} Vec2;




// Precess mouse event, normally dragging lightsource or obstacle
void Process_Mouse_Evevt(Mouse_Event *mouse, Circle *bulb, Circle *obstacle)
{    
    // Left button to adjust light source positon
    if (mouse->button == 1)
    {
        // Set position within the screen
        //bulb->x = mouse->x;   
        bulb->x = (mouse->x < LEFT_BORDER) ? LEFT_BORDER : (mouse->x > RIGHT_BORDER ? RIGHT_BORDER : mouse->x);    
        //bulb->y = mouse->y;
        bulb->y = (mouse->y < UP_BORDER) ? UP_BORDER : (mouse->y > DOWN_BORDER ? DOWN_BORDER : mouse->y); 
    }


    // Right button to adjust obstacle position
    if (mouse->button == 2)
    {
        // Set position within the screen
        //obstacle->x = mouse->x;   
        obstacle->x = (mouse->x < LEFT_BORDER) ? LEFT_BORDER : (mouse->x > RIGHT_BORDER ? RIGHT_BORDER : mouse->x);    
        //obstacle->y = mouse->y;
        obstacle->y = (mouse->y < UP_BORDER) ? UP_BORDER : (mouse->y > DOWN_BORDER ? DOWN_BORDER : mouse->y); 
    }
};




// Cauculate normal when obstacle is a CIRCLE.
Vec2 Normal_circle(int pixel_x, int pixel_y, Circle obstacle)
{
    Vec2 normal;
    double len;

    normal.x = pixel_x - obstacle.x;
    normal.y = obstacle.y - pixel_y;

    len = sqrt(normal.x * normal.x + normal.y * normal.y);

    normal.x = normal.x / len;
    normal.y = normal.y / len;
    return normal;
};




// Judge whether pixel is a obstacle point
int is_obstacle(Tigr* screen, int pixel_x, int pixel_y)
{
    TPixel pixel_color, obs_color;
    pixel_color = tigrGet(screen, pixel_x, pixel_y);
    obs_color = SILVER;
    if (pixel_color.g == obs_color.g && pixel_color.b == obs_color.b && pixel_color.r == obs_color.r)
    {
        return 1;
    }
    else
    {
        return 0;
    }
};




// Judge whether pixel is a boundary point
int is_boundary(Tigr* screen, int pixel_x, int pixel_y)
{
    // Pixel is not obstacle at all
    if (is_obstacle(screen, pixel_x, pixel_y) == 0)
    {
        return 0;
    }
        
    // Pixel at least have a non-obstacle nearby point.
    if (is_obstacle(screen, pixel_x - 1, pixel_y) == 0)
    {
        return 1;
    }   
    if (is_obstacle(screen, pixel_x + 1, pixel_y) == 0)
    {
        return 1;
    }
    if (is_obstacle(screen, pixel_x, pixel_y - 1) == 0)
    {
        return 1;
    }
    if (is_obstacle(screen, pixel_x, pixel_y + 1) == 0)
    {
        return 1;
    }

    // Pixel is inside the obstacle
    return 0;
};




// Caculate normal use PCA methed -- Covariance analysis
Vec2 Normal(Tigr* screen, int pixel_x, int pixel_y, int s_radius)
{
    Vec2 normal;
    Vec2 b_points[512];             // Store boundary points

    TPixel window_color, boundary_color;
    window_color = GREEN;
    boundary_color = RED;

    // Collect boundary pixels
    int i, j, p = 0;
    for (i = pixel_x - s_radius; i <= pixel_x + s_radius; i = i + 1)
    {
        for (j = pixel_y - s_radius; j <= pixel_y + s_radius; j = j + 1)
        {
            if (is_boundary(screen, i, j) == 1)
            {
                b_points[p].x = i;
                b_points[p].y = j;
                p = p + 1;
            }
        }
    }
    /*
    //  Show red boundary points!
    printf("Boundary: %d points\n", p);
    for (i = 0; i < p; i++)
    {
        tigrPlot(screen, b_points[i].x, b_points[i].y, boundary_color);
    }*/


    // Cauculate normal vector
    // PCA: use covariance to get normal vec
    
    // 1. get mean x,y
    double mean_x = 0.00, mean_y = 0.00;
    for (i = 0; i < p; i++)
    {
        mean_x = mean_x + b_points[i].x;
        mean_y = mean_y + b_points[i].y;
    }
    mean_x = mean_x / p;
    mean_y = mean_y / p;
    // 2. get covariance matrix
    double xx = 0.00, xy = 0.00, yy = 0.00;
    for (i = 0; i < p; i++)
    {
        xx = xx + pow(b_points[i].x - mean_x, 2);
        xy = xy + (b_points[i].x - mean_x) * (b_points[i].y - mean_y);
        yy = yy + pow(b_points[i].y - mean_y, 2);
    }
    // 3, get normal vec
    double theta;
    theta = 0.5 * atan2(2.0 * xy, xx - yy);

    normal.x = - sin(theta);
    normal.y = - cos(theta);        // Screen y is opposite to coodinate y
    return normal;
};




// Draw rays with reflection
void Generate_Rays(Tigr* screen, Circle bulb, Border border, int ray_num, int ref_num)
{    
    double rad_step;
    rad_step = 2 * PI / ray_num;          // Angle step in current ray number setup
    
    // Draw each ray by rad_tep
    int i;
    for (i = 0; i < ray_num; i = i + 1)
    {
        //printf("Step: %d    Angle: %f\n", i, i * rad_step);
        double rad;
        rad = i * rad_step;

        // Get ray drawing step
        double x_step, y_step;
        x_step = sin(rad);
        y_step = cos(rad);      

        // Get the start point of the ray -- normally on the light source surface
        double photon_x, photon_y;
        int pixel_x, pixel_y;
        TPixel ray_color;
        photon_x = bulb.x + (bulb.r-1) * x_step;
        photon_y = bulb.y - (bulb.r-1) * y_step;            // Because screen y is opposite to nomal coordinate y
        
        
        // Draw specific ray
        int end_flag = 0;                     // Flag when to stop drawing this ray. 1, reach border; 2, reach obstacle
        int ref_cont = 0;                     // Reflection counting
        ray_color = YELLOW;

        int j;
        for (j = 0; ref_cont <= ref_num ; j = j + 1)
        {
            
            tigrPlot(screen, (int)photon_x, (int)photon_y, ray_color);      // Draw ray pixel

            photon_x = photon_x + x_step;
            photon_y = photon_y - y_step;                                   // Because screen y is opposite to nomal coordinate y
            pixel_x = (int)photon_x;
            pixel_y = (int)photon_y;


            // Process border refelections
            if (pixel_x <= LEFT_BORDER)
            {
                x_step = -1.0 * x_step;                      // Reflect the ray
                photon_x = pixel_x +1;                       // Make sure reflect begin!
                photon_y = pixel_y;
                ref_cont = ref_cont + 1;                     // Count a reflection
                ray_color.a = ray_color.a * BORDER_ALBEDO;   // Cut the ray lightness
                continue;
            }
            if (pixel_x >= RIGHT_BORDER)
            {
                x_step = -1.0 * x_step;
                photon_x = pixel_x;
                photon_y = pixel_y;
                ref_cont = ref_cont + 1;
                ray_color.a = ray_color.a * BORDER_ALBEDO;
                continue;
            }
            if (pixel_y <= UP_BORDER)
            {
                y_step = -1.0 * y_step;
                photon_x = pixel_x;
                photon_y = pixel_y + 1;
                ref_cont = ref_cont + 1;
                ray_color.a = ray_color.a * BORDER_ALBEDO;
                continue;
            }
            if (pixel_y >= DOWN_BORDER)
            {
                y_step = -1.0 * y_step;
                photon_x = pixel_x;
                photon_y = pixel_y;
                ref_cont = ref_cont + 1;
                ray_color.a = ray_color.a * BORDER_ALBEDO;
                continue;
            }




            // Process obstacle reflecitons           
            if (is_obstacle(screen, pixel_x, pixel_y) == 1)
            {   
                Vec2 normal, normal_circle;
                normal_circle = Normal_circle(pixel_x, pixel_y, obstacle);  // Get normal for circle obstacle
                normal = Normal(screen, pixel_x, pixel_y, SAMPLE_RADIUS);   // Get normal with PCA method
                
                // Caculate refleced ray direction and step
                double dot;
                dot = x_step * normal.x + y_step * normal.y;

                if (dot > 0)                                                // Make sure normal direction is correct. Because y is opposite in screen, so dot should > 0
                {
                    dot = - dot;
                    normal.x = -normal.x; 
                    normal.y = -normal.y;
                }
                x_step = x_step - 2.0 * dot * normal.x;
                y_step = y_step - 2.0 * dot * normal.y;

                photon_x = pixel_x + 1.0 * x_step;
                photon_y = pixel_y - 1.0 * y_step;

                ref_cont = ref_cont + 1;
                ray_color.a = ray_color.a * OBSTACLE_ALBEDO;

                /*
                // Test normal direction
                if (ref_cont == 1 || ref_cont == 2)
                {
                    //printf("ref: %d -- %d  %d  -  %f  %f  -  %f  %f \n", ref_cont, pixel_x, pixel_y, normal.x, normal.y, x_step, y_step);
                    printf("ref: %d -- %d  %d  -  %f  %f  -  %f  %f \n", ref_cont, pixel_x, pixel_y, normal_circle.x, normal_circle.y, normal.x, normal.y);
                }*/
                continue;
            }
        }
    }
};




// MAIN FUNCTION
int main(int argc, char *argv[])
{
    // Init a window with HEIGHT and WIDTH
    Tigr *screen = tigrWindow(WIDTH, HEIGHT, "Tiny Ray Tracing Demo", 0);

    // Init the light source
    bulb.x = WIDTH / 4;
    bulb.y = HEIGHT / 2;
    bulb.r = 10;
    bulb.color = GOLD;

    // Init the obstacle circle
    obstacle.x = WIDTH / 4 * 3;
    obstacle.y = HEIGHT / 2;
    obstacle.r = 100;
    obstacle.color = SILVER;


   

    // Keep the window alive
    while (!tigrClosed(screen)) 
    {
        // Black the window
        tigrClear(screen, BLACK);

        // Scan mouse event and process it
        tigrMouse(screen, &mouse.x, &mouse.y, &mouse.button);
        Process_Mouse_Evevt(&mouse, &bulb, &obstacle);

        // Drow obstacle
        tigrFillCircle(screen, obstacle.x, obstacle.y, obstacle.r, obstacle.color);        
        //tigrFillRect(screen, 600, 200, 150, 75, obstacle.color);                      // Support multi obstacles


        // Drow rays from the light source
        // Key function!!!
        Generate_Rays(screen, bulb, border, RAY_NUM, REF_NUM);


        // Drow light source
        tigrFillCircle(screen, bulb.x, bulb.y, bulb.r, bulb.color);
        //tigrCircle(screen, bulb.x, bulb.y, bulb.r, RED);



        
        // Drow borderlines and show infos
        tigrFillRect(screen, 0, 0, WIDTH, UP_BORDER + 1, BLACK);
        tigrFillRect(screen, 0, DOWN_BORDER - 1 , WIDTH, HEIGHT - DOWN_BORDER + 1, BLACK);
        tigrRect(screen, 0, 0, WIDTH, HEIGHT, WHITE);
        tigrRect(screen, 0, UP_BORDER, WIDTH, DOWN_BORDER - UP_BORDER, WHITE);

        // Show some words on top
        tigrPrint(screen, tfont, 5, 6, WHITE, "Let's have a try with Ray-Tracing");
        // Show infos on the bottom
        tigrPrint(screen, tfont, 5, HEIGHT - 30, WHITE, "Light Bulb");
        tigrPrint(screen, tfont, 75, HEIGHT - 30, WHITE, "Position: (%d, %d)", (int)bulb.x, (int)bulb.y);
        tigrPrint(screen, tfont, 5, HEIGHT - 16, WHITE, "Obstacle");
        tigrPrint(screen, tfont, 75, HEIGHT - 16, WHITE, "Position: (%d, %d)", (int)obstacle.x, (int)obstacle.y);
        


        // Update the screen
        tigrUpdate(screen);
    }

    // Free the window
    tigrFree(screen);
    return 0;
}
