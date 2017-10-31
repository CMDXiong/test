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
		<h2><A NAME="T_environment"></A>environment</h2>
        <xsl:apply-templates select="command[catagories/catagorie='environment']"/>
		<h2><A NAME="T_files"></A>files</h2>
        <xsl:apply-templates select="command[catagories/catagorie='files']"/>
		<h2><A NAME="T_interface"></A>interface</h2>
        <xsl:apply-templates select="command[catagories/catagorie='interface']"/>
		<h2><A NAME="T_settings"></A>settings</h2>
        <xsl:apply-templates select="command[catagories/catagorie='settings']"/>
		<h2><A NAME="T_operations"></A>operations</h2>
        <xsl:apply-templates select="command[catagories/catagorie='operations']"/>
		<h2><A NAME="T_groupoperations"></A>group operations</h2>
        <xsl:apply-templates select="command[catagories/catagorie='groupoperations']"/>
		<h2><A NAME="T_booleanoperations"></A>boolean operations</h2>
        <xsl:apply-templates select="command[catagories/catagorie='booleanoperations']"/>
		<h2><A NAME="T_transform"></A>transform</h2>
        <xsl:apply-templates select="command[catagories/catagorie='transform']"/>
		<h2><A NAME="T_groups"></A>groups</h2>
        <xsl:apply-templates select="command[catagories/catagorie='groups']"/>
		<h2><A NAME="T_layers"></A>layer settings</h2>
        <xsl:apply-templates select="command[catagories/catagorie='layers']"/>
		<h2><A NAME="T_display"></A>display settings</h2>
        <xsl:apply-templates select="command[catagories/catagorie='display']"/>
		<h2><A NAME="T_automation"></A>automatic processing</h2>
        <xsl:apply-templates select="command[catagories/catagorie='automation']"/>
		<h2><A NAME="T_all"></A>all</h2>
        <xsl:apply-templates select="command">
				<xsl:sort select="name"/>
        </xsl:apply-templates>
   	</body>
</xsl:template>

<xsl:template match="command[catagories/catagorie]">
<xsl:choose>
    <xsl:when test="name/@anchor!=''">
	<A HREF="commands.html#T_{name/@anchor}" target="commands"><xsl:value-of select="name"/></A>
    </xsl:when>
    <xsl:otherwise>
	<A HREF="commands.html#T_{name}" target="commands"><xsl:value-of select="name"/></A>
    </xsl:otherwise>
</xsl:choose>
<br/>
<!--        <xsl:apply-templates select="catagories"/-->
</xsl:template>

<xsl:template match="catagories">
	<p>	<xsl:value-of select="../../name"/> </p>
    <xsl:for-each select="catagorie">
    <A HREF="commands.html#T_{.}" target="commands"><xsl:value-of select="."/></A>
    <BR/> 
    </xsl:for-each>
</xsl:template>

<xsl:template match="command">
<xsl:choose>
    <xsl:when test="name/@anchor!=''">
	<A HREF="commands.html#T_{name/@anchor}" target="commands"><xsl:value-of select="name"/></A>
    </xsl:when>
    <xsl:otherwise>
	<A HREF="commands.html#T_{name}" target="commands"><xsl:value-of select="name"/></A>
    </xsl:otherwise>
</xsl:choose>
<br/>
<!--        <xsl:apply-templates select="catagories"/-->
</xsl:template>

</xsl:stylesheet>

