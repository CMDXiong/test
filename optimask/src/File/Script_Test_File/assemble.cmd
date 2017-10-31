!!AWG LOCATIONS FROM AWGLOC.xls Plan 4B
!@clr
!add circle layer = 15 radius = 12990 n_sides = 199 at (0,0)
!add box layer = 15 (-5500,-11100) (5500,11100)
!add box layer = 15 (-11100,-5500) (11100,5500)
!add box layer = 15 (-8750,-8750) (8750,8750)
!@p20ac1layout log off
!select layer 5 all; delete;
!@P20A_C1_BUS_FID log off;

!!add cell mask_stripe -2725.000 9630.000
!!add cell awg1a 0 0
!!add cell awg1a 688.128 0
!!add cell awg1a 0 2490.368

@clr
fill off
!!add circle layer = 15 radius = 6300 n_sides = 199 at (0,0)
!!add wire layer = 99 width=3 at (-6300,0) (6300,0)
!!add wire layer = 99 width=3 at (0,-6300) (0,6300)
!add box layer = 15 (-6300,-6300) (6300,6300)
!add box layer = 15 (-6300,-6300) (6300,-3780)
!add box layer = 15 (-6300,-3780) (6300,-1260)
!add box layer = 15 (-6300,-1260) (6300,1260)
!add box layer = 15 (-6300,1260) (6300,3780)
!add box layer = 15 (-6300,3780) (6300,6300)
!!add cell mask_stripe -2725.000 9630.000
add cell BDRYMARK 0 0

!	!BAR3		CELL	X	Y
	add	cell	AWG12A	-5013.504	131.072
	add	cell	AWG12B	-4489.216	98.304
	add	cell	AWG12C	-3964.928	98.304
	add	cell	AWG12D	-3506.176	98.304
	add	cell	AWG12E	-3047.424	98.304
	add	cell	AWG12F	-2555.904	98.304
	add	cell	AWG12G	-2031.616	98.304
	add	cell	ECHELLE1	-1168.500	0.000
	add	cell	AWG1A3	360.448	0.000
	add	cell	AWG11C	983.040	0.000
	add	cell	AWG2G	1572.864	0.000
	add	cell	AWG2H	2228.224	0.000
	add	cell	AWG2I	2850.816	0.000
	add	cell	AWG2J	3473.408	0.000
	add	cell	AWG2L	4128.768	0.000
	add	cell	AWG2N	4751.360	0.000
					
!	!BAR4				
	add	cell	AWG1A4	-4620.288	2523.136
	add	cell	AWG6A	-3997.696	2523.136
	add	cell	AWG6B	-3375.104	2523.136
	add	cell	AWG8A	-2752.512	2523.136
	add	cell	AWG8B	-2129.920	2523.136
	add	cell	AWG8C	-1507.328	2523.136
	add	cell	AWG8D	-884.736	2523.136
	add	cell	AWG8E	-262.144	2523.136
	add	cell	AWG8F	360.448	2523.136
	add	cell	AWG11D	983.040	2523.136
	add	cell	AWG3F	1474.560	2523.136
	add	cell	AWG3E	2162.688	2523.136
	add	cell	AWG1A4	2850.816	2523.136
	add	cell	AWG3C	3506.176	2523.136
	add	cell	AWG3B	4145.152	2523.136
	add	cell	AWG3A	4784.128	2523.136
					
!	!BAR5				
	add	cell	AWG9A	-4620.288	5046.272
	add	cell	AWG9B	-3997.696	5046.272
	add	cell	AWG9C	-3375.104	5046.272
	add	cell	AWG1A5	-2752.512	5046.272
	add	cell	AWG9D	-2129.920	5046.272
	add	cell	AWG9E	-1507.328	5046.272
	add	cell	AWG9F	-884.736	5046.272
	add	cell	AWG10A	-262.144	5046.272
	add	cell	AWG10B	360.448	5046.272
	add	cell	AWG10C	983.040	5046.272
	add	cell	AWG10D	1605.632	5046.272
	add	cell	AWG4A	2392.064	5046.272
	add	cell	AWG4B	2981.888	5046.272
	add	cell	AWG4C	3604.480	5046.272
	add	cell	AWG4D	4161.536	5046.272
	add	cell	AWG1A5	4718.592	5046.272
					
!	!BAR2				
	add	cell	ECHELLE2	-1795.500	-2520.000
	add	cell	AWG7B1	-262.144	-2490.368
	add	cell	AWG7B2	360.448	-2490.368
	add	cell	AWG7B3	983.040	-2490.368
	add	cell	AWG7B4	1605.632	-2490.368
	add	cell	AWG7B5	2228.224	-2490.368
	add	cell	AWG7B6	2850.816	-2490.368
	add	cell	AWG1A2	3473.408	-2490.368
	add	cell	AWG5C	4096.000	-2490.368
	add	cell	AWG5D	4718.592	-2490.368
					
!	!BAR1				
	add	cell	ECHELLE1	-4670.000	-2520.000
	add	cell	AWG11A	-4718.592	-5079.040
	add	cell	AWG11B	-4025.000	-4521.984
	add	cell	AWG2C	-3080.192	-4554.752
	add	cell	AWG2E	-2326.528	-4784.128
	add	cell	AWG7A1	-1507.328	-4980.736
	add	cell	AWG7A2	-884.736	-4980.736
	add	cell	AWG7A3	-262.144	-4980.736
	add	cell	AWG7A4	360.448	-4980.736
	add	cell	AWG1A1	983.040	-4980.736
	add	cell	AWG7A6	1605.632	-4980.736
	add	cell	AWG7A7	2228.224	-4980.736
	add	cell	AWG7A8	2850.816	-4980.736
	add	cell	AWG7A9	3473.408	-4980.736
	add	cell	AWG5A	4096.000	-4980.736
	add	cell	AWG5B	4718.592	-4980.736

	add	cell	TESTWG	5668.864	0.000
