<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                version='1.0'>

<!-- ********************************************************************
     $Id: sections.xsl,v 1.4 2000/10/26 01:00:28 ndw Exp $
     ********************************************************************

     This file is part of the XSL DocBook Stylesheet distribution.
     See ../README or http://nwalsh.com/docbook/xsl/ for copyright
     and other information.

     ******************************************************************** -->

<!-- ==================================================================== -->

<xsl:template match="section">
  <xsl:variable name="id">
    <xsl:call-template name="object.id"/>
  </xsl:variable>

  <fo:block id="{$id}">
    <xsl:call-template name="section.titlepage"/>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="/section">
  <fo:page-sequence master-name="pages">
    <fo:static-content flow-name="xsl-region-after">
      <fo:block text-align-last="center" font-size="10pt">
        <fo:page-number/>
      </fo:block>
    </fo:static-content>

    <fo:flow>
      <xsl:call-template name="section.titlepage"/>
      <xsl:apply-templates/>
   </fo:flow>
  </fo:page-sequence>
</xsl:template>

<xsl:template match="title" mode="section.titlepage.recto.mode">
  <xsl:variable name="section" select="(ancestor::section
                                        |ancestor::simplesect
                                        |ancestor::sect1
                                        |ancestor::sect2
                                        |ancestor::sect3
                                        |ancestor::sect4
                                        |ancestor::sect5)[last()]"/>
  <xsl:call-template name="section.heading">
    <xsl:with-param name="level">
      <xsl:call-template name="section.level">
        <xsl:with-param name="node" select="$section"/>
      </xsl:call-template>
    </xsl:with-param>
    <xsl:with-param name="title">
      <xsl:apply-templates select="$section" mode="title.ref"/>
    </xsl:with-param>
  </xsl:call-template>
</xsl:template>

<xsl:template match="sect1">
  <xsl:variable name="id">
    <xsl:call-template name="object.id"/>
  </xsl:variable>

  <fo:block id="{$id}">
    <xsl:call-template name="sect1.titlepage"/>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="/sect1">
  <fo:page-sequence master-name="pages">
    <fo:static-content flow-name="xsl-region-after">
      <fo:block text-align-last="center" font-size="10pt">
        <fo:page-number/>
      </fo:block>
    </fo:static-content>

    <fo:flow>
      <xsl:call-template name="sect1.titlepage"/>
      <xsl:apply-templates/>
   </fo:flow>
  </fo:page-sequence>
</xsl:template>

<xsl:template match="title" mode="sect1.titlepage.recto.mode">
  <xsl:apply-templates select="." mode="section.titlepage.recto.mode"/>
</xsl:template>

<xsl:template match="sect2">
  <xsl:variable name="id">
    <xsl:call-template name="object.id"/>
  </xsl:variable>

  <fo:block id="{$id}">
    <xsl:call-template name="sect2.titlepage"/>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="title" mode="sect2.titlepage.recto.mode">
  <xsl:apply-templates select="." mode="section.titlepage.recto.mode"/>
</xsl:template>

<xsl:template match="sect3">
  <xsl:variable name="id">
    <xsl:call-template name="object.id"/>
  </xsl:variable>

  <fo:block id="{$id}">
    <xsl:call-template name="sect3.titlepage"/>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="title" mode="sect3.titlepage.recto.mode">
  <xsl:apply-templates select="." mode="section.titlepage.recto.mode"/>
</xsl:template>

<xsl:template match="sect4">
  <xsl:variable name="id">
    <xsl:call-template name="object.id"/>
  </xsl:variable>

  <fo:block id="{$id}">
    <xsl:call-template name="sect4.titlepage"/>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="title" mode="sect4.titlepage.recto.mode">
  <xsl:apply-templates select="." mode="section.titlepage.recto.mode"/>
</xsl:template>

<xsl:template match="sect5">
  <xsl:variable name="id">
    <xsl:call-template name="object.id"/>
  </xsl:variable>

  <fo:block id="{$id}">
    <xsl:call-template name="sect5.titlepage"/>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="title" mode="sect5.titlepage.recto.mode">
  <xsl:apply-templates select="." mode="section.titlepage.recto.mode"/>
</xsl:template>

<xsl:template match="simplesect">
  <xsl:variable name="id">
    <xsl:call-template name="object.id"/>
  </xsl:variable>

  <fo:block id="{$id}">
    <xsl:call-template name="simplesect.titlepage"/>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="title" mode="simplesect.titlepage.recto.mode">
  <xsl:apply-templates select="." mode="section.titlepage.recto.mode"/>
</xsl:template>

<xsl:template match="section/title"></xsl:template>
<xsl:template match="sectioninfo"></xsl:template>

<xsl:template match="sect1/title"></xsl:template>
<xsl:template match="sect1info"></xsl:template>

<xsl:template match="sect2/title"></xsl:template>
<xsl:template match="sect2info"></xsl:template>

<xsl:template match="sect3/title"></xsl:template>
<xsl:template match="sect3info"></xsl:template>

<xsl:template match="sect4/title"></xsl:template>
<xsl:template match="sect4info"></xsl:template>

<xsl:template match="sect5/title"></xsl:template>
<xsl:template match="sect5info"></xsl:template>

<xsl:template match="simplesect/title"></xsl:template>

<!-- ==================================================================== -->

<xsl:template name="section.heading">
  <xsl:param name="level">1</xsl:param>
  <xsl:param name="title"></xsl:param>
  <xsl:variable name="fsize">
    <xsl:choose>
      <xsl:when test="$level=1">18</xsl:when>
      <xsl:when test="$level=2">16</xsl:when>
      <xsl:when test="$level=3">14</xsl:when>
      <xsl:when test="$level=4">12</xsl:when>
      <xsl:when test="$level=5">12</xsl:when>
      <xsl:otherwise>10</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <fo:block font-size="{$fsize}pt" 
            font-weight="bold"
            space-before.minimum="1em"
            space-before.optimum="1.5em"
            space-before.maximum="2em">
    <xsl:copy-of select="$title"/>
  </fo:block>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template match="bridgehead">
  <!-- need to calculate depth! -->
  <fo:block font-size="16pt" font-weight="bold">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

</xsl:stylesheet>

