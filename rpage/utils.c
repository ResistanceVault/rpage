/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#include <stdio.h>
#include <math.h>
// #include <exec/types.h>
#include "utils.h"

#define FP_SHFT 0

int qsqr(int i)
{
    int j = 0;
    while(j * j < i)
        j++;

    return j;
}

void swap(char *x, char *y)
{
   char tmp;
   
   tmp = *x;
   *x = *y;
   *y = tmp;
}

short str_find_delimiter(short start, char *str)
{
    short l = (short)strlen(str);
    while(start < l)
    {
        if (str[start] == '\n')
            return start;
        start++;
    }

    return start;
}

/* A utility function to reverse a string */
void reverse(char str[], int length) 
{ 
	int start = 0; 
	int end = length -1; 
	while (start < end) 
	{ 
		swap(str+start, str+end); 
		start++; 
		end--; 
	} 
} 

/* Custom Implementation of itoa() */
char* citoa(int num, char* str, int base) 
{ 
	int i = 0; 
	BOOL isNegative = FALSE; 

	/* Handle 0 explicitely, otherwise empty string is printed for 0 */
	if (num == 0) 
	{ 
		str[i++] = '0'; 
		str[i] = '\0'; 
		return str; 
	} 

	// In standard itoa(), negative numbers are handled only with 
	// base 10. Otherwise numbers are considered unsigned. 
	if (num < 0 && base == 10) 
	{ 
		isNegative = TRUE; 
		num = -num; 
	} 

	// Process individual digits 
	while (num != 0) 
	{ 
		int rem = num % base; 
		str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
		num = num/base; 
	} 

	// If number is negative, append '-' 
	if (isNegative) 
		str[i++] = '-'; 

	str[i] = '\0'; // Append string terminator 

	// Reverse the string 
	reverse(str, i); 

	return str; 
}

int range_adjust(int val, int in_lower, int in_upper, int out_lower, int out_upper)
{
	/* return (val - in_lower) / (in_upper - in_lower) * (out_upper - out_lower) + out_lower */
	return ((val - in_lower) * (out_upper - out_lower)) / (in_upper - in_lower) + out_lower;
}

int clamp(int x, int in_lower, int in_upper){
	return min(max(x, in_lower), in_upper);
}

BOOL point_within_rect(vec2 *pt, rect *r)
{
    if (pt != NULL)
        if (pt->x >= r->sx && pt->y >= r->sy)
            if (pt->x < r->ex && pt->y < r->ey)
                return TRUE;
    return FALSE;
}

BOOL point_within_quad(vec2 *pt, poly *pl)
{
    vec2 pt_list[4];

    pt_list[0].x = pl->p0.x;
    pt_list[0].y = pl->p0.y;
    pt_list[1].x = pl->p1.x;
    pt_list[1].y = pl->p1.y;
    pt_list[2].x = pl->p2.x;
    pt_list[2].y = pl->p2.y;
    pt_list[3].x = pl->p3.x;
    pt_list[3].y = pl->p3.y;

    return point_within_polygon(pt, pt_list, 4);
}

/*  Point VS Polygon test, works in integer.
    Routine by By https://github.com/JustasB */
BOOL point_within_polygon(vec2 *pt, vec2 *pt_list, unsigned short n_pt)
{
    short i, j;
    short pos = 0, neg = 0;
    short x, y, x1, y1, x2, y2, d;
   //Check if a triangle or higher n-gon
    if (n_pt < 3)
    {
        // printf("point_within_polygon(), n_pt < 3 !\n");
        return FALSE;
    }

    //n>2 Keep track of cross product sign changes
 
    for (i = 0; i < n_pt; i++)
    {
        //If point is in the polygon
        if (pt_list[i].x == pt->x && pt_list[i].y == pt->y)
            return TRUE;

        //Form a segment between the i'th point
        x1 = pt_list[i].x;
        y1 = pt_list[i].y;

        //And the i+1'th, or if i is the last, with the first point
        j = i < (n_pt - 1) ? i + 1 : 0;

        x2 = pt_list[j].x;
        y2 = pt_list[j].y;

        x = pt->x;
        y = pt->y;

        //Compute the cross product
        d = (x - x1)*(y2 - y1) - (y - y1)*(x2 - x1);

        if (d > 0) pos++;
        if (d < 0) neg++;

        //If the sign changes, then point is outside
        if (pos > 0 && neg > 0)
            return FALSE;
    }

    //If no change in direction, then on same side of all segments, and thus inside
    return TRUE;
}