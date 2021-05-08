/*
This program will simulate an extremely simplified roller coaster going through a limited selection of roller coaster elements in real time
while displaying various important statistics such as: x and y position, elapsed time, angles, G's, and safety information*/

//NOTE:
//NEED TO DO gcc project.c -lm TO USE ON EUSTIS WITH MATH.H LIBRARY
//!!!!!!!!!!!!!!!!!!!!!!!!!!


//various libraries used
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h> //sys/time.h works for eustis
#include <unistd.h> //used to make usleep() function

//constant for time that passes, in seconds, between each stat display
#define TIME_CONSTANT 0.3

//structure declaration for array of element structures
struct element{
    int type;
    int length;
    int height;
};

/*PRE-CONDITIONS: height of the drop, target angle of descent, pointers to x and y position, and time at start of the coaster (in this case start of the coaster is this element)
POST-CONDITIONS: returns the velocity of the coaster once drop completes
ACTIONS: drop computations is done in 3 sections: 1. from horizontal to angle of descent 2. downwards sloped line at constant angle 3. from target angle to horizontal.
*/
double drop(int height, int angle, double *x, double *y, double initial_time);

/*PRE-CONDITIONS: velocity at start of loop, radius of the loop, pointers to x and y position, and time at start of the coaster
POST-CONDITIONS: returns the velocity of the coaster once loop completes
ACTIONS: runs the computations of the coaster going around the loop
*/
double loop(double initial_velocity, int radius, double *x, double *y, double initial_time);

/*PRE-CONDITIONS: velocity at start of straight section, length of straight section, pointers to x and y position, time at start of the coaster, and type of straight section (brakes or no brakes)
POST-CONDITIONS: returns the velocity of the coaster once straight section completes
ACTIONS: runs the computations of the coaster going through the straight section. Depending on the value of type, this function will treat the straight section as either having brakes or no brakes
*/
double straight(double initial_velocity, int length, double *x, double *y, double initial_time, int type);

/*PRE-CONDITIONS: velocity at current time, radius at current time, current x and y position, current angle, type of element the coaster is currently in, and current time since start of the coaster
POST-CONDITIONS: none
ACTIONS: prints out instantaneous statistics about the coaster.  Is only run inside the drop, straight, or loop functions
*/
void print_stats(double velocity, int radius, double x, double y, double angle, int type, double time);

/*PRE-CONDITIONS: current velocity, current radius, current angle, and type of element the coaster is currently in
POST-CONDITIONS: none
ACTIONS: depending on the type of element the coaster is going through, will compute and display instantaneous vertical G's.
Will also give a rough estimate of safety based on ASTM standards for vertical G's.  Run during the print stats function.
*/
void G_check(double initial_velocity, double radius, double angle, int type);

/*PRE-CONDITIONS: an array of element structures
POST-CONDITIONS: none
ACTIONS: will fill out the order and "shape" of the different elements the user wishes to be in the coaster
*/
void menu(struct element element_list[]);

