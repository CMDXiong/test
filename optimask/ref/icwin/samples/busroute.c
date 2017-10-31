#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "showlib.h"

extern int errno;

FILE *fp_sho, *fp_out;
char *sho_file_name, *output_file_name;

typedef struct{ double x, y; } FPOS;
typedef struct{ int dx, dy;} SHIFT_DIR;
SHIFT_DIR shift_table[4][4] =  {{{ 0, 0}, {-1, 1}, { 0, 0}, { 1, 1}},
                                {{-1, 1}, { 0, 0}, {-1,-1}, { 0, 0}},
                                {{ 0, 0}, {-1,-1}, { 0, 0}, { 1,-1}},
                                {{ 1, 1}, { 0, 0}, { 1,-1}, { 0, 0}}};
SHIFT_DIR end_shift_table[4] =  {{ 0, 1}, {-1, 0}, { 0,-1}, { 1, 0}};

/************************ Command line arguments *************************

File names:

   argv[1]  string  tmp directory name (ends in \)

   Example: argv[1]=c:\iced\tmp\ =>
            .SHO file is      c:\iced\tmp\busroute.sho
            Output file is    c:\iced\tmp\busroute.out

Outline wire parameters:

   argv[2]  int  layer NUMBER for "outline" wire

Bus wire parameters:

   argv[3]  int     layer number
   argv[4]  int     wire type
   argv[5]  double  wire width
   argv[6]  double  wire spacing
   argv[7]  int     number of wires in bus

*********************** End Command line arguments **********************/

/*#**********************************************************************/
/*                                                                      */
/*                   void main(int argc, char *argv[])                  */
/*                                                                      */
/**************************-******************-**************************/

