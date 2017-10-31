<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:doc="http://nwalsh.com/xsl/documentation/1.0"
                exclude-result-prefixes="doc"
                version='1.0'>

<!-- ********************************************************************
     $Id: param.xsl,v 1.5 2000/10/26 01:02:07 ndw Exp $
     ********************************************************************

     This file is part of the XSL DocBook Stylesheet distribution.
     See ../README or http://nwalsh.com/docbook/xsl/ for copyright
     and other information.

     ******************************************************************** -->

<doc:reference xmlns="">
<referenceinfo>
<releaseinfo role="meta">
$Id: param.xsl,v 1.5 2000/10/26 01:02:07 ndw Exp $
</releaseinfo>
<author><surname>Walsh</surname>
<firstname>Norman</firstname></author>
<copyright><year>1999</year><year>2000</year>
<holder>Norman Walsh</holder>
</copyright>
</referenceinfo>
<title>Formatting Object Paramter Reference</title>

<partintro>
<section><title>Introduction</title>

<para>This is technical reference documentation for the DocBook XSL
Stylesheets; it documents (some of) the parameters, templates, and
other elements of the stylesheets.</para>

<para>This reference describes each of the Formatting Object
Stylesheet parameters.  These are the <quote>easily
customizable</quote> parts of the stylesheet.  If you want to specify
an alternate value for one or more of these parameters, you can do so
in a <quote>driver</quote> stylesheet.</para>

<para>For example, if you want to turn on automatic section numbering,
you might create a driver stylesheet like this:</para>

<programlisting><![CDATA[<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

  <xsl:import href="/path/to/fo/docbook.xsl"/>

  <xsl:param name="section.autolabel" select="1"/>

</xsl:stylesheet>]]></programlisting>

<para>Naturally, you have to change the
<sgmltag class='attribute'>href</sgmltag> attribute on
<literal>&lt;xsl:import&gt;</literal>
to point to <filename>docbook.xsl</filename>
on your system.</para>

<para>This is not intended to be <quote>user</quote> documentation.
It is provided for developers writing customization layers for the
stylesheets, and for anyone who's interested in <quote>how it
works</quote>.</para>

<para>Although I am trying to be thorough, this documentation is known
to be incomplete. Don't forget to read the source, too :-)</para>
</section>
</partintro>
</doc:reference>

<xsl:param name="author.othername.in.middle" select="1"/>
<xsl:param name="html.stylesheet">docbook.css</xsl:param>
<xsl:param name="html.stylesheet.type">text/css</xsl:param>
<xsl:param name="refentry.xref.manvolnum" select="1"/>
<xsl:param name="show.comments" select="1"/>
<xsl:param name="funcsynopsis.style">kr</xsl:param>
<xsl:param name="funcsynopsis.decoration" select="1"/>
<xsl:param name="refentry.generate.name" select="1"/>

<xsl:param name="admon.graphics" select="0"/>
<xsl:param name="admon.graphics.path">../images/</xsl:param>

<xsl:param name="section.autolabel" select="0"/>
<xsl:param name="section.label.includes.component.label" select="0"/>
<xsl:param name="chapter.autolabel" select="1"/>
<xsl:param name="part.autolabel" select="0"/>
<xsl:param name="preface.autolabel" select="0"/>

<xsl:param name="biblioentry.item.separator">. </xsl:param>

<!-- ==================================================================== -->
<xsl:param name="qandadiv.autolabel" select="1"/>

<doc:variable name="qandadiv.autolabel" xmlns="">
<refpurpose>Are divisions in QAndASets enumerated?</refpurpose>
<refdescription>
<para>If true (non-zero), unlabeled qandadivs will be enumerated.
</para>
</refdescription>
</doc:variable>

<!-- ==================================================================== -->
<xsl:param name="qanda.inherit.numeration" select="0"/>

<doc:variable name="qanda.inherit.numeration" xmlns="">
<refpurpose>Does enumeration of QandASet components inherit the numeration of parent elements?</refpurpose>
<refdescription>
<para>If true (non-zero), numbered QandADiv elements and Questions and Answers inherit
the numeration of the ancestors of the QandASet.
</para>
</refdescription>
</doc:variable>

<!-- ==================================================================== -->

<xsl:param name="graphic.default.extension"></xsl:param>

<doc:variable name="graphic.default.extension" xmlns="">
<refpurpose>Default extension for graphic filenames</refpurpose>
<refdescription>
<para>If a <sgmltag>graphic</sgmltag> or <sgmltag>mediaobject</sgmltag>
includes a reference to a filename that does not include an extension,
and the <sgmltag class="attribute">format</sgmltag> attribute is
<emphasis>unspecified</emphasis>, the default extension will be used.
</para>
</refdescription>
</doc:variable>

<!-- ==================================================================== -->

<xsl:attribute-set name="formal.title.attrs">
  <xsl:attribute name="space-before.optimum">2em</xsl:attribute>
  <xsl:attribute name="space-before.minimum">1.8em</xsl:attribute>
  <xsl:attribute name="space-before.maximum">2.2em</xsl:attribute>
  <xsl:attribute name="font-weight">bold</xsl:attribute>
  <xsl:attribute name="font-size">12pt</xsl:attribute>
</xsl:attribute-set>

<doc:attribute-set name="formal.title.attrs" xmlns="">
<refpurpose>Properties of formal object titles</refpurpose>
<refdescription>
<para>This attribute set is used to specify the properties of formal
object titles.
</para>
</refdescription>
</doc:attribute-set>

<!-- ==================================================================== -->

<xsl:attribute-set name="component.title.attrs">
  <xsl:attribute name="space-before.optimum">2em</xsl:attribute>
  <xsl:attribute name="space-before.minimum">1.8em</xsl:attribute>
  <xsl:attribute name="space-before.maximum">2.2em</xsl:attribute>
  <xsl:attribute name="font-weight">bold</xsl:attribute>
  <xsl:attribute name="font-size">18pt</xsl:attribute>
  <xsl:attribute name="space-after.optimum">1.5em</xsl:attribute>
  <xsl:attribute name="space-after.minimum">1.3em</xsl:attribute>
  <xsl:attribute name="space-after.maximum">1.8em</xsl:attribute>
</xsl:attribute-set>

<doc:attribute-set name="component.title.attrs" xmlns="">
<refpurpose>Properties of component titles</refpurpose>
<refdescription>
<para>This attribute set is used to specify the properties of component
titles.
</para>
</refdescription>
</doc:attribute-set>

<!-- ==================================================================== -->

<xsl:attribute-set name="normal.para.spacing">
  <xsl:attribute name="space-before.optimum">1em</xsl:attribute>
  <xsl:attribute name="space-before.minimum">0.8em</xsl:attribute>
  <xsl:attribute name="space-before.maximum">1.2em</xsl:attribute>
</xsl:attribute-set>

<doc:attribute-set name="component.title.attrs" xmlns="">
<refpurpose>Spacing properties of normal paragraphs</refpurpose>
<refdescription>
<para>This attribute set is used to specify the spacing properties
of normal paragraphs.
</para>
</refdescription>
</doc:attribute-set>

</xsl:stylesheet>

