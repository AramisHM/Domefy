/*
 *	Aramis Hornung Moraes - (23.08.2012)
 *	Please don't remove this information.
 *	This is a ANSI C program written by Aramis Hornung Moraes
 *	to calculate the distance between two points in the 
 *	three-dimensional space and to calculate the angle between two points in the 
 *	three-dimensional space.
 *	I would appreciate  if you keep this credits.
 *	Project duration until first release -  Started (23.08.2012) - Ended (01.09.2012) (10 Days) 
 *	Organizing code - Started (02.09.2012) - Ended (1.11.2011)
 *	Version 1.0.0.0
 */

/* You may ask yourself: why would you use this function and not the Game Engine's quaternions? Well this is pure C and the only dependency here is the standard
math.h library. */

#include <math.h>
#include <stdio.h>

#define PI 3.14159265
#define todeg (180/PI)* /* Converts a value in radians to degrees */
#define torad (PI/180)* /* Converts a value in degrees to radians */

// The function have been modifyed to return the z axis variation from two points.

double triangulateTarget(double x0, double y0, double z0, double x1, double y1, double z1)
{
	//double	x0 = 0, /* "Point A x position" */
    //y0 = 0, /* "Point A y position" */
    //z0 = 0, /* "Point A z position" */
    //x1 = 0, /* "Point B x position" */
    //y1 = 0, /* "Point B y position" */
    //z1 = 0, /* "Point B z position" */
    double maximum_distance,
    distance,
    limit, /* How near objects are adopting the reference of maximum distance [0-1] */
    Xx, /* "Distance x betweent point A to B on X axis" */
    Xy, /* "Distance y betweent point A to B on X axis" */
    Yx, /* "Distance x betweent point A to B on Y axis" */
    Yy, /* "Distance y betweent point A to B on Y axis" */
    Zx, /* "Distance x betweent point A to B on Z axis" */
    Zy, /* "Distance y betweent point A to B on Z axis" */
    hypotenuse_X_axis,
    hypotenuse_Y_axis,
    hypotenuse_Z_axis,
    sine_X_axis,
    cosine_X_axis,
    sine_Y_axis,
    cosine_Y_axis,
    sine_Z_axis,
    cosine_Z_axis,
    angle_X_axis,
    angle_Y_axis,
    angle_Z_axis,
    rotation_X_axis = -1, /* Aditional rotation on X axis (optional). */
    rotation_Y_axis = -1, /* Aditional rotation on Y axis (optional). */
    rotation_Z_axis = -1, /* Aditional rotation on Z axis (optional). */
    rotated_angle_X_axis,
    rotated_angle_Y_axis,
    rotated_angle_Z_axis,
    rotated_sine_X_axis,
	rotated_cosine_X_axis,
    rotated_sine_Y_axis,
    rotated_cosine_Y_axis,
    rotated_sine_Z_axis,
    rotated_cosine_Z_axis,
    RXx, /* "Distance x betweent point A to B on X axis" */
    RXy, /* "Distance y betweent point A to B on X axis" */
    RYx, /* "Distance x betweent point A to B on Y axis" */
    RYy, /* "Distance y betweent point A to B on Y axis" */
    RZx, /* "Distance x betweent point A to B on Z axis" */
    RZy, /* "Distance y betweent point A to B on Z axis" */
	result_hypotenuse_X_axis,
	result_hypotenuse_Y_axis,
	result_hypotenuse_Z_axis,
	result_sine_X_axis = 0,
	result_cosine_X_axis = 0,
    result_sine_Y_axis = 0,
    result_cosine_Y_axis = 0,
    result_sine_Z_axis = 0,
    result_cosine_Z_axis = 0,
    result_angle_X,
    result_angle_Y,
    result_angle_Z;
    
	/* Input */
	/*
	printf("Position of point A:\n x = "); scanf("%lf",&x0);
	printf(" y = "); scanf("%lf",&y0);
	printf(" z = "); scanf("%lf",&z0);
	printf("\nPosition of point B:\n x = "); scanf("%lf",&x1);
	printf(" y = "); scanf("%lf",&y1);
	printf(" z = "); scanf("%lf",&z1);
	printf("\nSet the maximum distance to check if the points are inside this limit: "); scanf("%lf",&maximum_distance);
	printf("\nPoint A plus extra rotation on X axis in degrees: ");

	while(rotation_X_axis < 0 || rotation_X_axis > 360)
		scanf("%lf", &rotation_X_axis);
	printf("Point A plus extra rotation on Y axis in degrees: ");
	while(rotation_Y_axis < 0 || rotation_Y_axis > 360)
		scanf("%lf", &rotation_Y_axis);
	printf("Point A plus extra rotation on Z axis in degrees: ");
	while(rotation_Z_axis < 0 || rotation_Z_axis > 360)
		scanf("%lf", &rotation_Z_axis);
	*/
	maximum_distance = 100;
	rotation_X_axis = rotation_Y_axis = rotation_Z_axis = 0;

    
	/* Calculating */
	/* Distance between the two points*/
	distance = sqrt(( ((x1-x0)*(x1-x0)) + ((y1-y0)*(y1-y0)) + ((z1-z0)*(z1-z0)) ));
	limit = (1 - (((100*distance)/maximum_distance)*0.01));
	/* Angles */
	/* X axis */
	Xx = (y1-y0);
	Xy = (z1-z0);
	hypotenuse_X_axis = hypot(Xx,Xy);
	sine_X_axis = (Xy/hypotenuse_X_axis);
	cosine_X_axis = (Xx/hypotenuse_X_axis);
	angle_X_axis = todeg(atan2(Xy,Xx));
	if(angle_X_axis < 0)
		angle_X_axis = (360 + angle_X_axis);
	rotated_angle_X_axis = (angle_X_axis - rotation_X_axis);
	if(rotated_angle_X_axis < 0)
		rotated_angle_X_axis = 360 + rotated_angle_X_axis;
	rotated_sine_X_axis = sin(torad(rotated_angle_X_axis));
	rotated_cosine_X_axis = cos(torad(rotated_angle_X_axis));
	/* Y axis */
	Yx = (x1-x0);
	Yy = (z1-z0);
	hypotenuse_Y_axis = hypot(Yx,Yy);
	sine_Y_axis = (Yy/hypotenuse_Y_axis);
	cosine_Y_axis = (Yx/hypotenuse_Y_axis);
	angle_Y_axis = todeg(atan2(Yy,Yx));
	if(angle_Y_axis < 0)
		angle_Y_axis = (360 + angle_Y_axis);
	rotated_angle_Y_axis = (angle_Y_axis - rotation_Y_axis);
	if(rotated_angle_Y_axis < 0)
		rotated_angle_Y_axis = 360 + rotated_angle_Y_axis;
	rotated_sine_Y_axis = sin(torad(rotated_angle_Y_axis));
	rotated_cosine_Y_axis = cos(torad(rotated_angle_Y_axis));
	/* Z axis */
	Zx = (x1-x0);
	Zy = (y1-y0);
	hypotenuse_Z_axis = hypot(Zx,Zy);
	sine_Z_axis = (Zy/hypotenuse_Z_axis);
	cosine_Z_axis = (Zx/hypotenuse_Z_axis);
	angle_Z_axis = todeg(atan2(Zy,Zx));
	if(angle_Z_axis < 0)
		angle_Z_axis = (360 + angle_Z_axis);
	rotated_angle_Z_axis = (angle_Z_axis - rotation_Z_axis);
	if(rotated_angle_Z_axis < 0)
		rotated_angle_Z_axis = 360 + rotated_angle_Z_axis;
	rotated_sine_Z_axis = sin(torad(rotated_angle_Z_axis));
	rotated_cosine_Z_axis = cos(torad(rotated_angle_Z_axis));
	/* Before Z axis rotation */
	result_angle_Z = (angle_Z_axis - rotation_Z_axis);
	if(result_angle_Z < 0)
		result_angle_Z = (360 + result_angle_Z);
	rotated_sine_Z_axis = sin(torad(result_angle_Z));
	rotated_cosine_Z_axis = cos(torad(result_angle_Z));
	RXx = hypotenuse_Z_axis * sin(torad(result_angle_Z));
	RXy = Xy;
	result_hypotenuse_X_axis = hypot(RXx,RXy);
	result_angle_X = todeg(atan2(RXy,RXx));
	RYx = (hypotenuse_Z_axis * cos((torad(result_angle_Z))));
	RYy = Yy;
	result_hypotenuse_Y_axis = hypot(RYx,RYy);
	result_angle_Y = todeg(atan2(RYy,RYx));
	RZx = (hypotenuse_Z_axis * cos((torad(result_angle_Z))));
	RZy = (hypotenuse_Z_axis * sin((torad(result_angle_Z))));
	if(result_angle_X < 0)
		result_angle_X = (360 + result_angle_X);
	if(result_angle_Y < 0)
		result_angle_Y = (360 + result_angle_Y);
	/* Before Y axis rotation */
	result_angle_Y = (result_angle_Y - rotation_Y_axis);
	if(result_angle_Y < 0)
		result_angle_Y = (360 + result_angle_Y);
	rotated_sine_Y_axis = sin(torad(result_angle_Y));
	rotated_cosine_Y_axis = cos(torad(result_angle_Y));
	RXx = RXx;
	RXy = (result_hypotenuse_Y_axis * sin(torad(result_angle_Y)));
	result_hypotenuse_X_axis = hypot(RXx,RXy);
	result_angle_X = todeg(atan2(RXy,RXx));
	RYx = (result_hypotenuse_Y_axis * cos(torad(result_angle_Y)));
	RYy = (result_hypotenuse_Y_axis * sin(torad(result_angle_Y)));
	RZx = (result_hypotenuse_Y_axis * cos(torad(result_angle_Y)));
	RZy = (hypotenuse_Z_axis * sin(torad(result_angle_Z)));
	result_hypotenuse_Z_axis = hypot(RZx,RZy);
	result_angle_Z = todeg(atan2(RZy,RZx));
	if(result_angle_X < 0)
		result_angle_X = (360 + result_angle_X);
	if(result_angle_Z < 0)
		result_angle_Z = (360 + result_angle_Z);
	/* Before X axis rotation */
	result_angle_X = (result_angle_X - rotation_X_axis);
	if(result_angle_X < 0)
		result_angle_X = (360 + result_angle_X);
	rotated_sine_X_axis = sin(torad(result_angle_X));
	rotated_cosine_X_axis = cos(torad(result_angle_X));
	RXx = (result_hypotenuse_X_axis * cos(torad(result_angle_X)));
	RXy = (result_hypotenuse_X_axis * sin(torad(result_angle_X)));
	RYx = RYx;
	RYy = (result_hypotenuse_X_axis * sin(torad(result_angle_X)));
	result_hypotenuse_Y_axis = hypot(RYx,RYy);
	result_angle_Y = todeg(atan2(RYy,RYx));
	RZx = RZx;
	RZy = (result_hypotenuse_X_axis * cos(torad(result_angle_X)));
	result_hypotenuse_Z_axis = hypot(RZx,RZy);
	result_angle_Z = todeg(atan2(RZy,RZx)); 
	if(result_angle_Y < 0)
		result_angle_Y = (360 + result_angle_Y);
	if(result_angle_Z < 0)
		result_angle_Z = (360 + result_angle_Z);

	/* Print the result on the console */
	//printf("\nThe distance in unities between the two points is: %f\n", distance);
	//printf("Those points are %f in a scale of 0 to 1 near to each other\nadopting the reference of %f that is the maximum distance.\n\n", limit, maximum_distance);
	//printf("result angle X: %f\n", result_angle_X);
	//printf("result angle Y: %f\n", result_angle_Y);
	//printf("result angle Z: %f\n", result_angle_Z);
	//printf("\n");
	//fflush(stdin);
	//printf("Press enter to exit"); getchar();

	return result_angle_Y; // thats what we want at the moment :)
}