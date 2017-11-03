<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:doc="http://nwalsh.com/xsl/documentation/1.0"
                exclude-result-prefixes="doc"
                version='1.0'>

<xsl:output method="xml"
            indent="no"/>

<!-- ********************************************************************
     $Id: docbook.xsl,v 1.3 2000/10/26 01:02:54 ndw Exp $
     ********************************************************************

     This file is part of the XSL DocBook Stylesheet distribution.
     See ../README or http://nwalsh.com/docbook/xsl/ for copyright
     and other information.

     ******************************************************************** -->

<!-- ==================================================================== -->

<xsl:include href="../VERSION"/>
<xsl:include href="../lib/lib.xsl"/>
<xsl:include href="../common/l10n.xsl"/>
<xsl:include href="../common/common.xsl"/>
<xsl:include href="autotoc.xsl"/>
<xsl:include href="lists.xsl"/>
<xsl:include href="callout.xsl"/>
<xsl:include href="verbatim.xsl"/>
<xsl:include href="graphics.xsl"/>
<xsl:include href="xref.xsl"/>
<xsl:include href="formal.xsl"/>
<xsl:include href="table.xsl"/>
<xsl:include href="sections.xsl"/>
<xsl:include href="inline.xsl"/>
<xsl:include href="footnote.xsl"/>
<xsl:include href="fo.xsl"/>
<xsl:include href="info.xsl"/>
<xsl:include href="keywords.xsl"/>
<xsl:include href="division.xsl"/>
<xsl:include href="index.xsl"/>
<xsl:include href="toc.xsl"/>
<xsl:include href="refentry.xsl"/>
<xsl:include href="math.xsl"/>
<xsl:include href="admon.xsl"/>
<xsl:include href="component.xsl"/>
<xsl:include href="biblio.xsl"/>
<xsl:include href="glossary.xsl"/>
<xsl:include href="block.xsl"/>
<xsl:include href="qandaset.xsl"/>
<xsl:include href="synop.xsl"/>
<xsl:include href="titlepage.xsl"/>
<xsl:include href="titlepage.templates.xsl"/>
<xsl:include href="param.xsl"/>

<!-- ==================================================================== -->

<xsl:template match="*">
  <fo:block color="red">
    <xsl:text>&lt;</xsl:text>
    <xsl:value-of select="name(.)"/>
    <xsl:text>&gt;</xsl:text>
    <xsl:apply-templates/> 
    <xsl:text>&lt;/</xsl:text>
    <xsl:value-of select="name(.)"/>
    <xsl:text>&gt;</xsl:text>
  </fo:block>
</xsl:template>

<xsl:template match="text()">
  <xsl:value-of select="."/> 
</xsl:template>

<xsl:template match="processing-instruction()">
</xsl:template>

<xsl:template match="/">
  <xsl:variable name="document.element" select="*[1]"/>
  <xsl:variable name="title">
    <xsl:choose>
      <xsl:when test="$document.element/title[1]">
        <xsl:value-of select="$document.element/title[1]"/>
      </xsl:when>
      <xsl:otherwise>[could not find document title]</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="docinfo" 
                select="*[1]/artheader
                        |*[1]/articleinfo
                        |*[1]/sectioninfo
                        |*[1]/sect1info"/>

  <fo:root font-family="Times Roman"
           font-size="12pt"
           text-align="justify">
    <fo:layout-master-set>
      <fo:page-sequence-master master-name="pages">
	<fo:repeatable-page-master-alternatives>
	  <fo:conditional-page-master-reference
	    odd-or-even="odd" master-name="right"/>
	  <fo:conditional-page-master-reference
	    odd-or-even="even" master-name="left"/>
	</fo:repeatable-page-master-alternatives>
      </fo:page-sequence-master>
      <fo:simple-page-master
        master-name="right"
        margin-top="75pt"
        margin-bottom="25pt"
        margin-left="100pt"
        margin-right="50pt">
        <fo:region-body margin-bottom="50pt"/>
        <fo:region-after extent="25pt"/>
      </fo:simple-page-master>
      <fo:simple-page-master
        master-name="left"
        margin-top="75pt"
        margin-bottom="25pt"
        margin-left="50pt"
        margin-right="100pt">
        <fo:region-body margin-bottom="50pt"/>
        <fo:region-after extent="25pt"/>
      </fo:simple-page-master>
    </fo:layout-master-set>

    <xsl:apply-templates/>

  </fo:root>
</xsl:template>

<!-- ==================================================================== -->

</xsl:stylesheet>
