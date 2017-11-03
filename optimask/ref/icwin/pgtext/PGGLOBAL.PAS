Unit PGGlobal;        { Written by Steven Stern }

{ This unit defines all of the global variables }

Interface

Const
Rev : string = '0.6';
OutFileName : string = 'PGTEXT.CMD';
MaxCoords : integer = 44;

Var
i : integer;
IFile : text;
OFile : Text;
Text_String : string;
Q : real;
XX : real;
YY : real;
Width : real;
Size : real;
Field : string;
Size_String : string;
Justification : string;
Original_Just : string;
Layer : string;
Orient : string;
MCopyright : boolean;
Copyright : boolean;
Rotation : integer;
Mirror : string;
OkToContinue : boolean ;
NumCoords : integer;
Coords : array[1..44,1..2] of real;

Implementation

End.
