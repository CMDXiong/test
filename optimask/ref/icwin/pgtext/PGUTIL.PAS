Unit PGutil;        { Written by Steven Stern }

Interface

Procedure Read_show_file;
Procedure Init_Coords;
Procedure Check_Justification;
Procedure Check_Mirror;
Procedure Write_Character;
Procedure WriteHeader;
Procedure OpenOutFile;
Procedure CloseOutFile;
Procedure Write_Character_Header;
Procedure WriteEndHeader;

Implementation

Uses PGglobal, PGfont1;

Procedure Init_Coords;
Begin
      For i:= 1 to MaxCoords do
      Begin
      Coords[i,1]:=0;
      Coords[i,2]:=0;
      end;
end;

Procedure Check_Justification;
Begin
If Justification='LT' then
Begin
If Rotation=0 then  YY:=YY-Size;
If Rotation=1 then  XX:=XX+Size;
If Rotation=2 then
Begin
XX:=XX-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1));
end;
If Rotation=3 then
Begin
YY:=YY-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1));
end;
end;
If Justification='CT' then
Begin
If Rotation=0 then
Begin
XX:=XX-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1))/2;
YY:=YY-Size;
end;
If Rotation=1 then
Begin
XX:=XX+Size;
YY:=YY-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1))/2;
end;
If Rotation=2 then
Begin
XX:=XX-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1))/2;
end;
If Rotation=3 then
Begin
YY:=YY-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1))/2;
end;
end;
If Justification='RT' then
Begin
If Rotation=0 then
Begin
XX:=XX-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1));   YY:=YY-Size;
end;
If Rotation=1 then
Begin
XX:=XX+Size; YY:=YY-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1));
end;
If Rotation=2 then
Begin
end;
If Rotation=3 then
Begin
end;
end;
If Justification='LC' then
Begin
If Rotation=0 then  YY:=YY-Size/2;
If Rotation=1 then  XX:=XX+Size/2;
If Rotation=2 then
Begin
XX:=XX-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1));
YY:=YY-Size/2;
end;
If Rotation=3 then
Begin
XX:=XX+Size/2;
YY:=YY-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1));
end;
end;
If Justification='CC' then
Begin
If (Rotation=0) or (Rotation=2) then
Begin
XX:=XX-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1))/2;
YY:=YY-Size/2;
end;
If (Rotation=1) or (Rotation=3) then
Begin
XX:=XX+Size/2;
YY:=YY-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1))/2;
end;
end;
If Justification='RC' then
Begin
If Rotation=0 then
Begin
XX:=XX-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1));
YY:=YY-Size/2;
end;
If Rotation=1 then
Begin
XX:=XX+Size/2;
YY:=YY-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1));
end;
If Rotation=2 then
Begin
YY:=YY-Size/2;
end;
If Rotation=3 then
Begin
XX:=XX+Size/2;
end;
end;
If Justification='LB' then
Begin
If Rotation=0 then
Begin
end;
If Rotation=1 then
Begin
end;
If Rotation=2 then
Begin
XX:=XX-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1));
YY:=YY-Size;
end;
If Rotation=3 then
Begin
XX:=XX+Size;
YY:=YY-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1));
end;
end;
If Justification='CB' then
Begin
If Rotation=0 then
Begin
XX:=XX-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1))/2;
end;
If Rotation=1 then
Begin
YY:=YY-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1))/2;
end;
If Rotation=2 then
Begin
XX:=XX-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1))/2;
YY:=YY-Size;
end;
If Rotation=3 then
Begin
XX:=XX+Size;
YY:=YY-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1))/2;
end;
end;
If Justification='RB' then
Begin
If Rotation=0 then
Begin
XX:=XX-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1));
end;
If Rotation=1 then
Begin
YY:=YY-(Width*Q*Length(Text_String)+2.0*Q*(Length(Text_String)-1));
end;
If Rotation=2 then
Begin
YY:=YY-Size;
end;
If Rotation=3 then
Begin
XX:=XX+Size;
end;
end;
end;

