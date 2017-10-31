AWG design procedure (AWG class methods):

1. If no previous AWG class data exists on hard drive, one can start from
scratch by using AWG constructor:

    a = awg;            - creates empty AWG structure
    a = awg([]);        - creates empty AWG structure
    a = awg('AWG_name') - creates empty AWG structure named 'AWG_name'
    
2. If file 'AWG_name.mat' exists, it may contain AWG structure 'AWG_name'.
In this case, MATLAB will attempt to load this file into AWG class variable:

    a = awg('AWG_name') - loads AWG structure from file named 'AWG_name.mat'
    
3. For newly created AWG structures, always initialize AWG parameters (this 
may not be necessary for loaded AWG structures already containing these parameters;
AWG default parameters can be changed by direct editing 'init.m' file located
in AWG classes directory):

    a = init(a);
    
4. For manual design procedure use (this method is interactive and 
it authomatically saves all work before exit in case of your memory lapse,
so that it can be perfectly restored by reloading it from file):

    a = design(a);

--------------------------------------------------------------------------------------------

Helpful comments about varying main parameters:

   Fix Lx, 
   Start from large theta0 (65 deg), small beta1 (20 deg). This will give you 
     big r and di, but small spacing. 
   reduce beta1,
   change theta0 and beta1 till you reach limit 
   (min_r=?mm, spacing before bend<?um, center spacing<?um).
   Increase Lx, .....
   
   (di = straight grating waveguide length)

   Fix Lx and theta0(<68deg), smaller beta1 ==> smaller r, smaller WG center spacing, 
                                                larger di, larger spacing before bend
   Fix Lx and beta1,  smaller theta0 ==> smaller r, larger di, Smaller center spacing

   When theta0>68deg, the above rule may not hold......

   
   *** The spacing between horizontal arms is always decreasing (from top to bottom),
       independent of any parameter!

   di is proportional to 1/r

--------------------------------------------------------------------------------------------

5. Use a = grating(a) to calculate gdsdata for grating layout. The method is interactive,
several parameters are required at the prompt. Half of the slabs are generated as well.
Associated dummies are put on layer 30 (it's hardwired for now).

6. Use a = inout(a) to finalize the AWG design by generating gdsdata for input and output
waveguides on both sides of the array.

7. Use awgsave(a) to save all work or gdssave(a) to generate the GDSII file with AWG data.