int main(void){
    //various variables for the program
    int initial_height;
    int drop_angle;
    double velocity;
    double initial_time;
    double ellapsed_time;
    double x, y;
    int i;
    struct element element_list[1000]; //array of structures that will contain the elements the coaster does some stats
    struct timeval now; //this is a part of the time.h library.  time(0) only gives time in seconds, and to make the program run smooth, I needed time
    //in milliseconds.  This declaration allows me to access both seconds and microseconds so the coaster can run in real time.

    //ask user and store starting height of the coaster
    printf("Please enter height of main drop in meters (integer): ");
    scanf("%d", &initial_height);

    //ask user and store angle of descent for the drop of the coaster
    printf("Please enter angle of main drop in degrees (integer, between 30 and 80 preferred): ");
    scanf("%d", &drop_angle);

    //run menu function using the structure array to "create" the coaster
    menu(element_list);

    gettimeofday(&now, NULL);//this command, also part of time.h, gets the number of seconds and microseconds
    initial_time = (double)now.tv_sec + now.tv_usec / 1000000.0; //sets the start time of the coaster with an accuracy of microseconds.  Part of time.h

    //initialize starting location of coaster, at the top of the first drop and horizontal
    x = 0;
    y = initial_height;

    //runs the drop function, ends when coaster reaches bottom of the drop
    velocity = drop(initial_height, drop_angle, &x, &y, initial_time);

    //for loop that runs through the structure array and sends the program to different functions based on which element should be running
    for(i = 0; i < 1000; i++){
        if(element_list[i].type == 4){ //if the coaster completes all elements, exits the loop
            printf("congratulations, your coaster has completed the last element.");
            break;
        }
        else if(element_list[i].type == 1 || element_list[i].type == 3){ //run straight section function as purely straight or brakes depending on type value in structure array
            velocity = straight(velocity, element_list[i].length, &x, &y, initial_time, element_list[i].type);
        }
        else if(element_list[i].type == 2){ //run loop function
            velocity = loop(velocity, element_list[i].height/2, &x, &y, initial_time);
        }

        if(velocity < 0){
            printf("\nYour coaster has stopped on the tracks and didn't complete the course.");
            break;
        }
    }

    return 0;
}

double drop(int height, int angle, double *x, double *y, double initial_time){
    //various variable declarations for the function
    int exit = 0;
    double initial_velocity = 4.0; //velocity coaster has at top of drop to give the coaster a "push" down the first drop
    double theta = 0;
    int top_radius = 5; //radius of arc that goes from horizontal to the desired user angle
    double bottom_radius;
    double velocity;
    double xtemp, ytemp, xoffset;
    double ellapsed_time;
    struct timeval now; //to make time in microseconds work, this structure must be declared in every function where time will be "running"

    //set velocity to velocity at start
    velocity = initial_velocity;

    //this while loop is for going from horizontal to the desired angle of descent
    while(exit == 0){
        //getting current elapsed time in microseconds
        gettimeofday(&now, NULL);
        ellapsed_time = (double)now.tv_sec + now.tv_usec / 1000000.0 - initial_time;

        theta += (velocity * TIME_CONSTANT) / top_radius;//dtheta/dt changes as velocity changes
        //parametric equations for x and y
        *x = top_radius * sin(theta);
        *y = height-top_radius + top_radius * cos(theta);
        velocity = sqrt(initial_velocity * initial_velocity + (2 * 9.81 * (height - *y)));//conservation of energy for current velocity

        //displaying stats and section name for start of drop
        printf("DROP START: \n");
        print_stats(velocity, top_radius, *x, *y, theta*180/3.14, 0, ellapsed_time);

        //once angle is greater than desired angle, exits loop
        if(theta >= angle*3.14/180){
            break;
        }
    }

    //correct angle to be at desired angle
    theta = angle*3.14/180;

    //this while loop is for going downwards at a constant slope downwards during drop at desired angle
    while(exit == 0){
        //getting current elapsed time in microseconds
        gettimeofday(&now, NULL);
        ellapsed_time = (double)now.tv_sec + now.tv_usec / 1000000.0 - initial_time;

        //parametric equations for x and y, add to x and y values from end of last section
        *x += TIME_CONSTANT * velocity * cos(angle*3.14/180);
        *y -= TIME_CONSTANT * velocity * sin(angle*3.14/180);
        velocity = sqrt(initial_velocity * initial_velocity + (2 * 9.81 * (height - *y)));//conservation of energy for current velocity

        //displaying stats and section name for straight part of drop
        printf("DROP STRAIGHT: \n");
        print_stats(velocity, top_radius, *x, *y, theta*180/3.14, 1, ellapsed_time);

        //exits drop once coaster reaches 1/3 of total drop height
        if(*y <= height / 3.0){
            break;
        }
    }

    //sets radius of bottom arc based on current height and desired angle
    bottom_radius = *y/(1-cos(3.14*angle/180));
    //x values that help keep track of x position
    xtemp = *x;
    xoffset = sqrt(2 * bottom_radius * *y - *y * *y);

    while(exit == 0){
        //getting current elapsed time in microseconds
        gettimeofday(&now, NULL);
        ellapsed_time = (double)now.tv_sec + now.tv_usec / 1000000.0 - initial_time;

        theta -= (velocity * TIME_CONSTANT) / bottom_radius;//dtheta/dt changes as velocity changes
        //parametric equations for x and y, use xtemp and xoffset values to ensure correct x position
        *x = xtemp + xoffset - bottom_radius * sin(theta);
        *y = bottom_radius * (1 - cos(theta));
        velocity = sqrt(initial_velocity * initial_velocity + (2 * 9.81 * (height - *y)));//conservation of energy for current velocity

        //displaying stats and section name for bottom of drop
        printf("DROP PULL OUT: \n");
        print_stats(velocity, (int)bottom_radius, *x, *y, theta*180/3.14, 2, ellapsed_time);

        //once the angle is less than or equal to zero, can exit the drop; coaster has leveled out
        if(theta <= 0.0){
            break;
        }
    }

    //makes sure y value is at 0
    *y = 0;
    return velocity; //returns velocity gained from the drop
}