Procedure Check_Mirror;
Begin
 If (Mirror='MX') and ((Rotation=0) or (Rotation=2)) or
    (Mirror='MY') and ((Rotation=1) or (Rotation=3)) then
 Case Justification[1] of
  'L' : Justification[1]:='R';
  'R' : Justification[1]:='L';
  'L' : Justification[1]:='R';
  'R' : Justification[1]:='L';
  'L' : Justification[1]:='R';
  'R' : Justification[1]:='L';
 end;
 If (Mirror='MX') and ((Rotation=1) or (Rotation=3)) or
    (Mirror='MY') and ((Rotation=0) or (Rotation=2)) then
 Case Justification [2] of
  'T' : Justification[2]:='B';
  'B' : Justification[2]:='T';
  'T' : Justification[2]:='B';
  'B' : Justification[2]:='T';
  'T' : Justification[2]:='B';
  'B' : Justification[2]:='T';
 end;
end;
Procedure Read_Until_Blank(Current_line:string;Startpos:integer);
Var
Found_blank : boolean;
Charac : string;
Begin
   Field:= '';
   Found_blank := false;
   While not Found_blank do
   Begin
   Charac:=Copy(Current_line,Startpos,1);
   If Charac = ' ' then  Found_blank:=True else
   Field:=Field + Charac;
   Startpos:=Startpos+1;
   end;
   end;

Procedure Read_show_file;

Const
  In_File_Name  : String = 'PGTEXT.SHO';

Var
  IFile : Text;
  XYcoords : string;
  Code : integer;
  Broken_line : boolean;
  Ch : Char;
  j : integer;
  First_dquote : integer;
  Second_dquote : integer;
  Current_line : string;
  Reverse_line : string;
  Current_line_size : integer;
  Read_line : string;

Begin
        Current_line:='';
        Reverse_line:='';
        Text_String:='';
        Broken_line:=True;
        MCopyright:=false;
        Copyright:=false;
        Orient:='R0';
        Mirror:='None';
        Original_Just:='';
        Justification:='';
        Width:=7.0;
        Rotation:=0;
        Assign(IFile,In_File_Name);
        Reset(IFile);
        If IOResult = 0 then
 Begin
        Readln(IFile); { skip first text line }
        Readln(IFile); { skip second blank line }
        while not Eof(IFile) do    { read next command line and any  }
  begin                            { continuation lines following it }
        While Broken_line do
        Begin
         Readln (IFile,Read_line);
         If Read_line[Length(Read_line)] = '&' then
         Begin
          Read_line[Length(Read_line)]:= ' ';     { remove ampersand }
          Broken_line:=True;
         end else
         Begin
          Broken_line:=false;
         end;
         Current_line:=Current_line + Read_line;
        end;
        If not Eof(Ifile) then Readln(IFile);  { Skip extra commands }
  end;
        If Pos('ADD  TEXT',Current_line) = 1 then
        Begin
        Current_line_size:=Length(Current_line);
        For i:=Current_line_size downto 1 do
        Reverse_line:=Reverse_line+Current_line[i];
        First_dquote:=Pos('"',Current_line)+1;
        Second_dquote:=Current_line_size-Pos('"',Reverse_line)+1;
        Text_String:=Copy(Current_line,First_dquote,Second_dquote-First_dquote);
        If Text_String='@M' then
        Begin
         MCopyright:=True;
         Text_String:='@';
         Width:=10;
        end;
        If Text_String='@C' then
        Begin
         Copyright:=True;
         Text_String:='@';
         Width:=10;
        end;
        Current_line:=Copy(Current_line,Second_dquote+1,Current_line_size);
        i:=Pos('LAYER ',Current_line);
        Read_Until_Blank(Current_line,i+6);
        Layer:=Field;
        Current_line:=Copy(Current_line,i+7+Length(Layer),Length(Current_line));
        i:=Pos('SIZE=',Current_line);
        Read_Until_Blank(Current_line,i+5);
        Size_String:=Field;
        Val(Field,Size,code);
        i:=Pos('JUST=',Current_line);
        Read_Until_Blank(Current_line,i+5);
        Original_Just:=Field;
        Justification:=Original_Just;
        i:=Pos('MX',Current_line);
        If i <> 0 then Mirror:='MX';
        i:=Pos('MY',Current_line);
        If i <> 0 then Mirror:='MY';
        i:=Pos('R0',Current_line);
        If i <> 0 then Orient:='R0';
        If Orient = 'R0' then Rotation:=0;
        i:=Pos('R1',Current_line);
        If i <> 0 then Orient:='R1';
        If Orient = 'R1' then Rotation:=1;
        i:=Pos('R2',Current_line);
        If i <> 0 then Orient:='R2';
        If Orient = 'R2' then Rotation:=2;
        i:=Pos('R3',Current_line);
        If i <> 0 then Orient:='R3';
        If Orient = 'R3' then Rotation:=3;
        i:=Pos('(',Current_line);
        j:=Pos(')',Current_line);
        XYcoords:=Copy(Current_line,i+1,j-i-1);
        i:=Pos(',',XYcoords);
        Val(Copy(XYcoords,1,i-1),XX,code);
        Val(Copy(XYcoords,i+1,Length(XYcoords)),YY,code);
        end else
        Writeln('ADD TEXT command not found in input file');
 end else
        Writeln('Unable to Open PGTEXT.SHO input file');
        Close(IFile);

