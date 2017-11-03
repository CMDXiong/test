<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                version='1.0'>

<!-- ********************************************************************
     $Id: xref.xsl,v 1.3 2000/10/29 14:24:35 ndw Exp $
     ********************************************************************

     This file is part of the XSL DocBook Stylesheet distribution.
     See ../README or http://nwalsh.com/docbook/xsl/ for copyright
     and other information.

     ******************************************************************** -->

<!-- ==================================================================== -->

<xsl:template match="xref">
  <xsl:variable name="targets" select="id(@linkend)"/>
  <xsl:variable name="target" select="$targets[1]"/>
  <xsl:variable name="refelem" select="name($target)"/>

  <xsl:call-template name="check.id.unique">
    <xsl:with-param name="linkend" select="@linkend"/>
  </xsl:call-template>

  <xsl:choose>
    <xsl:when test="$refelem=''">
      <xsl:message>
	<xsl:text>XRef to nonexistent id: </xsl:text>
	<xsl:value-of select="@linkend"/>
      </xsl:message>
      <xsl:text>???</xsl:text>
    </xsl:when>

    <xsl:when test="$target/@xreflabel">
      <fo:basic-link internal-destination="{@linkend}">
	<xsl:call-template name="xref.xreflabel">
	  <xsl:with-param name="target" select="$target"/>
	</xsl:call-template>
      </fo:basic-link>
    </xsl:when>

    <xsl:otherwise>
      <fo:basic-link internal-destination="{@linkend}">
        <xsl:choose>
	  <xsl:when test="@endterm">
	    <xsl:variable name="etargets" select="id(@endterm)"/>
	    <xsl:variable name="etarget" select="$etargets[1]"/>
	    <xsl:choose>
	      <xsl:when test="count($etarget) = 0">
		<xsl:message>
		  <xsl:value-of select="count($etargets)"/>
		  <xsl:text>Endterm points to nonexistent ID: </xsl:text>
		  <xsl:value-of select="@endterm"/>
		</xsl:message>
		<xsl:text>???</xsl:text>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:apply-templates select="$etarget" mode="xref.text"/>
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:when>

          <xsl:when test="$refelem='figure'">
            <xsl:call-template name="xref.figure">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='example'">
            <xsl:call-template name="xref.example">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='table'">
            <xsl:call-template name="xref.table">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='equation'">
            <xsl:call-template name="xref.equation">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='cmdsynopsis'">
            <xsl:call-template name="xref.cmdsynopsis">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='funcsynopsis'">
            <xsl:call-template name="xref.funcsynopsis">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='dedication'">
            <xsl:call-template name="xref.dedication">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='preface'">
            <xsl:call-template name="xref.preface">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='chapter'">
            <xsl:call-template name="xref.chapter">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='appendix'">
            <xsl:call-template name="xref.appendix">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='bibliography'">
            <xsl:call-template name="xref.bibliography">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='biblioentry'">
            <xsl:call-template name="xref.biblioentry">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='bibliomixed'">
            <xsl:call-template name="xref.biblioentry">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='glossary'">
            <xsl:call-template name="xref.glossary">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='index'">
            <xsl:call-template name="xref.index">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='section'
                          or $refelem='simplesect'
                          or $refelem='sect1'
                          or $refelem='sect2'
                          or $refelem='sect3'
                          or $refelem='sect4'
                          or $refelem='sect5'
                          or $refelem='refsect1'
                          or $refelem='refsect2'
                          or $refelem='refsect3'">
            <xsl:call-template name="xref.section">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='listitem' 
                          and name($target/..)='orderedlist'">
            <xsl:apply-templates select="$target" mode="xref"/>
          </xsl:when>

          <xsl:when test="$refelem='question'">
            <xsl:call-template name="xref.question">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='answer'">
            <xsl:call-template name="xref.answer">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='part'">
            <xsl:call-template name="xref.part">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='reference'">
            <xsl:call-template name="xref.reference">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:when test="$refelem='book'">
            <xsl:call-template name="xref.book">
              <xsl:with-param name="target" select="$target"/>
            </xsl:call-template>
          </xsl:when>

          <xsl:otherwise>
	    <xsl:message>
	      <xsl:text>[Don't know what gentext to create for xref to: "</xsl:text>
	      <xsl:value-of select="$refelem"/>
	      <xsl:text>"]</xsl:text>
	    </xsl:message>
	    <xsl:text>???</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </fo:basic-link>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!--- ==================================================================== -->

<xsl:template name="cross-reference">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>
  <xsl:param name="xref.text">
    <xsl:call-template name="gentext.xref.text">
      <xsl:with-param name="element.name" select="$refelem"/>
      <xsl:with-param name="default">%g %n</xsl:with-param>
    </xsl:call-template>
  </xsl:param>

  <xsl:call-template name="subst.xref.text">
    <xsl:with-param name="xref.text" select="$xref.text"/>
    <xsl:with-param name="target" select="$target"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="xref.figure">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:call-template name="cross-reference">
    <xsl:with-param name="target" select="$target"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="xref.example">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:call-template name="cross-reference">
    <xsl:with-param name="target" select="$target"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="xref.table">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:call-template name="cross-reference">
    <xsl:with-param name="target" select="$target"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="xref.equation">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:call-template name="cross-reference">
    <xsl:with-param name="target" select="$target"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="xref.cmdsynopsis">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:variable name="command" select="($target//command)[1]"/>

  <xsl:apply-templates select="$command" mode="xref"/>
</xsl:template>

<xsl:template name="xref.funcsynopsis">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:variable name="func" select="($target//function)[1]"/>

  <xsl:apply-templates select="$func" mode="xref"/>
</xsl:template>

<xsl:template name="xref.dedication">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:call-template name="cross-reference">
    <xsl:with-param name="target" select="$target"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="xref.preface">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:call-template name="cross-reference">
    <xsl:with-param name="target" select="$target"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="xref.chapter">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:call-template name="cross-reference">
    <xsl:with-param name="target" select="$target"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="xref.appendix">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:call-template name="cross-reference">
    <xsl:with-param name="target" select="$target"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="xref.bibliography">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:call-template name="cross-reference">
    <xsl:with-param name="target" select="$target"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="xref.biblioentry">
  <!-- handles both biblioentry and bibliomixed -->
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:text>[</xsl:text>
  <xsl:choose>
    <xsl:when test="local-name($target/*[1]) = 'abbrev'">
      <xsl:apply-templates select="$target/*[1]"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="@id"/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:text>]</xsl:text>
</xsl:template>

<xsl:template name="xref.glossary">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:call-template name="cross-reference">
    <xsl:with-param name="target" select="$target"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="xref.index">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:call-template name="cross-reference">
    <xsl:with-param name="target" select="$target"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="xref.section">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:choose>
    <xsl:when test="$section.autolabel">
      <xsl:call-template name="gentext.element.name">
        <xsl:with-param name="element.name" select="$refelem"/>
      </xsl:call-template>
      <xsl:text> </xsl:text>
      <xsl:apply-templates select="$target" mode="label.content"/>

    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="gentext.element.name">
        <xsl:with-param name="element.name">the section called</xsl:with-param>
      </xsl:call-template>
      <xsl:text> </xsl:text>
      <xsl:call-template name="gentext.startquote"/>
      <xsl:apply-templates select="$target" mode="title.content"/>
      <xsl:call-template name="gentext.endquote"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="xref.question">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:call-template name="cross-reference">
    <xsl:with-param name="target" select="$target"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="xref.answer">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:call-template name="cross-reference">
    <xsl:with-param name="target" select="$target"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="xref.part">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:call-template name="cross-reference">
    <xsl:with-param name="target" select="$target"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="xref.reference">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:call-template name="cross-reference">
    <xsl:with-param name="target" select="$target"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="xref.book">
  <xsl:param name="target" select="."/>
  <xsl:param name="refelem" select="name($target)"/>

  <xsl:variable name="title">
    <xsl:choose>
      <xsl:when test="$target/title">
        <xsl:apply-templates select="$target/title" mode="xref"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="$target/bookinfo/title"
                             mode="xref"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <fo:inline font-style="italic">
    <xsl:copy-of select="$title"/>
  </fo:inline>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template match="link">
  <xsl:variable name="targets" select="id(@linkend)"/>
  <xsl:variable name="target" select="$targets[1]"/>

  <xsl:call-template name="check.id.unique">
    <xsl:with-param name="linkend" select="@linkend"/>
  </xsl:call-template>

  <fo:basic-link internal-destination="{@linkend}">
    <xsl:apply-templates/>
  </fo:basic-link>
</xsl:template>

<xsl:template match="ulink">
  <fo:basic-link external-destination="{@url}">
    <xsl:choose>
      <xsl:when test="count(child::node())=0">
	<xsl:value-of select="@url"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:apply-templates/>
      </xsl:otherwise>
    </xsl:choose>
  </fo:basic-link>
  <xsl:if test="count(child::node()) != 0">
    <xsl:text> [</xsl:text>
    <xsl:value-of select="@url"/>
    <xsl:text>]</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="olink">
  <xsl:apply-templates/>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template name="title.xref">
  <xsl:param name="target" select="."/>
  <xsl:choose>
    <xsl:when test="name($target) = 'figure'
                    or name($target) = 'example'
                    or name($target) = 'equation'
                    or name($target) = 'table'
                    or name($target) = 'dedication'
                    or name($target) = 'preface'
                    or name($target) = 'bibliography'
                    or name($target) = 'glossary'
                    or name($target) = 'index'
                    or name($target) = 'setindex'
                    or name($target) = 'colophon'">
      <xsl:call-template name="gentext.startquote"/>
      <xsl:apply-templates select="$target" mode="title.content"/>
      <xsl:call-template name="gentext.endquote"/>
    </xsl:when>
    <xsl:otherwise>
      <i>
        <xsl:apply-templates select="$target" mode="title.content"/>
      </i>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="number.xref">
  <xsl:param name="target" select="."/>
  <xsl:apply-templates select="$target" mode="label.content"/>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template name="xref.xreflabel">
  <!-- called to process an xreflabel...you might use this to make  -->
  <!-- xreflabels come out in the right font for different targets, -->
  <!-- for example. -->
  <xsl:param name="target" select="."/>
  <xsl:value-of select="$target/@xreflabel"/>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template match="title" mode="xref">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="command" mode="xref">
  <xsl:call-template name="inline.boldseq"/>
</xsl:template>

<xsl:template match="function" mode="xref">
  <xsl:call-template name="inline.monoseq"/>
</xsl:template>

</xsl:stylesheet>