double loop(double initial_velocity, int radius, double *x, double *y, double initial_time){
    //various variables to be used for the loop
    int exit = 0;
    double theta = 0.0;
    double ellapsed_time;
    double velocity;
    double x_initial;
    struct timeval now;//to make time in microseconds work, this structure must be declared in every function where time will be "running"

    //marks the x location of start of the loop
    x_initial = *x;

    while(exit == 0){
        //getting current elapsed time in microseconds
        gettimeofday(&now, NULL);
        ellapsed_time = (double)now.tv_sec + now.tv_usec / 1000000.0 - initial_time;

        theta += (velocity * TIME_CONSTANT) / radius;//dtheta/dt changes as velocity changes
        //parametric equations for x and y for the loop
        *x = x_initial + radius * sin(theta);
        *y = radius - radius * cos(theta);

        if((2 * 9.81 * *y) >= initial_velocity * initial_velocity){ //this checks if the coaster has lost enough velocity to stop and not complete the loop
            printf("Your coaster failed to complete the loop.");
            return -1;//ends whole program because it failed to complete the loop
        }
        else{
            velocity = sqrt((initial_velocity * initial_velocity) - (2 * 9.81 * *y));//conservation of energy to find current energy
        }

        //print name of current element and instantaneous stats
        printf("LOOP: \n");
        print_stats(velocity, radius, *x, *y, theta * 180 / 3.14, 2, ellapsed_time);

        //once theta has gone through 2*pi rad or 360 degrees, exits the loop loop
        if(theta >= 2*3.14){
            break;
        }
    }

    //ensures x and y position are same as where they began because this loop is a circle
    *y = 0;
    *x = x_initial;

    //returns velocity at end of loop
    return velocity;
}

double straight(double initial_velocity, int length, double *x, double *y, double initial_time, int type){
    //various variables to be used for a straight section
    int exit = 0;
    double velocity;
    double ellapsed_time;
    double friction_coeff;
    double x_initial = *x;
    struct timeval now;//to make time in microseconds work, this structure must be declared in every function where time will be "running"

    //depending on the type (brakes or no brakes), will make the coefficient of friction much higher for brake section
    if(type == 1){
        friction_coeff = 0.01; //still a little natural friction if no brakes
    }
    else if(type == 3){
        friction_coeff = 1; // high friction for brakes
    }

    //makes velocity equal to velocity at start of straight section
    velocity = initial_velocity;

    while(exit == 0){
        //getting current elapsed time in microseconds
        gettimeofday(&now, NULL);
        ellapsed_time = (double)now.tv_sec + now.tv_usec / 1000000.0 - initial_time;

        *x += velocity * TIME_CONSTANT;//adds to x position based on velocity * time

        if(2 * friction_coeff * 9.81 * velocity * TIME_CONSTANT >= velocity * velocity){ //checks to see if coaster lost all velocity while on straight section
            printf("Your coaster has stopped on the ");
            //print different message if brakes or no brakes
            if(type == 1){
                printf("straight section");
            }
            else{
                printf("brake section");
            }
            return -1; //ends program if lost all velocity
        }
        else{
            velocity = sqrt(velocity * velocity - 2 * friction_coeff * 9.81 * velocity * TIME_CONSTANT);//conservation of energy
        }

        //prints name and stats based on the section having brakes or no brakes
        if(type == 1){//for no brakes
            printf("STRAIGHT SECTION: \n");
            print_stats(velocity, 0, *x, *y, 0, type, ellapsed_time);
        }
        else if(type == 3){//for brakes
            printf("BRAKE SECTION: \n");
            print_stats(velocity, 0, *x, *y, 0, type, ellapsed_time);
        }

        //once travels the desired length, exits the loop
        if(*x - x_initial >= length){
            break;
        }
    }

    //returns velocity at end of straight section
    return velocity;
}

