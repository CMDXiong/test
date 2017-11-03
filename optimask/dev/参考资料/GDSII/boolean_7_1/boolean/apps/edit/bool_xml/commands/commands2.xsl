<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
               version='1.0'>

<xsl:include href="../docbook/html/docbook.xsl"/>

<xsl:template match="/">
	<html>
        <xsl:apply-templates/>
	</html>
</xsl:template>

<xsl:template match="commands">
	<head>
	   <TITLE>Command Documentation</TITLE>
    </head>
    <body bgcolor="#99CCFF">
        <xsl:apply-templates/>
   	</body>
</xsl:template>

<xsl:template match="command">
<xsl:choose>
    <xsl:when test="name/@anchor!=''">
	<A NAME="T_{name/@anchor}" target="commands"></A>
    </xsl:when>
    <xsl:otherwise>
	<A NAME="T_{name}" target="commands"></A>
    </xsl:otherwise>
</xsl:choose>

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
<xsl:apply-templates select="catagories"/>
</TABLE>
<p></p>
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
    Related 
  </TD>
  <TD VALIGN="TOP">
        <xsl:apply-templates/>
  </TD>
</TR>
</xsl:template>

<xsl:template match="relation">
    <A HREF="#T_{@link}" target="commands"><xsl:apply-templates/></A> 
</xsl:template>

<xsl:template match="catagories">
<TR>
  <TD VALIGN="TOP">
    Catagory 
  </TD>
  <TD VALIGN="TOP">
        <xsl:apply-templates/>
  </TD>
</TR>
</xsl:template>

<xsl:template match="catagorie">
    <A HREF="toc.html#T_{.}" target="toc"><xsl:value-of select="."/></A> 
</xsl:template>

</xsl:stylesheet>

