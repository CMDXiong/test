Program PGtext;        { Written by Steven Stern }
{$M 65520 , 8192, 655360}

{ PGtext creates PG'able text within ICED-32 based on an initially placed
  text label. The PG'able text is created based on the same Layer, Size,
  Justification and Rotation originally used to create the text label.  }

Uses
 PGglobal, PGutil, PGfont1;


Begin   { Main Body PgText }

 Read_show_file;          { read PGTEXT.SHO file }
 Q:= Size/10;
 Check_Mirror;
 Check_Justification;
 OpenOutFile;
 If OkToContinue then
 Begin
  Init_Coords;
  WriteHeader;
  If MCopyright then Write_MCopyright else
  If Copyright then Write_Copyright else
  For i := 1 to length(Text_String) do
   Begin
    Case UpCase(Text_String[i]) of
           'A' : Write_A;
           'B' : Write_B;
           'C' : Write_C;
           'D' : Write_D;
           'E' : Write_E;
           'F' : Write_F;
           'G' : Write_G;
           'H' : Write_H;
           'I' : Write_I;
           'J' : Write_J;
           'K' : Write_K;
           'L' : Write_L;
           'M' : Write_M;
           'N' : Write_N;
           'O' : Write_O;
           'P' : Write_P;
           'Q' : Write_Q;
           'R' : Write_R;
           'S' : Write_S;
           'T' : Write_T;
           'U' : Write_U;
           'V' : Write_V;
           'W' : Write_W;
           'X' : Write_X;
           'Y' : Write_Y;
           'Z' : Write_Z;
           '1' : Write_1;
           '2' : Write_2;
           '3' : Write_3;
           '4' : Write_4;
           '5' : Write_5;
           '6' : Write_6;
           '7' : Write_7;
           '8' : Write_8;
           '9' : Write_9;
           '0' : Write_0;
           'Y' : Write_Y;
           '~' : Write_Tilda;
           '`' : Write_Left_Quote;
           '!' : Write_Exclamation;
           '@' : Write_At;
           '#' : Write_Pound;
           '$' : Write_Dollar;
           '%' : Write_Percent;
           '^' : Write_Up_Arrow;
           '&' : Write_Ampersand;
           '*' : Write_Star;
           '(' : Write_Left_Parenthesis;
           ')' : Write_Right_Parenthesis;
           '_' : Write_Underscore;
           '-' : Write_Minus;
           '=' : Write_Equal;
           '+' : Write_Plus;
           '[' : Write_Left_Bracket;
           ']' : Write_Right_Bracket;
           '{' : Write_Left_Brace;
           '}' : Write_Right_Brace;
           '|' : Write_Split_Bar;
           '\' : Write_Back_Slash;
           ':' : Write_Colon;
           ';' : Write_Semi_Colon;
           '"' : Write_Double_Quote;
           '''': Write_Right_Quote;
           '<' : Write_Left_Arrow;
           '>' : Write_Right_Arrow;
           ',' : Write_Coma;
           '.' : Write_Period;
           '?' : Write_Question;
           '/' : Write_Slash;
    end; { case of }

           If (Rotation=1) or (Rotation=3) then
           Begin
           YY := YY+Q*9.0;
           end else
           Begin
           XX := XX+Q*9.0;
           end;
   end;  { for i }
        WriteEndHeader;
        CloseOutFile;
  end else
  Begin
  Writeln('Unable to Open Output File');  { if OkToContinue }
  CloseOutFile;
  end;
End. { Main Body }
