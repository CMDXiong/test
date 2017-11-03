<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
               version='1.0'>

<xsl:include href="./html/docbook.xsl"/>

<xsl:template match="/">
	<html>
	<head>
	   <TITLE>Command Documentation</TITLE>
	</head>
	<body bgcolor="#99CCFF">
	<h1>Commands by catagorie</h1>
        <xsl:apply-templates select ="commands" mode="toc"/>
	<h1>commands sorted</h1>
        <xsl:apply-templates select ="commands" mode="sum"/>
   	</body>
	</html>
</xsl:template>

<xsl:template match="commands" mode="toc">
	<h2>catogorie4</h2>
    <xsl:apply-templates select="command[catagories/catagorie='catogorie4']"/>
	<h2>catogorie3</h2>
    <xsl:apply-templates select="command[catagories/catagorie='catogorie3']"/>
	<h2>catogorie2</h2>
    <xsl:apply-templates select="command[catagories/catagorie='catogorie2']"/>
	<h2>catogorie1</h2>
    <xsl:apply-templates select="command[catagories/catagorie='catogorie1']"/>
</xsl:template>

<xsl:template match="command[catagories/catagorie]">
    <A HREF="#T_{name}" ><xsl:value-of select="name"/></A>
	<br/>
<!--        <xsl:apply-templates select="catagories"/-->
</xsl:template>

<xsl:template match="catagories">
	<p>	<xsl:value-of select="../../name"/> </p>
    <xsl:for-each select="catagorie">
    <A HREF="#T_{.}" ><xsl:value-of select="."/></A>
    <BR/> 
    </xsl:for-each>
</xsl:template>


<xsl:template match="commands" mode="sum">
        <xsl:apply-templates select="command" mode="sum">
			 <xsl:sort select="name"/>
        </xsl:apply-templates>
</xsl:template>

<xsl:template match="command" mode="sum">

<A NAME="T_{name}" ></A>
<TABLE BORDER="5" CELLSPACING="2" CELLPADDING="2" WIDTH="600" BGCOLOR="#CCCCCC">
<TR BGCOLOR="#333333">
  <TH COLSPAN="2">
    <FONT COLOR="#FFFF00" SIZE="+1"><xsl:value-of select="name"/></FONT>
  </TH>
</TR>
<xsl:apply-templates select="menu"/>
<xsl:apply-templates select="short"/>
<xsl:apply-templates select="syntax"/>
<xsl:apply-templates select="long"/>
<xsl:apply-templates select="example"/>
<xsl:apply-templates select="notes"/>
<xsl:apply-templates select="relations"/>
</TABLE>
</xsl:template>

<xsl:template match="short">
<TR >
  <TD VALIGN="TOP">
    <FONT COLOR="#CC66CC">Short</FONT>
  </TD>
  <TD VALIGN="TOP" width="525">
    <FONT COLOR="#CC66CC"><xsl:value-of select="."/></FONT>
  </TD>
</TR>
</xsl:template>

<xsl:template match="menu">
<TR >
  <TD VALIGN="TOP">
    <FONT COLOR="#CC66CC">Menu</FONT>
  </TD>
  <TD VALIGN="TOP" width="525">
    <FONT COLOR="#CC66CC"><xsl:value-of select="."/></FONT>
  </TD>
</TR>
</xsl:template>


<xsl:template match="syntax">
<TR>
  <TD VALIGN="TOP">
    <I><FONT COLOR="#CC0000">Syntax</FONT></I>
  </TD>
  <TD VALIGN="TOP">
    <I><FONT COLOR="#CC0000">
        <xsl:apply-templates/>
    </FONT></I>
  </TD>
</TR>
</xsl:template>

<xsl:template match="long">
<TR>
  <TD VALIGN="TOP">
    Description
  </TD>
  <TD VALIGN="TOP">
        <xsl:apply-templates/>
  </TD>
</TR>
</xsl:template>

<xsl:template match="example">
<TR>
  <TD VALIGN="TOP">
    Example
  </TD>
  <TD VALIGN="TOP">
        <xsl:apply-templates/>
  </TD>
</TR>
</xsl:template>

<xsl:template match="notes">
<TR>
  <TD VALIGN="TOP">
    Notes
  </TD>
  <TD VALIGN="TOP">
    <UL>
    <xsl:for-each select="note">
    <LI><xsl:value-of select="."/></LI>
    </xsl:for-each>
    </UL>
  </TD>
</TR>
</xsl:template>

<xsl:template match="relations">
<TR>
  <TD VALIGN="TOP">
    Related <BR/>commands
  </TD>
  <TD VALIGN="TOP">
    <xsl:for-each select="relation">
    <A HREF="#T_{.}" ><xsl:value-of select="."/></A>
    <BR/> 
    </xsl:for-each>
  </TD>
</TR>
</xsl:template>

</xsl:stylesheet>

