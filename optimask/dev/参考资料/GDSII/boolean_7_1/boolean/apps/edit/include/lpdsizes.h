/*
 *	Program			lpdsizes.h
 *	Purpose			This header file contains all the coordinate for the gadgets
 *						in the layer properties dialog.
 * Author         Mollien J.C.
 *		"Sunday morning I'm waking up, can't even focus on my coffee cup
 *     I don't know whose bed I'm in, where do I start, where do I begin?"
 * Modified			13-10-1997
 * Created			13-10-1997
*/

#ifndef LPDSIZES_H
#define LPDSIZES_H

#define LPD_B_W 	20
#define LPD_B_H 	20

#ifdef __UNIX__
// unix sizes

	#define LPD_WH			35		// hoogte van de regels

// number messages
	#define LPD_NUMBER_X	15
	#define LPD_NUMBER_Y 40

// name texts
	#define LPD_NAME_X 			70
	#define LPD_NAME_W   		140
	#define LPD_NAME_H   		30

// Available checkboxes
	#define LPD_AVAILABLE_X		230

// Visible checkboxes
	#define LPD_VISIBLE_X 		280

// Selectable checkboxes
	#define LPD_SELECTABLE_X 	340

// readlayer checkboxes
	#define LPD_READLAYER_X 	390

// Border color buttons
	#define LPD_BORDERBOX_X 	490

// Fill color buttons
	#define LPD_FILLBOX_X 		420

// Pattern buttons
	#define LPD_PATTERNBOX_X 	530
	#define LPD_PATTERNBOX_W	50

	#define LPD_MAPIN_X 			600
	#define LPD_MAP_W 			30
	#define LPD_MAPOUT_X 		640



// Labels
	#define LPD_NUMBERL_Y 		5

// common defines for buttons

	#define LPD_BUTTON_W  		110
	#define LPD_BUTTON_H	 	 	25

//	_buttonOK
	#define LPD_BUTTONOK_X		700
	#define LPD_BUTTONOK_Y 		 50

//	_buttonCANCEL
	#define LPD_BUTTONCANCEL_Y  80

//	_buttonHIDE
	#define LPD_BUTTONHIDE_Y	120

//	_buttonSELECT
	#define LPD_BUTTONSELECT_Y	150

//	_buttonREAD
	#define LPD_BUTTONREAD_Y	180

//	_buttonPREV
	#define LPD_BUTTONPREV_Y	220

//	_buttonNEXT
	#define LPD_BUTTONNEXT_Y	260

// _groupbox1
	#define LPD_GROUPBOX1_X 5
	#define LPD_GROUPBOX1_Y 25
	#define LPD_GROUPBOX1_W LPD_BUTTONOK_X + LPD_BUTTON_W + 10// minimum is LPD_BUTTONOK_X + LPD_BUTTON_W + corners
	#define LPD_GROUPBOX1_H 370

//	_panel1
	#define LPD_PANEL1_W LPD_GROUPBOX1_W + 10
	#define LPD_PANEL1_H LPD_GROUPBOX1_H + LPD_GROUPBOX1_Y +10

#else
// windows sizes

	#define LPD_WH			23		// hoogte van de regels

// number messages
	#define LPD_NUMBER_X	15
	#define LPD_NUMBER_Y 30
// name texts
	#define LPD_NAME_X 			60
	#define LPD_NAME_W   		140
	#define LPD_NAME_H   		17
// Available checkboxes
	#define LPD_AVAILABLE_X		230
// Visible checkboxes
	#define LPD_VISIBLE_X 		275
// Selectable checkboxes
	#define LPD_SELECTABLE_X 	315
// readlayer checkboxes
	#define LPD_READLAYER_X 	355
// Border color buttons
	#define LPD_BORDERBOX_X 	450
// Fill color buttons
	#define LPD_FILLBOX_X 		400
// Pattern buttons
	#define LPD_PATTERNBOX_X 	500
	#define LPD_PATTERNBOX_W	30

	#define LPD_MAPIN_X 			540
	#define LPD_MAP_W 			30
	#define LPD_MAPOUT_X 		580

// Labels
	#define LPD_NUMBERL_Y 5

// common defines for buttons

	#define LPD_BUTTON_W  		100
	#define LPD_BUTTON_H	 	 	17

//	_buttonOK
	#define LPD_BUTTONOK_X		650
	#define LPD_BUTTONOK_Y 		 50
//	_buttonCANCEL
	#define LPD_BUTTONCANCEL_Y  70
//	_buttonHIDE
	#define LPD_BUTTONHIDE_Y	100
//	_buttonSELECT
	#define LPD_BUTTONSELECT_Y	120
//	_buttonREAD
	#define LPD_BUTTONREAD_Y	140
//	_buttonPREV
	#define LPD_BUTTONPREV_Y	170
//	_buttonNEXT
	#define LPD_BUTTONNEXT_Y	210
// _groupbox1
	#define LPD_GROUPBOX1_X 5
	#define LPD_GROUPBOX1_Y 15
	#define LPD_GROUPBOX1_W LPD_BUTTONOK_X + LPD_BUTTON_W + 10// minimum is LPD_BUTTONOK_X + LPD_BUTTON_W + corners
	#define LPD_GROUPBOX1_H 255
//	_panel1
	#define LPD_PANEL1_W LPD_GROUPBOX1_W + 10
	#define LPD_PANEL1_H LPD_GROUPBOX1_H + LPD_GROUPBOX1_Y +10

#endif // ifdef __UNIX__  .. else ..

#endif
