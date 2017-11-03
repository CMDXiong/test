<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:xlink="http://www.w3.org/1999/xlink"
                exclude-result-prefixes="xlink"
                version='1.0'>

<!-- ********************************************************************
     $Id: graphics.xsl,v 1.3 2000/10/26 00:59:25 ndw Exp $
     ********************************************************************

     This file is part of the XSL DocBook Stylesheet distribution.
     See ../README or http://nwalsh.com/docbook/xsl/ for copyright
     and other information.

     Contributors:
     Colin Paul Adams, <colin@colina.demon.co.uk>

     ******************************************************************** -->

<xsl:template match="screenshot">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="screeninfo">
</xsl:template>

<!-- ==================================================================== -->

<xsl:template name="process.image">
  <!-- When this template is called, the current node should be  -->
  <!-- a graphic, inlinegraphic, imagedata, or videodata. All    -->
  <!-- those elements have the same set of attributes, so we can -->
  <!-- handle them all in one place.                             -->
  <xsl:variable name="input-filename">
    <xsl:choose>
      <xsl:when test="@entityref">
        <xsl:value-of select="unparsed-entity-uri(@entityref)"/>
      </xsl:when>
      <xsl:when test="@fileref">
        <xsl:text>file:</xsl:text>
        <xsl:value-of select="@fileref"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:message>
          <xsl:text>Expected @entityref or @fileref on </xsl:text>
          <xsl:value-of select="name(.)"/>
        </xsl:message>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="fileext">
    <xsl:call-template name="filename-extension">
      <xsl:with-param name="filename" select="$input-filename"/>
    </xsl:call-template>
  </xsl:variable>

  <xsl:variable name="filename">
    <xsl:choose>
      <xsl:when test="$fileext != ''">
        <xsl:value-of select="$input-filename"/>
      </xsl:when>
      <xsl:when test="$graphic.default.extension != ''">
        <xsl:value-of select="$input-filename"/>
        <xsl:text>.</xsl:text>
        <xsl:value-of select="$graphic.default.extension"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$input-filename"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="scale">
    <xsl:choose>
      <xsl:when test="@scale"><xsl:value-of select="@scale"/>%</xsl:when>
      <xsl:otherwise>auto</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="width">
    <xsl:choose>
      <xsl:when test="@width"><xsl:value-of select="@width"/></xsl:when>
      <xsl:otherwise>auto</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="height">
    <xsl:choose>
      <xsl:when test="@depth"><xsl:value-of select="@depth"/></xsl:when>
      <xsl:otherwise>auto</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <fo:external-graphic src="{$filename}"
    content-width="{$scale}" content-height="{$scale}"
    width="{$width}" height="{$height}"/>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template match="graphic">
  <fo:block>
    <xsl:call-template name="process.image"/>
  </fo:block>
</xsl:template>

<xsl:template match="inlinegraphic">
  <xsl:choose>
    <xsl:when test="@format='linespecific'">
      <a xlink:type="simple" xlink:show="embed" xlink:actuate="onLoad">
        <xsl:choose>
          <xsl:when test="@entityref">
            <xsl:attribute name="href">
              <xsl:value-of select="unparsed-entity-uri(@entityref)"/>
            </xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
            <xsl:attribute name="href">
              <xsl:value-of select="@fileref"/>
            </xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
      </a>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="process.image"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template match="mediaobject">
  <fo:block>
    <xsl:call-template name="select.mediaobject"/>
    <xsl:apply-templates select="caption"/>
  </fo:block>
</xsl:template>

<xsl:template match="inlinemediaobject">
  <xsl:call-template name="select.mediaobject"/>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template match="imageobject">
  <xsl:apply-templates select="imagedata"/>
</xsl:template>

<xsl:template match="imagedata">
  <xsl:choose>
    <xsl:when test="@format='linespecific'">
      <a xlink:type="simple" xlink:show="embed" xlink:actuate="onLoad">
        <xsl:choose>
          <xsl:when test="@entityref">
            <xsl:attribute name="href">
              <xsl:value-of select="unparsed-entity-uri(@entityref)"/>
            </xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
            <xsl:attribute name="href">
              <xsl:value-of select="@fileref"/>
            </xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
      </a>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="process.image"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template match="videoobject">
  <xsl:apply-templates select="videodata"/>
</xsl:template>

<xsl:template match="videodata">
  <xsl:call-template name="process.image"/>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template match="audioobject">
  <xsl:apply-templates select="audiodata"/>
</xsl:template>

<xsl:template match="audiodata">
  <xsl:call-template name="process.image"/>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template match="textobject">
  <xsl:apply-templates/>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template match="caption">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

</xsl:stylesheet>
