<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:xlink="http://www.w3.org/1999/xlink"
                exclude-result-prefixes="xlink"
                version='1.0'>

<!-- ********************************************************************
     $Id: graphics.xsl,v 1.4 2000/10/26 01:05:14 ndw Exp $
     ********************************************************************

     This file is part of the XSL DocBook Stylesheet distribution.
     See ../README or http://nwalsh.com/docbook/xsl/ for copyright
     and other information.

     Contributors:
     Colin Paul Adams, <colin@colina.demon.co.uk>

     ******************************************************************** -->

<!-- ==================================================================== -->

<xsl:template match="screenshot">
  <div class="{name(.)}">
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="screeninfo">
</xsl:template>

<!-- ==================================================================== -->

<xsl:template name="process.image">
  <!-- When this template is called, the current node should be  -->
  <!-- a graphic, inlinegraphic, imagedata, or videodata. All    -->
  <!-- those elements have the same set of attributes, so we can -->
  <!-- handle them all in one place.                             -->
  <xsl:param name="tag" select="'img'"/>
  <xsl:param name="alt"/>

  <xsl:variable name="input-filename">
    <xsl:choose>
      <xsl:when test="@entityref">
        <xsl:value-of select="unparsed-entity-uri(@entityref)"/>
      </xsl:when>
      <xsl:when test="@fileref">
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
        <xsl:value-of select="$graphic.default.extension"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$input-filename"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="width">
    <xsl:choose>
      <xsl:when test="@scale"><xsl:value-of select="@scale"/>%</xsl:when>
      <xsl:when test="@width"><xsl:value-of select="@width"/></xsl:when>
      <xsl:otherwise></xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="height">
    <xsl:choose>
      <xsl:when test="@scale"></xsl:when>
      <xsl:when test="@depth"><xsl:value-of select="@depth"/></xsl:when>
      <xsl:otherwise></xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="align">
    <xsl:value-of select="@align"/>
  </xsl:variable>

  <xsl:element name="{$tag}">
    <xsl:attribute name="src">
      <xsl:value-of select="$filename"/>
    </xsl:attribute>

    <xsl:if test="$align != ''">
      <xsl:attribute name="align">
        <xsl:value-of select="$align"/>
      </xsl:attribute>
    </xsl:if>
    <xsl:if test="$height != ''">
      <xsl:attribute name="height">
        <xsl:value-of select="$height"/>
      </xsl:attribute>
    </xsl:if>
    <xsl:if test="$width != ''">
      <xsl:attribute name="width">
        <xsl:value-of select="$width"/>
      </xsl:attribute>
    </xsl:if>
    <xsl:if test="$alt != ''">
      <xsl:attribute name="alt">
        <xsl:value-of select="$alt"/>
      </xsl:attribute>
    </xsl:if>
  </xsl:element>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template match="graphic">
  <p>
    <xsl:call-template name="process.image"/>
  </p>
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
  <div class="{name(.)}">
    <xsl:call-template name="select.mediaobject"/>
    <xsl:apply-templates select="caption"/>
  </div>
</xsl:template>

<xsl:template match="inlinemediaobject">
  <span class="{name(.)}">
    <xsl:call-template name="select.mediaobject"/>
  </span>
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
      <xsl:call-template name="process.image">
        <xsl:with-param name="alt">
          <xsl:apply-templates select="(../../textobject/phrase)[1]"/>
        </xsl:with-param>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template match="videoobject">
  <xsl:apply-templates select="videodata"/>
</xsl:template>

<xsl:template match="videodata">
  <xsl:call-template name="process.image">
    <xsl:with-param name="tag" select="'embed'"/>
    <xsl:with-param name="alt">
      <xsl:apply-templates select="(../../textobject/phrase)[1]"/>
    </xsl:with-param>
  </xsl:call-template>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template match="audioobject">
  <xsl:apply-templates select="audiodata"/>
</xsl:template>

<xsl:template match="audiodata">
  <xsl:call-template name="process.image">
    <xsl:with-param name="tag" select="'embed'"/>
    <xsl:with-param name="alt">
      <xsl:apply-templates select="(../../textobject/phrase)[1]"/>
    </xsl:with-param>
  </xsl:call-template>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template match="textobject">
  <xsl:apply-templates/>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template match="caption">
  <div class="{name(.)}">
    <xsl:apply-templates/>
  </div>
</xsl:template>

</xsl:stylesheet>