void print_stats(double velocity, int radius, double x, double y, double angle, int type, double time){
    printf("______________________________\n");//formatting

    //formatting and printing out x position, y position, velocity, time, and angle
    printf("\tX:%20.3lf m\n\tY:%20.3lf m\n", x, y);
    printf("\tvelocity:%13.3lf m/s\n\ttime:%17.3fl s\n", velocity, time);
    printf("\tAngle:%16.3lf degrees\n\t", angle);

    //runs G_check function
    G_check(velocity, radius, angle*3.14/180, type);

    printf("\n______________________________\n\n\n\n\n\n\n\n\n\n\n");//formatting, several \n's make it possible to see only most recent print of stats

    //usleep() is another command of the time.h library.  This makes the program wait a certain number of microseconds.  If this were not here, it would be
    //extremely difficult to read any of the printouts
    usleep(300000);
}

void G_check(double initial_velocity, double radius, double angle, int type){
    double G;//variable for number of G's

    if(type == 1 || type == 3){ //for straight sections and diagonal part of the drop
        G = cos(angle);
    }
    else if(type == 2){ //for the loop and bottom part of the drop
        //G calculation
        G = initial_velocity * initial_velocity/(9.81*radius) + cos(angle);
    }
    else if(type == 0){ // for top part of drop
        //G calculation
        G = -initial_velocity * initial_velocity/(9.81*radius) + cos(angle);
    }

    if (G >= -1 && G < 3.5){//safe range of G's
        printf("G's:%18.3lf safe", G);
    }
    else if ((G >= 3.5 && G < 5.5) || (G >= -2 && G < -1)){ //concerning range of G's
        printf("G's:%18.3lf caution", G);
    }
    else{//all other G's are unsafe
        printf("G's:%18.3lf UNSAFE", G);
    }
}

void menu(struct element element_list[]){
    //variables for defining elements chosen
    int number;//for the element the user chooses
    int length;
    int height;
    int index;//for keeping track of order of the elements

    while(1){
        //ask user to make a selection
        printf("1\tstraight section\n2\tvertical loop\n3\tbrake section\n4\texit\n");
        printf("Select an element to add:");
        scanf("%d", &number);

        if(number == 1){ //choose straight section
            printf("Enter length of straight section in meters (integer): ");
            scanf("%d", &length);
            element_list[index].length = length;
            element_list[index].type = 1;
        }
        else if(number == 2){ //choose loop
            printf("Enter height of loop in meters (integer): ");
            scanf("%d", &height);
            element_list[index].height = height;
            element_list[index].type = 2;
        }
        else if(number == 3){ //choose brakes
            printf("Enter length of break section in meters (integer): ");
            scanf("%d", &length);
            element_list[index].length = length;
            element_list[index].type = 3;
        }
        else if(number == 4){ //user not want any more elements
            element_list[index].type = 4;
            break; //exits elements selection
        }

        index++;//increases by 1 every time the user selects an element
    }
}
