#!/opt/bin/perl

system("xt.exe commands.xml commands2.xsl commands.html");
system("xt.exe commands.xml commandindex.xsl toc.html");

print "end\n";