end;

Procedure WriteHeader;
begin
  Writeln(OFile,'! PGTEXT.CMD - ICED PG''able Text Generator');
  Writeln(OFile,'!');
  Writeln(OFile,'! Version ',Rev,' (C) by Steven Stern');
  Writeln(OFile,'!');
  Writeln(OFile,'! Text String : ',Text_String);
  Writeln(OFile,'!      Layer  : ',Layer);
  Writeln(OFile,'!      Size   : ',Size_String);
  Writeln(OFile,'!      Just.  : ',Original_Just);
  Writeln(OFile,'!      Orient.: ',Orient);
  Writeln(Ofile,'!      Mirror : ',Mirror);
  Writeln(OFile,'!      Origin : ',XX,',',YY);
  Writeln(OFile,'!');
end;

Procedure OpenOutFile;
Begin
    OkToContinue := true;
    Assign (OFile,OutFileName);
    ReWrite (OFile);
    If IOResult <> 0 then
      OkToContinue := False;
end;

Procedure CloseOutFile;
Begin
     Close (OFile);
end;

Procedure Write_Character_Header;
Begin
      Writeln(OFile,'!');
      Writeln(OFile,'! Create character "',Text_String[i],'"');
      Writeln(OFile,'ADD POLYGON LAYER=',Layer,' AT     &');
end;

Procedure WriteEndHeader;
begin
      Writeln(OFile,'!');
      Writeln(OFile,'! End of PGTEXT.CMD file.');
end;


Procedure Write_Character;
var
j : integer;
Begin
  For j:=1 to Numcoords-1 do
    begin
      If (Rotation = 1) or (Rotation = 3) then
      begin
      Writeln (OFile,'(',XX-Q*Coords[j,2]:12:4,',',YY+Q*Coords[j,1]:12:4,')',' &');
      end else
      begin
      Writeln (OFile,'(',XX+Q*Coords[j,1]:12:4,',',YY+Q*Coords[j,2]:12:4,')',' &');
      end;
    end;
  j:=Numcoords;
  If (Rotation = 1) or (Rotation = 3) then
    begin
      Writeln (OFile,'(',XX-Q*Coords[j,2]:12:4,',',YY+Q*Coords[j,1]:12:4,')');
    end else
    begin
      Writeln (OFile,'(',XX+Q*Coords[j,1]:12:4,',',YY+Q*Coords[j,2]:12:4,')');
    end;
end;

End.