void main(int argc, char *argv[])
{
   char *ptr0, *ptr;
   int len, wire_layer, wire_type, n_wires, spine_layer, i, j, dir_code;
   double wire_pitch, wire_width, adjust, wire_spacing, bus_width;
   double offset, x, y, dx, dy;

   /* The following arrays are static to avoid stack overflows when using a 
      16-bit compiler */

   static FPOS pos[200];           /* Max. number of points in a wire is 200 */
   static SHIFT_DIR shift_dir[200];/* Max. number of points in a wire is 200 */
   static char dir[199];         /* Max. number of segments in a wire is 199 */
                                     
   int n_pos = 0;

   /* Check argument count */
 
   if(argc != 8){
      crash("Command line error -- Incorrect argument count\n"
            "Expected 7 arguments -- found %d", argc-1);
      }
   len = strlen(argv[1]);

   /* Form file names from argv[1] */

   sho_file_name = malloc(len + 20);
   output_file_name = malloc(len + 20);
   if(sho_file_name==NULL || output_file_name==NULL){
      crash("Insufficient memory");
      }
   strcpy(sho_file_name, argv[1]);
   strcpy(sho_file_name+len, "BUSROUTE.SHO");

   strcpy(output_file_name, argv[1]);
   strcpy(output_file_name+len, "BUSROUTE.OUT");

   /* Process argv[]'s 2 thru 7 */

   if(sscanf(argv[2], "%d", &spine_layer)==0){
      crash("Command line error in argument 2 -- Outline layer number\n"
            "Expected integer -- found \"%s\"", argv[2]);
      }
   if(spine_layer < 1 || spine_layer > 255){
      crash("Command line error in argument 2  -- Outline layer number\n"
            "Value \"%s\" out of range [1:255]", argv[2]);
      }

   if(sscanf(argv[3], "%d", &wire_layer)==0){
      crash("Command line error in argument 3 -- Bus layer number\n"
            "Expected integer -- found \"%s\"", argv[3]);
      }
   if(wire_layer < 1 || wire_layer > 255){
      crash("Command line error in argument 3 -- Bus layer number\n"
            "Value \"%s\" out of range [1:255]", argv[3]);
      }

   if(sscanf(argv[4], "%d", &wire_type)==0){
      crash("Command line error in argument 4 -- Bus wire type\n"
            "Expected integer -- found \"%s\"", argv[4]);
      }
   if(wire_type!=0 && wire_type!=2){
      crash("Command line error in argument 4 -- Bus wire type\n"
            "Invalid value \"%s\" -- expected 0 or 2", argv[4]);
      }

   if(sscanf(argv[5], "%le", &wire_width)==0){
      crash("Command line error in argument 5 -- Bus wire width\n"
            "Expected real number -- found \"%s\"", argv[5]);
      }
   if(wire_width <= 0){
      crash("Command line error in argument 5 -- Bus wire width\n"
            "Invalid value \"%s\" -- must be greater than 0", argv[5]);
      }

   if(sscanf(argv[6], "%le", &wire_spacing)==0){
      crash("Command line error in argument 6 -- Bus wire spacing\n"
            "Expected real number -- found \"%s\"", argv[6]);
      }
   if(wire_spacing<=0){
      crash("Command line error in argument 6 -- Bus wire spacing\n"
            "Invalid value \"%s\" -- must be greater than 0", argv[6]);
      }

   if(sscanf(argv[7], "%d", &n_wires)==0){
      crash("Command line error in argument 7 -- Number of wires in bus\n"
            "Expected integer -- found \"%s\"", argv[7]);
      }
   if(n_wires < 1){
      crash("Command line error in argument 7 -- Number of wires in bus\n"
            "Invalid value \"%s\" -- must be greater than 0", argv[7]);
      }

   /* Open files */

   errno = 0;  /* errno is set (and used) by the system.  Sometimes a non-zero 
               value is left over from an internal system call */

   fp_sho = fopen(sho_file_name, "r");
   if(fp_sho==NULL){
      sys_crash("Could not open input file \"%s\"", sho_file_name);
      }

   fp_out = fopen(output_file_name, "w+t");
   if(fp_out==NULL){
      sys_crash("Could not open output file \"%s\"", output_file_name);
      }

   /* Read outline wire spine coordinates */

   /* We will ONLY try to check for obvious errors that could be caused
      by using the wrong SHOW command in BUSROUTE.CMD.  We will NOT try
      to check everything. */

   ptr0 = find_line("ADD WIRE");
   if(ptr0==NULL){
      crash("No ADD WIRE command in input file \"%s\"", sho_file_name);
      }
   ptr = skip_passed(ptr0, "ADD WIRE");
   ptr = skip_passed(ptr, "WIDTH=");
   bus_width = atof(ptr);
   ptr = skip_passed(ptr, "AT ");

   while((ptr = skip_passed(ptr, "("))!=NULL){
      if(sscanf(ptr, " %lg, %lg)", &x, &y)!=2){
         crash("Error reading ADD WIRE command\n"
               "Expected (x, y)... -- found \"%s\"", ptr-1);
         }
      pos[n_pos].x = x;
      pos[n_pos].y = y;
      n_pos++;
      }

/****************************** ALGORITHM ******************************

Definitions:
   The SPINE wire is the wire used to outline the bus.
   A BUS wire is one of the wires in the bus.

We now have the coordinates of the spine the wire and we want to use them
to compute the coordinates of the bus wires.  The coordinates of a bus wire
are computed by adding a shift_vector to the coordinates of the spine wire.
The shift_vector for the i-th vertex of the j-th wire is:

   shift_vector.x = (j - (n_wires-1)/2.) * wire_pitch * shift_dir.dx
   shift_vector.y = (j - (n_wires-1)/2.) * wire_pitch * shift_dir.dy

The direction of the shift vector for a given vertex depends on the 
directions of the spine wire segments that meet at the vertex.  

The algorithm has the following steps:

1) Compute direction codes (dir[]) for each segment of the spine wire.
2) Adjust spine wire endpoints to allow for extended ends of bus and spine
   wires.
3) Use these codes and the preprepared look-up tables to compute shift_dir[]
   for each spine wire vertex.
4) Compute (and output) the vertexes for the bus wires.

1) Compute dir[i] for each segment:

   dir[i] = is based on the direction of a vector from pos[i] to pos[i+1].  
   dir[i]=0 => vector points in the positive x direction
   dir[i]=1 => vector points in the positive y direction
   dir[i]=2 => vector points in the negative x direction
   dir[i]=3 => vector points in the negative y direction

   There n_pos vertexes are numbered from 0 to n_pos-1.  There are only 
   n_pos-1 wire segments => n_pos-1 dir[]'s.  We will number them from 
   0 to n_pos-2. */
   
   for(i=0; i<=n_pos-2; i++){
      dx = pos[i+1].x - pos[i].x;
      dy = pos[i+1].y - pos[i].y;
      dir_code = -1;
      if(dy==0){
         if(dx > 0) dir_code = 0;
         else if(dx < 0) dir_code = 2;
         }
      else if(dx==0){
         if(dy > 0) dir_code = 1;
         else if(dy < 0) dir_code = 3;
         }
      if(dir_code==-1){
         crash("Bad wire segment connecting points (%g,%g) and (%g,%g).\n"
               "Points cannot be connected by a horizontal or vertical line.", 
               pos[i].x, pos[i].y, pos[i+1].x, pos[i+1].y);
         }
      dir[i] = dir_code;
      }

/* 2) Adjust spine wire endpoints to allow for extended ends of bus and spine
      wires: */

   adjust = 0.5 * (wire_type==2 ? (bus_width - wire_width) : bus_width);

   dir_code = dir[0];
   if(dir_code==0)      pos[0].x -= adjust;
   else if(dir_code==1) pos[0].y -= adjust;
   else if(dir_code==2) pos[0].x += adjust;
   else if(dir_code==3) pos[0].y += adjust;
      
   dir_code = dir[n_pos-2];
   if(dir_code==0)      pos[n_pos-1].x += adjust;
   else if(dir_code==1) pos[n_pos-1].y += adjust;
   else if(dir_code==2) pos[n_pos-1].x -= adjust;
   else if(dir_code==3) pos[n_pos-1].y -= adjust;

/* 3) Compute shift_dir[] for each spine wire vertex: */

   shift_dir[0] = end_shift_table[dir[0]];
   shift_dir[n_pos-1] = end_shift_table[dir[n_pos-2]];
   for(i=1; i<=n_pos-2; i++){
      shift_dir[i] = shift_table[dir[i-1]][dir[i]];
      }

/* 4) Compute (and output) the vertexes for the bus wires: */

   wire_pitch = wire_spacing + wire_width;
   offset = 0.5 * (n_wires-1) * wire_pitch;
   for(j=0; j<n_wires; j++){
      fprintf(fp_out, "ADD WIRE LAYER=%d WIDTH=%g TYPE=%d AT", 
         wire_layer, wire_width, wire_type);
      for(i=0; i<n_pos; i++){
         x = pos[i].x;
         if(shift_dir[i].dx==1) x += offset;
         else if(shift_dir[i].dx==-1) x -= offset;
         y = pos[i].y;
         if(shift_dir[i].dy==1) y += offset;
         else if(shift_dir[i].dy==-1) y -= offset;
         fprintf(fp_out, " (%g, %g)", x, y);
         }
      offset -= wire_pitch;
      fprintf(fp_out, "\n", x, y);
      }
   exit(0);
   }
