<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

<!-- ********************************************************************
     $Id: verbatim.xsl,v 1.2 2000/10/26 01:08:46 ndw Exp $
     ********************************************************************

     This file is part of the XSL DocBook Stylesheet distribution.
     See ../README or http://nwalsh.com/docbook/xsl/ for copyright
     and other information.

     ******************************************************************** -->

<xsl:template match="programlisting|screen|literallayout[@class='monospaced']">
  <pre class="{name(.)}"><xsl:apply-templates/></pre>
</xsl:template>

<xsl:template match="literallayout">
  <xsl:choose>
    <xsl:when test="@class='monospaced'">
      <pre>
        <xsl:apply-templates/>
      </pre>
    </xsl:when>
    <xsl:otherwise>
      <div class="{name(.)}">
        <xsl:apply-templates/>
      </div>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="literallayout[not(@class)
                                   or @class != 'monospaced']//text()">
  <xsl:call-template name="make-verbatim">
    <xsl:with-param name="text" select="."/>
  </xsl:call-template>
</xsl:template>

<xsl:template match="address">
  <div class="{name(.)}">
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="address//text()">
  <xsl:call-template name="make-verbatim">
    <xsl:with-param name="text" select="."/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="make-verbatim">
  <xsl:param name="text" select="''"/>

  <xsl:variable name="starts-with-space"
                select="substring($text, 1, 1) = ' '"/>

  <xsl:variable name="starts-with-nl"
                select="substring($text, 1, 1) = '&#xA;'"/>

  <xsl:variable name="before-space">
    <xsl:if test="contains($text, ' ')">
      <xsl:value-of select="substring-before($text, ' ')"/>
    </xsl:if>
  </xsl:variable>

  <xsl:variable name="before-nl">
    <xsl:if test="contains($text, '&#xA;')">
      <xsl:value-of select="substring-before($text, '&#xA;')"/>
    </xsl:if>
  </xsl:variable>

  <xsl:choose>
    <xsl:when test="$starts-with-space">
      <xsl:text>&#160;</xsl:text>
      <xsl:call-template name="make-verbatim">
        <xsl:with-param name="text" select="substring($text,2)"/>
      </xsl:call-template>
    </xsl:when>

    <xsl:when test="$starts-with-nl">
      <br/>
      <xsl:call-template name="make-verbatim">
        <xsl:with-param name="text" select="substring($text,2)"/>
      </xsl:call-template>
    </xsl:when>

    <!-- if the string before a space is shorter than the string before
         a newline, fix the space...-->
    <xsl:when test="$before-space != ''
                    and (string-length($before-space)
                         &lt; string-length($before-nl))">
      <xsl:value-of select="$before-space"/>
      <xsl:text>&#160;</xsl:text>
      <xsl:call-template name="make-verbatim">
        <xsl:with-param name="text" select="substring-after($text, ' ')"/>
      </xsl:call-template>
    </xsl:when>

    <!-- if the string before a newline is shorter than the string before
         a space, fix the newline...-->
    <xsl:when test="$before-nl != ''
                    and (string-length($before-nl)
                         &lt; string-length($before-space))">
      <xsl:value-of select="$before-nl"/>
      <br/>
      <xsl:call-template name="make-verbatim">
        <xsl:with-param name="text" select="substring-after($text, '&#xA;')"/>
      </xsl:call-template>
    </xsl:when>

    <!-- the string before the newline and the string before the
         space are the same; which means they must both be empty -->
    <xsl:otherwise>
      <xsl:value-of select="$text"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
